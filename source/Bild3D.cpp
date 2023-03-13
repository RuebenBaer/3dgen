/* TODO (Ansi#1#): Zuschnittgröße beim Laden neues Teilbild nullen*/
/* DONE (Ansi#1#): Grau und Schwarzweißsteuerung beim Speichern pruefen*/
/* DONE (Ansi#2#): Alle Funktionen auf notwendige Aufrufe von
                    'Zuschnitt'-Schnittstellenfunktionen absuchen */
/* DONE (Ansi#2#): Aufrufe der Schnittstellenfunktionen implementieren */
/* DONE (Ansi#1#): nicht vorhandenen Speicher von GroesseDCX...Y suchen und abstellen */

/*  3DGenerator - merges two pictures into one anaglyph using
    the 'red' value of the 'right' picture and the 'blue' and 'green' values of the left picture for red-cyan-glasses
    the 'red' and 'blue' value of the 'right' picture and the 'green' values of the left picture for magenta-green-glasses
    Copyright (C) 2012-2013  Ansgar Rütten

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <wx/imagjpeg.h>
#include <wx/font.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "Bild3D.h"
#include <cmath>

#define PI 3.14159265358979
using namespace std;

int F_Min(int a, int b)
{
    return((a < b) ? a : b);
}

int F_Max(int a, int b)
{
    return((a > b) ? a : b);
}

Bild3D::Bild3D()
{
    wxImageHandler *JPEGHandler = new wxJPEGHandler();
    wxImage::AddHandler(JPEGHandler);
    wxImageHandler *PNGHandler = new wxPNGHandler();
    wxImage::AddHandler(PNGHandler);

    LinkerPfad = wxT("Bilder/LinksKeinBild.jpg");
    LinkesOriginal.LoadFile(LinkerPfad, wxBITMAP_TYPE_JPEG);
    if(!(LinkesOriginal.Ok()))throw(wxT("Laden des linken Bildes fehlgeschlagen"));
    LinkesBild = LinkesOriginal.Copy();
    GroesseLinks.SetKoordinaten(LinkesBild.GetWidth(), 0, LinkesBild.GetHeight());

    RechterPfad = wxT("Bilder/RechtsKeinBild.jpg");
    RechtesOriginal.LoadFile(RechterPfad, wxBITMAP_TYPE_JPEG);
    if(!(RechtesOriginal.Ok()))throw(wxT("Laden des rechten Bildes fehlgeschlagen"));
    RechtesBild = RechtesOriginal.Copy();
    GroesseRechts.SetKoordinaten(RechtesBild.GetWidth(), 0, RechtesBild.GetHeight());

    Anaglyphe.LoadFile(RechterPfad, wxBITMAP_TYPE_JPEG);

    Offset.SetKoordinaten(100, 0, 0);
    //deltaOffset.SetKoordinaten(0, 0, 0);

    MinGroessenFinden();

    //Startwerte setzen
    AnzeigeVergroesserung=1;
    LinkeDrehung=0;
    deltaLinkeDrehung=0;
    RechteDrehung=0;
    deltaRechteDrehung=0;
    LinksAktiv=1;
    SWSchwellen=5;

    randProzent = 0.1;

    /*Zuschnitt vorbereiten*/
    Zuschnitt = new AuswahlRechteck(wxRect(-3000, -3000, 6000, 6000));
    zuschnittAktiv = 0;
    PfeilDefault = new wxCursor(wxCURSOR_ARROW);
}

Bild3D::Bild3D(const Bild3D &Obj)
{
    if(this != &Obj)
    {
        LinkerPfad = Obj.LinkerPfad;
        RechterPfad = Obj.RechterPfad;
        AnzeigeVergroesserung = Obj.AnzeigeVergroesserung;
        LinkeDrehung = Obj.LinkeDrehung;
        deltaLinkeDrehung = Obj.deltaLinkeDrehung;
        RechteDrehung = Obj.RechteDrehung;
        deltaRechteDrehung = Obj.deltaRechteDrehung;
        LinksAktiv = Obj.LinksAktiv;
        SWSchwellen = Obj.SWSchwellen;
        randProzent = Obj.randProzent;

        Offset.SetKoordinaten(Obj.Offset.x(), Obj.Offset.y(), Obj.Offset.z());
        deltaOffset.SetKoordinaten(Obj.deltaOffset.x(), Obj.deltaOffset.y(), Obj.deltaOffset.z());
        DrehPunkt.SetKoordinaten(Obj.DrehPunkt.x(), Obj.DrehPunkt.y(), Obj.DrehPunkt.z());

        LinkesOriginal.LoadFile(LinkerPfad, wxBITMAP_TYPE_JPEG);
        if(!(LinkesOriginal.Ok()))throw(wxT("Laden des linken Bildes fehlgeschlagen"));
        LinkesBild = LinkesOriginal.Copy();
        GroesseLinks.SetKoordinaten(LinkesBild.GetWidth(), 0, LinkesBild.GetHeight());

        RechtesOriginal.LoadFile(RechterPfad, wxBITMAP_TYPE_JPEG);
        if(!(RechtesOriginal.Ok()))throw(wxT("Laden des rechten Bildes fehlgeschlagen"));
        RechtesBild = RechtesOriginal.Copy();
        GroesseRechts.SetKoordinaten(RechtesBild.GetWidth(), 0, RechtesBild.GetHeight());

        Anaglyphe.LoadFile(RechterPfad, wxBITMAP_TYPE_JPEG);

        /*Zuschnitt vorbereiten*/
        Zuschnitt = new AuswahlRechteck(Obj.Zuschnitt->GetAuswahl());
        zuschnittAktiv = Obj.zuschnittAktiv;
        PfeilDefault = new wxCursor(wxCURSOR_ARROW);

        MinGroessenFinden();

        aktuellerEffekt = Obj.aktuellerEffekt;
        EffektPruefen();
    }

}

Bild3D::~Bild3D()
{
    LinkesBild.Destroy();
    LinkesOriginal.Destroy();
    RechtesBild.Destroy();
    RechtesOriginal.Destroy();
    Anaglyphe.Destroy();

    delete PfeilDefault;
    delete Zuschnitt;
}

/*Bild bearbeiten*/

void Bild3D::InGrauUmwandeln(void)
{
    unsigned char MittelFarbe;
    unsigned char* BildDaten;
    wxImage* WandelBild;

    WandelBild = &LinkesBild;
    if(WandelBild->Ok())
    {
        BildDaten = WandelBild->GetData();
        for(int i=0; i<((WandelBild->GetWidth()*WandelBild->GetHeight())*3); i+=3)
        {
            MittelFarbe = static_cast<unsigned char>(BildDaten[i]*0.299f+BildDaten[i+1]*0.587f+BildDaten[i+2]*0.114f);
            BildDaten[i] = BildDaten[i+1] = BildDaten[i+2] = MittelFarbe;
        }
    }
    WandelBild = &RechtesBild;
    if(WandelBild->Ok())
    {
        BildDaten = WandelBild->GetData();
        for(int i=0; i<((WandelBild->GetWidth()*WandelBild->GetHeight())*3); i+=3)
        {
            MittelFarbe = static_cast<unsigned char>(BildDaten[i]*0.299f+BildDaten[i+1]*0.587f+BildDaten[i+2]*0.114f);
            BildDaten[i] = BildDaten[i+1] = BildDaten[i+2] = MittelFarbe;
        }
    }
    return;
}

void Bild3D::InSWUmwandeln(void)
{
    unsigned char MittelFarbe;
    unsigned char* BildDaten;
    wxImage* WandelBild;

    float SchattenSprung;
    SchattenSprung = 255 / (SWSchwellen);

    WandelBild = &LinkesBild;
    if(WandelBild->Ok())
    {
        BildDaten = WandelBild->GetData();
        for(int i=0; i<((WandelBild->GetWidth()*WandelBild->GetHeight())*3); i+=3)
        {
            MittelFarbe = static_cast<unsigned char>(BildDaten[i]*0.299f+BildDaten[i+1]*0.587f+BildDaten[i+2]*0.114f);
            MittelFarbe += static_cast<unsigned char>(SchattenSprung/2 - Rest((MittelFarbe + SchattenSprung/2), SchattenSprung));
			BildDaten[i] = BildDaten[i+1] = BildDaten[i+2] = MittelFarbe;
        }
    }
    WandelBild = &RechtesBild;
    if(WandelBild->Ok())
    {
        BildDaten = WandelBild->GetData();
        for(int i=0; i<((WandelBild->GetWidth()*WandelBild->GetHeight())*3); i+=3)
        {
            MittelFarbe = static_cast<unsigned char>(BildDaten[i]*0.299f+BildDaten[i+1]*0.587f+BildDaten[i+2]*0.114f);
            MittelFarbe += static_cast<unsigned char>(SchattenSprung/2 - Rest((MittelFarbe + SchattenSprung/2), SchattenSprung));
			BildDaten[i] = BildDaten[i+1] = BildDaten[i+2] = MittelFarbe;
        }
    }
    return;
}

void Bild3D::TauscheLinksRechts(void)
{
    if(LinkesBild.Ok() && RechtesBild.Ok())
    {
        wxImage TempBild = LinkesBild;
        LinkesBild = RechtesBild;
        RechtesBild = TempBild;

        TempBild = LinkesOriginal;
        LinkesOriginal = RechtesOriginal;
        RechtesOriginal = TempBild;

        wxString TempPfad = LinkerPfad;
        LinkerPfad = RechterPfad;
        RechterPfad = TempPfad;

        Offset.SetKoordinaten(-(Offset.x()), 0, -(Offset.z()));
        deltaOffset.SetKoordinaten(-(deltaOffset.x()), 0, -(deltaOffset.z()));

        Vektor tempVektor=GroesseLinks;
        GroesseLinks=GroesseRechts;
        GroesseRechts=tempVektor;

        double tempDouble=LinkeDrehung;
        LinkeDrehung=RechteDrehung;
        RechteDrehung=tempDouble;

        tempDouble=deltaLinkeDrehung;
        deltaLinkeDrehung=deltaRechteDrehung;
        deltaRechteDrehung=tempDouble;
    }
    return;
}

void Bild3D::SetzeDrehpunkt(wxPoint Pkt)
{
    DrehPunkt.SetKoordinaten(Pkt.x, 0, Pkt.y);
    DrehPunkt/=AnzeigeVergroesserung;
    return;
}

wxPoint Bild3D::Drehen(double Winkel)
{
    Vektor AB, AO, BO, MAN, DP, H, H_, MA, MB, MA_, AO_, BO_, MAN_, DP_, AB_;

    AB=Offset * AnzeigeVergroesserung;
    DP=DrehPunkt * AnzeigeVergroesserung;

    if(LinksAktiv)
    {
        MA = GroesseLinks/2;
        MB = GroesseRechts/2;
        deltaLinkeDrehung+=Winkel;
    }else{
        AB*=(-1);
        MA = GroesseRechts/2;
        MB = GroesseLinks/2;
        deltaRechteDrehung+=Winkel;
    }

    if(AB.x()>0)
    {
        AO.SetKoordinaten(0, AB.x());
        BO.SetKoordinaten(0, 0.);
    }else{
        AO.SetKoordinaten(0, 0.);
        BO.SetKoordinaten(0, AB.x()*(-1));
    }
    if(AB.z()>0)
    {
        AO.SetKoordinaten(2, AB.z());
        BO.SetKoordinaten(2, 0.);
    }else{
        AO.SetKoordinaten(2, 0.);
        BO.SetKoordinaten(2, AB.z()*(-1));
    }

    /*theoretische Anaglyphengroesse*/
    if( ((MA.x()*2)-AO.x()) < ((MB.x()*2)-BO.x()) )/*x-Koordinate*/
    {
        MAN.SetKoordinaten(0, (((MA.x()*2)-AO.x())/2) );
    }else{
        MAN.SetKoordinaten(0, (((MB.x()*2)-BO.x())/2) );
    }
    if( ((MA.z()*2)-AO.z()) < ((MB.z()*2)-BO.z()) )/*z-Koordinate*/
    {
        MAN.SetKoordinaten(2, (((MA.z()*2)-AO.z())/2) );
    }else{
        MAN.SetKoordinaten(2, (((MB.z()*2)-BO.z())/2) );
    }

    H = MA - AO - MAN - DP;
    if(LinksAktiv)
    {
        H_ = H.drehen(0, -deltaLinkeDrehung, 0);
        LinkesBild = LinkesOriginal.Scale(int(LinkesOriginal.GetWidth()*AnzeigeVergroesserung+0.5), int(LinkesOriginal.GetHeight()*AnzeigeVergroesserung+0.5))
                    .Rotate((LinkeDrehung+deltaLinkeDrehung), wxPoint(0, 0), false);
        MA_.SetKoordinaten(LinkesBild.GetWidth()/2, 0, LinkesBild.GetHeight()/2);
        //if(deltaLinkeDrehung<0.0001)H_= H;
    }else{
        H_ = H.drehen(0, -deltaRechteDrehung,0);
        RechtesBild = RechtesOriginal.Scale(int(RechtesOriginal.GetWidth()*AnzeigeVergroesserung+0.5), int(RechtesOriginal.GetHeight()*AnzeigeVergroesserung+0.5))
                    .Rotate((RechteDrehung+deltaRechteDrehung), wxPoint(0, 0), false);
        MA_.SetKoordinaten(RechtesBild.GetWidth()/2, 0, RechtesBild.GetHeight()/2);
        //if(deltaRechteDrehung<0.0001)H_= H;
    }

    deltaOffset = MA_ - H_ + H - MA;
    AB_ = AB + deltaOffset;
    deltaOffset /= AnzeigeVergroesserung;
    if(!LinksAktiv)deltaOffset*=(-1);

    /*Drehpunkt fuer Rueckgabe ermitteln*/
    if(AB_.x()>0)
    {
        AO_.SetKoordinaten(0, AB_.x());
        BO_.SetKoordinaten(0, 0.);
    }else{
        AO_.SetKoordinaten(0, 0.);
        BO_.SetKoordinaten(0, AB_.x()*(-1));
    }
    if(AB_.z()>0)
    {
        AO_.SetKoordinaten(2, AB_.z());
        BO_.SetKoordinaten(2, 0.);
    }else{
        AO_.SetKoordinaten(2, 0.);
        BO_.SetKoordinaten(2, AB_.z()*(-1));
    }

    /*theoretische Anaglyphengroesse*/
    if( ((MA_.x()*2)-AO_.x()) < ((MB.x()*2)-BO_.x()) )/*x-Koordinate*/
    {
        MAN_.SetKoordinaten(0, (((MA_.x()*2)-AO_.x())/2) );
    }else{
        MAN_.SetKoordinaten(0, (((MB.x()*2)-BO_.x())/2) );
    }
    if( ((MA_.z()*2)-AO_.z()) < ((MB.z()*2)-BO_.z()) )/*z-Koordinate*/
    {
        MAN_.SetKoordinaten(2, (((MA_.z()*2)-AO_.z())/2) );
    }else{
        MAN_.SetKoordinaten(2, (((MB.z()*2)-BO_.z())/2) );
    }

    DP_ = DP + MAN + BO - BO_ - MAN_;

    return (wxPoint((int)DP_.x(),(int)DP_.z()));
}

void Bild3D::SetzeRand(double wert)
{
    randProzent = wert;
    return;
}

void Bild3D::DrehungBeenden()
{
    Offset += deltaOffset;
    deltaOffset.SetKoordinaten(0, 0, 0);
    if(LinksAktiv)
    {
        LinkeDrehung+=deltaLinkeDrehung;
        deltaLinkeDrehung=0;
    }else{
        RechteDrehung+=deltaRechteDrehung;
        deltaRechteDrehung=0;
    }
    GroesseLinks.SetKoordinaten(LinkesBild.GetWidth(), 0, LinkesBild.GetHeight());
    GroesseRechts.SetKoordinaten(RechtesBild.GetWidth(), 0, RechtesBild.GetHeight());

    return;
}

void Bild3D::Verschieben(int x, int z)
{
    Vektor Verschub;
    Verschub.SetKoordinaten(x, 0, z);
    Verschub /= AnzeigeVergroesserung;

    if(LinksAktiv)
    {
        Offset -= Verschub;
    }else{
        Offset += Verschub;
    }
    return;
}

void Bild3D::ZoomeTeilBild(float)
{
/*noch nichts implementiert*/
}

/*Bilder laden und loeschen*/

bool Bild3D::LadeLinkesBild(wxString BildPfad)
{
    if(LinkesBild.Ok()) LinkesBild.Destroy(); //erst mal kaputt machen
    if(LinkesOriginal.Ok()) LinkesOriginal.Destroy(); //erst mal kaputt machen

    bool Erfolg = LinkesBild.LoadFile(BildPfad, wxBITMAP_TYPE_JPEG);
    if(Erfolg)
    {
        LinkerPfad = BildPfad;
        LinkesOriginal.LoadFile(BildPfad, wxBITMAP_TYPE_JPEG);
    }

    Offset.SetKoordinaten(0, 0, 0);
    deltaOffset.SetKoordinaten(0, 0, 0);
    LinkeDrehung=0;
    deltaLinkeDrehung=0;
    GroesseLinks.SetKoordinaten(LinkesBild.GetWidth(), 0, LinkesBild.GetHeight());

	MinGroessenFinden();
	AnzeigeGroesseAnpassen();

    Zuschnitt->SetGrenzen(wxRect(int(-LinkesBild.GetWidth()/(2 * AnzeigeVergroesserung)),
                                int(-LinkesBild.GetHeight()/(2 * AnzeigeVergroesserung)),
                                int(LinkesBild.GetWidth()/(AnzeigeVergroesserung)),
                                int(LinkesBild.GetHeight()/(AnzeigeVergroesserung))));
    Zuschnitt->SetAuswahl(wxRect(int(-LinkesBild.GetWidth()/(2 * AnzeigeVergroesserung)),
                                int(-LinkesBild.GetHeight()/(2 * AnzeigeVergroesserung)),
                                int(LinkesBild.GetWidth()/(AnzeigeVergroesserung)),
                                int(LinkesBild.GetHeight()/(AnzeigeVergroesserung))));
    return Erfolg;
}

bool Bild3D::LadeRechtesBild(wxString BildPfad)
{
    if(RechtesBild.Ok()) RechtesBild.Destroy(); //erst mal kaputt machen
    if(RechtesOriginal.Ok()) RechtesOriginal.Destroy(); //erst mal kaputt machen

    bool Erfolg = RechtesBild.LoadFile(BildPfad, wxBITMAP_TYPE_JPEG);
    if(Erfolg)
    {
        RechterPfad = BildPfad;
        RechtesOriginal.LoadFile(BildPfad, wxBITMAP_TYPE_JPEG);
    }

    Offset.SetKoordinaten(0, 0, 0);
    deltaOffset.SetKoordinaten(0, 0, 0);
    RechteDrehung=0;
    deltaRechteDrehung=0;
    GroesseRechts.SetKoordinaten(RechtesBild.GetWidth(), 0, RechtesBild.GetHeight());

	MinGroessenFinden();
	AnzeigeGroesseAnpassen();

    Zuschnitt->SetGrenzen(wxRect(int(-RechtesBild.GetWidth()/(2 * AnzeigeVergroesserung)),
                                int(-RechtesBild.GetHeight()/(2 * AnzeigeVergroesserung)),
                                int(RechtesBild.GetWidth()/(AnzeigeVergroesserung)),
                                int(RechtesBild.GetHeight()/(AnzeigeVergroesserung))));
    Zuschnitt->SetAuswahl(wxRect(int(-RechtesBild.GetWidth()/(2 * AnzeigeVergroesserung)),
                                int(-RechtesBild.GetHeight()/(2 * AnzeigeVergroesserung)),
                                int(RechtesBild.GetWidth()/(AnzeigeVergroesserung)),
                                int(RechtesBild.GetHeight()/(AnzeigeVergroesserung))));
    return Erfolg;
}

void Bild3D::NeuLaden(void)
{
    LinkesBild = LinkesOriginal.Scale(int(LinkesOriginal.GetWidth()*AnzeigeVergroesserung+0.5), int(LinkesOriginal.GetHeight()*AnzeigeVergroesserung+0.5))
                    .Rotate((LinkeDrehung+deltaLinkeDrehung), wxPoint(0, 0), false);
    RechtesBild = RechtesOriginal.Scale(int(RechtesOriginal.GetWidth()*AnzeigeVergroesserung+0.5), int(RechtesOriginal.GetHeight()*AnzeigeVergroesserung+0.5))
                    .Rotate((RechteDrehung+deltaRechteDrehung), wxPoint(0, 0), false);
    EffektPruefen();
    return;
}

void Bild3D::Hochzeit(void)
{
    Hochzeit(1);
    AnzeigeFaktor = 1.0;
    return;
}

bool Bild3D::SpeicherAnaglyphe(wxString Pfad)
{
    logSchreiben("\nSpeicherung der Anaglyphe\n");
    bool Erfolg;
    float tempVergroesserung = AnzeigeVergroesserung;
    AnzeigeVergroesserung = 1.0;

    LinkesBild = LinkesOriginal.Rotate((LinkeDrehung+deltaLinkeDrehung), wxPoint(0, 0), true);
    RechtesBild = RechtesOriginal.Rotate((RechteDrehung+deltaRechteDrehung), wxPoint(0, 0), true);
    EffektPruefen();
    Hochzeit(1);
    logSchreiben("Hochzeit erfolgreich\n");
    AnzeigeVergroesserung = tempVergroesserung;

    if(zuschnittAktiv)
    {
        wxSize tempGroesse;
        wxPoint tempPoint;
        wxRect schnipsel = Zuschnitt->GetAuswahl();

        tempPoint = wxPoint(int(schnipsel.x)+ Anaglyphe.GetWidth()/2, int(schnipsel.y) + Anaglyphe.GetHeight()/2);
        if(tempPoint.x<0)tempPoint.x=0;
        if(tempPoint.y<0)tempPoint.y=0;

        tempGroesse = schnipsel.GetSize();
        if((Anaglyphe.GetWidth()-tempPoint.x)<tempGroesse.GetWidth()) tempGroesse.SetWidth(Anaglyphe.GetWidth()-tempPoint.x);
        if((Anaglyphe.GetHeight()-tempPoint.y)<tempGroesse.GetHeight()) tempGroesse.SetHeight(Anaglyphe.GetHeight()-tempPoint.y);

//        wxImage Test = Anaglyphe.Resize(tempGroesse, tempPoint); Irgendwas klappt so nicht
		wxImage Test = Anaglyphe.GetSubImage(wxRect(tempPoint, tempGroesse));

        Erfolg=0;
        if(Test.Ok())
        {
            Erfolg = Test.SaveFile(Pfad, wxBITMAP_TYPE_PNG);
            logSchreiben("SubImage erfolgreich erstellt\n");
            DebugString = wxT("SubImage erfolgreich erstellt\n");
        } else {
            DebugString = wxT("SubImage nicht erstellt");
			DebugString.Append(wxT(" - Verschub: "));
			DebugString.Append(wxString::Format(wxT("%i"), tempPoint.x));
			DebugString.Append(wxT(" / "));
			DebugString.Append(wxString::Format(wxT("%i"), tempPoint.y));
			DebugString.Append(_("\n") + wxT("Groesse: "));
			DebugString.Append(wxString::Format(wxT("%i"), tempGroesse.GetWidth()));
			DebugString.Append(wxT(" / ") + wxString::Format(wxT("%i"), tempGroesse.GetHeight()));
        }
    }else{
        Erfolg = Anaglyphe.SaveFile(Pfad, wxBITMAP_TYPE_PNG);
    }
    if(Erfolg)
    {
        logSchreiben("Anaglyphe wurde gespeichert\n");
        logSchreiben(static_cast<const char*>(Pfad));
    }
    /*Bilder skaliert neu laden*/
    NeuLaden();
    EffektPruefen();
    return Erfolg;
}

void Bild3D::LoescheLinkesBild(void)
{
    LinkesBild.Destroy();
    LinkesOriginal.Destroy();
    return;
}

void Bild3D::LoescheRechtesBild(void)
{
    RechtesBild.Destroy();
    RechtesOriginal.Destroy();
    return;
}

/*Private Funktionen*/
void Bild3D::MinGroessenFinden(void)
{
    if(LinkesOriginal.Ok() && RechtesOriginal.Ok())
    {
        MinSizeX = LinkesOriginal.GetWidth();
        MinSizeY = LinkesOriginal.GetHeight();
        if(RechtesOriginal.GetWidth()<MinSizeX)  MinSizeX = RechtesOriginal.GetWidth();
        if(RechtesOriginal.GetHeight()<MinSizeY) MinSizeY = RechtesOriginal.GetHeight();
        return;
    }
    if(LinkesOriginal.Ok())
    {
        MinSizeX = LinkesOriginal.GetWidth();
        MinSizeY = LinkesOriginal.GetHeight();
        return;
    }
    if(RechtesOriginal.Ok())
    {
        MinSizeX = RechtesOriginal.GetWidth();
        MinSizeY = RechtesOriginal.GetHeight();
        return;
    }
    return;
}

void Bild3D::AnzeigeAnpassen(wxSize dcSize)
{
	GroesseDCX = dcSize.GetWidth();
	GroesseDCY = dcSize.GetHeight();
	if(!(GroesseDCX > 0))GroesseDCX = 1;
	if(!(GroesseDCY > 0))GroesseDCY = 1;
	AnzeigeGroesseAnpassen();
    return;
}

void Bild3D::AnzeigeGroesseAnpassen()
{
    if(((float(GroesseDCX)/float(MinSizeX)) < (float(GroesseDCY)/float(MinSizeY))))
    {
        AnzeigeVergroesserung = (float(GroesseDCX)/float(MinSizeX));
    } else {
        AnzeigeVergroesserung = (float(GroesseDCY)/float(MinSizeY));
    }

    if(!(AnzeigeVergroesserung))throw(wxT("Vergroesserung ist null"));

    DrehungBeenden();
    NeuLaden();
    EffektPruefen();

    GroesseLinks.SetKoordinaten(LinkesBild.GetWidth(), 0, LinkesBild.GetHeight());
    GroesseRechts.SetKoordinaten(RechtesBild.GetWidth(), 0, RechtesBild.GetHeight());
    return;
}

void Bild3D::Draw3DBild(wxDC &dc)
{
    wxImage DrawImage;

    ZuschnittEingrenzen();
    Hochzeit();

    if(Anaglyphe.Ok())
    {
        logSchreiben("\nAnaglyphe ist i. O.\n");
        logSchreiben("Groeße: ");logSchreiben((float)Anaglyphe.GetWidth(), 0);
        logSchreiben(" / ");logSchreiben((float)Anaglyphe.GetHeight(), 0);
        logSchreiben("\n");
        DrawImage = Anaglyphe.Copy();
    } else
    if(LinkesBild.Ok())
    {
        logSchreiben("\nAnaglyphe nicht OK\n");
        DrawImage = LinkesBild.Copy();
    } else
    if(RechtesBild.Ok())
    {
        logSchreiben("\nAnaglyphe nicht OK\n");
        DrawImage = RechtesBild.Copy();
    } else throw(wxT("keine Bilder vorhanden"));

    wxSize dcSize = wxSize(GroesseDCX, GroesseDCY);//dc.GetSize();
    int DrawSizeX = (int)(DrawImage.GetWidth() / AnzeigeFaktor);
    int DrawSizeY = (int)(DrawImage.GetHeight() / AnzeigeFaktor);

    if(zuschnittAktiv)
    {
        Zuschnitt->DrawAuswahl(dc, dcSize, AnzeigeVergroesserung);
    }

    dc.DrawBitmap(wxBitmap(DrawImage.Rescale(DrawSizeX, DrawSizeY)),
                    ((dcSize.GetWidth()-DrawSizeX)/2),
                    ((dcSize.GetHeight()-DrawSizeY)/2), true);

    if(zuschnittAktiv)
    {
        dc.DestroyClippingRegion();
    }
    return;
}

wxRect Bild3D::GetAnzeigeGroesse(wxDC& dc)
{
    wxImage DrawImage;

    if(Anaglyphe.Ok())
    {
        DrawImage = Anaglyphe.Copy();
    } else
    if(LinkesBild.Ok())
    {
        DrawImage = LinkesBild.Copy();
    } else
    if(RechtesBild.Ok())
    {
        DrawImage = RechtesBild.Copy();
    } else return(wxRect(0,0,10,10));

    int Breite = (int)(DrawImage.GetWidth()); // AnzeigeFaktor);
    int Hoehe = (int)(DrawImage.GetHeight()); // AnzeigeFaktor);
    wxRect Rueckgabe = wxRect((int)((dc.GetSize().GetWidth()/2-Breite/2)),(int)((dc.GetSize().GetHeight()/2-Hoehe/2)), Breite, Hoehe);

    return Rueckgabe;
}

float Bild3D::GetAnzeigeFaktor(void)
{
	return(AnzeigeVergroesserung);
}


int Bild3D::DebugInfo(wxDC& dc, int Zeilen)
{
    wxFont TextFont = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dc.SetFont(TextFont);
    wxColour TextColour = wxColour(255, 255, 255);
    dc.SetTextForeground(TextColour);
    TextColour = wxColour(0,0,0);
    dc.SetTextBackground(TextColour);

    dc.DrawText(wxT("Vergrösserung: \t") + wxString::Format(wxT("%.3f"),AnzeigeVergroesserung), 10, Zeilen*12);
    Zeilen++;
    dc.DrawText(_("Offset: \t") + wxString::Format(wxT("%.1f"),Offset.x()) + wxT(" / ") + wxString::Format(wxT("%.1f"),Offset.z()), 10, Zeilen*12);
    Zeilen++;
    dc.DrawText(_("delta Offset: \t") + wxString::Format(wxT("%.1f"),deltaOffset.x()) + _(" / ") + wxString::Format(wxT("%.1f"),deltaOffset.z()), 10, Zeilen*12);
    Zeilen++;
	dc.DrawText(_("Drehung linkes Teilbild: \t") + wxString::Format(wxT("%.2f"),((LinkeDrehung + deltaLinkeDrehung)*180/PI))+ wxT("°"), 10, Zeilen*12);
    Zeilen++;
    dc.DrawText(_("Drehung rechtes Teilbild: \t") + wxString::Format(wxT("%.2f"), ((RechteDrehung + deltaRechteDrehung)*180/PI)) + wxT("°"), 10, Zeilen*12);
    Zeilen++;
    Zeilen++;
    dc.DrawText(_("SWSchwelle: \t") + wxString::Format(wxT("%i"), ((int)SWSchwellen)), 10, Zeilen*12);
    Zeilen++;

    wxString Puffer;
    int Stellen;
    while(!(DebugString.IsEmpty()))
    {
        Stellen = DebugString.Find('\n');
        if(Stellen==wxNOT_FOUND)break;
        Puffer = DebugString.Left(Stellen);
        //logSchreiben(static_cast<const char*>(Puffer));
        dc.DrawText(Puffer, 10, Zeilen*12);
        Stellen = DebugString.Len() - Stellen-1;
        DebugString = DebugString.Right(Stellen);
        Zeilen++;
    }
    Zeilen++;
    Zeilen = Zuschnitt->DebugInfo(dc, Zeilen);
    return (Zeilen);
}

float Bild3D::Rest(float Zahl, float Teiler)
{
	while(Zahl > Teiler)
	{
		Zahl -= Teiler;
	}
	return (Zahl);
}

void Bild3D::Effekt(EffektID neuerEffekt)
{
    DrehungBeenden();

    if(aktuellerEffekt!=neuerEffekt)
    {
        aktuellerEffekt=neuerEffekt;
        NeuLaden();
    }
    EffektPruefen();
    return;
}

void Bild3D::SetzeSWSchwellen(unsigned char neueSchwelle)
{
    /*if (SWSchwellen<0) SWSchwellen=0; negative Werte abfangen*/
    if(SWSchwellen!=neueSchwelle)
    {
        SWSchwellen=neueSchwelle;
        if(SWSchwellen==0)SWSchwellen=1;
        NeuLaden();
    }
    return;
}

unsigned char Bild3D::HoleSWSchwellen(void)
{
    return(SWSchwellen);
}

void Bild3D::EffektPruefen(void)
{
	if(aktuellerEffekt==ID_3DBILD_GRAU)InGrauUmwandeln();
    if(aktuellerEffekt==ID_3DBILD_SW)InSWUmwandeln();
    return;
}

void Bild3D::Zuschneiden(bool wert)
{
    zuschnittAktiv = wert;
    return;
}

bool Bild3D::Zuschneiden(void)
{
    return zuschnittAktiv;
}

void Bild3D::ZuschnittAendern(wxPoint von, wxPoint nach)
{
    wxPoint tempVon, tempNach;

    tempVon = wxPoint((int)((float)(von.x)/AnzeigeVergroesserung), (int)((float)(von.y)/AnzeigeVergroesserung));
    tempNach = wxPoint((int)((float)(nach.x)/AnzeigeVergroesserung), (int)((float)(nach.y)/AnzeigeVergroesserung));

    Zuschnitt->Aendern(tempVon, tempNach);
    return;
}

void Bild3D::ZuschnittEingrenzen()
{
    if(Anaglyphe.Ok())
    {
        Zuschnitt->SetGrenzen(wxRect(int(-Anaglyphe.GetWidth()/(2 * AnzeigeVergroesserung)),
                                    int(-Anaglyphe.GetHeight()/(2 * AnzeigeVergroesserung)),
                                    int(Anaglyphe.GetWidth()/(AnzeigeVergroesserung)),
                                    int(Anaglyphe.GetHeight()/(AnzeigeVergroesserung))));
    } else
    if(LinkesBild.Ok())
    {
        Zuschnitt->SetGrenzen(wxRect(int(-LinkesBild.GetWidth()/(2 * AnzeigeVergroesserung)),
                                    int(-LinkesBild.GetHeight()/(2 * AnzeigeVergroesserung)),
                                    int(LinkesBild.GetWidth()/(AnzeigeVergroesserung)),
                                    int(LinkesBild.GetHeight()/(AnzeigeVergroesserung))));
    } else
    if(RechtesBild.Ok())
    {
        Zuschnitt->SetGrenzen(wxRect(int(-RechtesBild.GetWidth()/(2 * AnzeigeVergroesserung)),
                                    int(-RechtesBild.GetHeight()/(2 * AnzeigeVergroesserung)),
                                    int(RechtesBild.GetWidth()/(AnzeigeVergroesserung)),
                                    int(RechtesBild.GetHeight()/(AnzeigeVergroesserung))));
    } else throw(wxT("keine Bilder vorhanden"));
}

void Bild3D::SetZuschnittStatus(wxPoint schirmMarke)
{
    int markeX = (int)(((float)schirmMarke.x - (float)GroesseDCX/2) / AnzeigeVergroesserung);
    int markeY = (int)(((float)schirmMarke.y - (float)GroesseDCY/2) / AnzeigeVergroesserung);
    Zuschnitt->SetStatus(wxPoint(markeX, markeY));
    return;
}

wxCursor Bild3D::MausStellung(wxPoint schirmMarke)
{
    int markeX = (int)(((float)schirmMarke.x - (float)GroesseDCX/2) / AnzeigeVergroesserung);
    int markeY = (int)(((float)schirmMarke.y - (float)GroesseDCY/2) / AnzeigeVergroesserung);
    return(Zuschnitt->MausStellung(wxPoint(markeX, markeY)));
}

void Bild3D::logSchreiben(const char*msg)
{
    Logbuch->open(logName,ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        *Logbuch<<msg;
        Logbuch->close();
    }
    return;
}

void Bild3D::logSchreiben(const float msg, int i)
{
    Logbuch->open(logName,ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        Logbuch->setf( ios::fixed, ios::floatfield );
        Logbuch->precision(i);
        *Logbuch<<msg;
        Logbuch->close();
    }
    return;
}

void Bild3D::InkrRand(double wert)
{
    randProzent += wert;
    if(randProzent > 1)randProzent=1;
    if(randProzent < 0)randProzent=0;
    MinGroessenFinden();
}

void Bild3D::InkrSWSchwellen(int wert)
{
    if(wert<0)
    {
        if(SWSchwellen <= abs(wert))
        {
            SWSchwellen = 1;
            NeuLaden();
            return;
        }
    }else
    {
        if((int)(SWSchwellen+wert)>255)
        {
            SWSchwellen = 255;
            NeuLaden();
            return;
        }
    }
    SWSchwellen += wert;
    NeuLaden();
    return;
}

/******************/
/*RotCyanAnaglyphe*/
/******************/
RotCyanAnaglyphe::RotCyanAnaglyphe() : Bild3D()
{
    /*Log starten*/
    logName = ("RotCyan.log");
	Logbuch = new ofstream(logName, ios_base::out|ios_base::trunc);
    if(Logbuch->good())
    {
        *Logbuch<<"Logbuch gestartet\n";
        Logbuch->close();
    }
}

RotCyanAnaglyphe::RotCyanAnaglyphe(const Bild3D& Obj) : Bild3D(Obj)
{
    if(this != &Obj)
    {
        /*Log starten*/
        logName = ("RotCyan.log");
        Logbuch = new ofstream(logName, ios_base::out|ios_base::trunc);
        if(Logbuch->good())
        {
            *Logbuch<<"Logbuch gestartet\n";
            Logbuch->close();
        }
    }
}

RotCyanAnaglyphe::~RotCyanAnaglyphe()
{
    logSchreiben("\nLogbuch geschlossen\n");
	delete Logbuch;
}

/*Bild herstellen und anzeigen*/
void RotCyanAnaglyphe::Hochzeit(float Fktr)
{
    int GroesseAnaglyphe[2];
    AnzeigeFaktor = Fktr;

    /*Exceptionhandling*/
    if(!(LinkesBild.Ok()))throw wxT("Linkes Bild nicht OK");
    if(!(RechtesBild.Ok()))throw wxT("Rechtes Bild nicht OK");
    /*Ende Exeptionhandling*/

    wxImage TempLinkesBild = LinkesBild.Scale((int)(LinkesBild.GetWidth()*AnzeigeFaktor), (int)(LinkesBild.GetHeight()*AnzeigeFaktor));
    wxImage TempRechtesBild = RechtesBild.Scale((int)(RechtesBild.GetWidth()*AnzeigeFaktor), (int)(RechtesBild.GetHeight()*AnzeigeFaktor));

    Vektor TempOffset = ((Offset + deltaOffset) * AnzeigeFaktor * AnzeigeVergroesserung);

    int TempOffsetLinks[2], TempOffsetRechts[2];
    TempOffsetLinks[0] = (TempOffset.x() > 0) ?  (int)(TempOffset.x()+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/
    TempOffsetLinks[1] = (TempOffset.z() > 0) ?  (int)(TempOffset.z()+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/

    TempOffsetRechts[0] = (TempOffset.x() < 0) ? (int)(-(TempOffset.x())+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/
    TempOffsetRechts[1] = (TempOffset.z() < 0) ? (int)(-(TempOffset.z())+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/

    if(TempLinkesBild.Ok() && TempRechtesBild.Ok())
    {
        /*neue Groesse der Anaglyphe*/
        if((TempLinkesBild.GetWidth()-TempOffsetLinks[0])<(TempRechtesBild.GetWidth()-TempOffsetRechts[0]))
        {
            GroesseAnaglyphe[0]=(TempLinkesBild.GetWidth()-TempOffsetLinks[0]);
        }else{
            GroesseAnaglyphe[0]=(TempRechtesBild.GetWidth()-TempOffsetRechts[0]);
        }

        if((TempLinkesBild.GetHeight()-TempOffsetLinks[1])<(TempRechtesBild.GetHeight()-TempOffsetRechts[1]))
        {
            GroesseAnaglyphe[1]=(TempLinkesBild.GetHeight()-TempOffsetLinks[1]);
        }else{
            GroesseAnaglyphe[1]=(TempRechtesBild.GetHeight()-TempOffsetRechts[1]);
        }

        if(GroesseAnaglyphe[0]<=0){
            Anaglyphe.Resize(wxSize(1, 1), wxPoint(0, 0));
            return;
        }
        if(GroesseAnaglyphe[1]<=0){
            Anaglyphe.Resize(wxSize(1, 1), wxPoint(0, 0));
            return;
        }

        Anaglyphe.Resize(wxSize(GroesseAnaglyphe[0], GroesseAnaglyphe[1]), wxPoint(0, 0));

        //Width = wxString::Format(wxT("%i"),Zahl);

        /*Data der Teilbilder holen und in einen Datensatz für die Anaglyphe "mischen"*/
        unsigned char* RechtsData;
        unsigned char* LinksData;
        unsigned char* AnaglypheData;
        RechtsData = TempRechtesBild.GetData();
        LinksData = TempLinkesBild.GetData();
        AnaglypheData = (unsigned char *) malloc(3 * GroesseAnaglyphe[0] * GroesseAnaglyphe[1]);

        //nur wenn Speicherallozierung erfolgreich war - sonst ist AnaglyphData = NULL
        if(AnaglypheData)
        {
            int StelleRechts=(TempOffsetRechts[1]*TempRechtesBild.GetWidth()+TempOffsetRechts[0])*3;
            int StelleLinks=(TempOffsetLinks[1]*TempLinkesBild.GetWidth()+TempOffsetLinks[0])*3;
            int Stelle=0;

            for(int Reihe=0; Reihe<GroesseAnaglyphe[1]; Reihe++)
            {
                for(int Spalte=0; Spalte<GroesseAnaglyphe[0]; Spalte++)
                {
                    /*Hier wird das neue Bild geschrieben*/
                    AnaglypheData[Stelle]=LinksData[StelleLinks];
                    AnaglypheData[(Stelle+1)]=RechtsData[(StelleRechts+1)];
                    AnaglypheData[(Stelle+2)]=RechtsData[(StelleRechts+2)];
                    Stelle+=3;
                    StelleRechts+=3;
                    StelleLinks+=3;
                }
            StelleRechts+=((TempRechtesBild.GetWidth()-GroesseAnaglyphe[0])*3);
            StelleLinks+=((TempLinkesBild.GetWidth()-GroesseAnaglyphe[0])*3);
            }
        Anaglyphe.SetData(AnaglypheData);
        }
    }
    return;
}

/***********************/
/*MagentaGruenAnaglyphe*/
/***********************/
MagentaGruenAnaglyphe::MagentaGruenAnaglyphe() : Bild3D()
{
    /*Log starten*/
    logName = ("MagentaGruen.log");
	Logbuch = new ofstream(logName, ios_base::out|ios_base::trunc);
    if(Logbuch->good())
    {
        *Logbuch<<"Logbuch gestartet\n";
        Logbuch->close();
    }
}

MagentaGruenAnaglyphe::MagentaGruenAnaglyphe(const Bild3D& Obj) : Bild3D(Obj)
{
    if(this != &Obj)
    {
        /*Log starten*/
        logName = ("MagentaGruen.log");
        Logbuch = new ofstream(logName, ios_base::out|ios_base::trunc);
        if(Logbuch->good())
        {
            *Logbuch<<"Logbuch gestartet\n";
            Logbuch->close();
        }
    }
}

MagentaGruenAnaglyphe::~MagentaGruenAnaglyphe()
{
    logSchreiben("\nLogbuch geschlossen\n");
	delete Logbuch;
}

/*Bild herstellen und anzeigen*/
void MagentaGruenAnaglyphe::Hochzeit(float Fktr)
{
    int GroesseAnaglyphe[2];
    AnzeigeFaktor = Fktr;

    /*Exceptionhandling*/
    if(!(LinkesBild.Ok()))throw wxT("Linkes Bild nicht OK");
    if(!(RechtesBild.Ok()))throw wxT("Rechtes Bild nicht OK");
    /*Ende Exeptionhandling*/

    wxImage TempLinkesBild = LinkesBild.Scale((int)(LinkesBild.GetWidth()*AnzeigeFaktor), (int)(LinkesBild.GetHeight()*AnzeigeFaktor));
    wxImage TempRechtesBild = RechtesBild.Scale((int)(RechtesBild.GetWidth()*AnzeigeFaktor), (int)(RechtesBild.GetHeight()*AnzeigeFaktor));

    Vektor TempOffset = ((Offset + deltaOffset) * AnzeigeFaktor * AnzeigeVergroesserung);

    int TempOffsetLinks[2], TempOffsetRechts[2];
    TempOffsetLinks[0] = (TempOffset.x() > 0) ?  (int)(TempOffset.x()+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/
    TempOffsetLinks[1] = (TempOffset.z() > 0) ?  (int)(TempOffset.z()+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/

    TempOffsetRechts[0] = (TempOffset.x() < 0) ? (int)(-(TempOffset.x())+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/
    TempOffsetRechts[1] = (TempOffset.z() < 0) ? (int)(-(TempOffset.z())+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/

    if(TempLinkesBild.Ok() && TempRechtesBild.Ok())
    {
        /*neue Groesse der Anaglyphe*/
        if((TempLinkesBild.GetWidth()-TempOffsetLinks[0])<(TempRechtesBild.GetWidth()-TempOffsetRechts[0]))
        {
            GroesseAnaglyphe[0]=(TempLinkesBild.GetWidth()-TempOffsetLinks[0]);
        }else{
            GroesseAnaglyphe[0]=(TempRechtesBild.GetWidth()-TempOffsetRechts[0]);
        }

        if((TempLinkesBild.GetHeight()-TempOffsetLinks[1])<(TempRechtesBild.GetHeight()-TempOffsetRechts[1]))
        {
            GroesseAnaglyphe[1]=(TempLinkesBild.GetHeight()-TempOffsetLinks[1]);
        }else{
            GroesseAnaglyphe[1]=(TempRechtesBild.GetHeight()-TempOffsetRechts[1]);
        }

        if(GroesseAnaglyphe[0]<=0){
            Anaglyphe.Resize(wxSize(1, 1), wxPoint(0, 0));
            return;
        }
        if(GroesseAnaglyphe[1]<=0){
            Anaglyphe.Resize(wxSize(1, 1), wxPoint(0, 0));
            return;
        }

        Anaglyphe.Resize(wxSize(GroesseAnaglyphe[0], GroesseAnaglyphe[1]), wxPoint(0, 0));

        //Width = wxString::Format(wxT("%i"),Zahl);

        /*Data der Teilbilder holen und in einen Datensatz für die Anaglyphe "mischen"*/
        unsigned char* RechtsData;
        unsigned char* LinksData;
        unsigned char* AnaglypheData;
        RechtsData = TempRechtesBild.GetData();
        LinksData = TempLinkesBild.GetData();
        AnaglypheData = (unsigned char *) malloc(3 * GroesseAnaglyphe[0] * GroesseAnaglyphe[1]);

        //nur wenn Speicherallozierung erfolgreich war - sonst ist AnaglyphData = NULL
        if(AnaglypheData)
        {
            int StelleRechts=(TempOffsetRechts[1]*TempRechtesBild.GetWidth()+TempOffsetRechts[0])*3;
            int StelleLinks=(TempOffsetLinks[1]*TempLinkesBild.GetWidth()+TempOffsetLinks[0])*3;
            int Stelle=0;

            for(int Reihe=0; Reihe<GroesseAnaglyphe[1]; Reihe++)
            {
                for(int Spalte=0; Spalte<GroesseAnaglyphe[0]; Spalte++)
                {
                    /*Hier wird das neue Bild geschrieben*/
                    AnaglypheData[Stelle]=RechtsData[StelleRechts];
                    AnaglypheData[(Stelle+1)]=LinksData[(StelleLinks+1)];
                    AnaglypheData[(Stelle+2)]=RechtsData[(StelleRechts+2)];
                    Stelle+=3;
                    StelleRechts+=3;
                    StelleLinks+=3;
                }
            StelleRechts+=((TempRechtesBild.GetWidth()-GroesseAnaglyphe[0])*3);
            StelleLinks+=((TempLinkesBild.GetWidth()-GroesseAnaglyphe[0])*3);
            }
        Anaglyphe.SetData(AnaglypheData);
        }
    }
    return;
}

/************************/
/*BernsteinBlauAnaglyphe*/
/************************/
BernsteinBlauAnaglyphe::BernsteinBlauAnaglyphe() : Bild3D()
{
    /*Log starten*/
    logName = ("BernsteinBlau.log");
	Logbuch = new ofstream(logName, ios_base::out|ios_base::trunc);
    if(Logbuch->good())
    {
        *Logbuch<<"Logbuch gestartet\n";
        Logbuch->close();
    }
}

BernsteinBlauAnaglyphe::BernsteinBlauAnaglyphe(const Bild3D& Obj) : Bild3D(Obj)
{
    if(this != &Obj)
    {
        /*Log starten*/
        logName = ("BernsteinBlau.log");
        Logbuch = new ofstream(logName, ios_base::out|ios_base::trunc);
        if(Logbuch->good())
        {
            *Logbuch<<"Logbuch gestartet\n";
            Logbuch->close();
        }
    }
}

BernsteinBlauAnaglyphe::~BernsteinBlauAnaglyphe()
{
    logSchreiben("\nLogbuch geschlossen\n");
	delete Logbuch;
}

/*Bild herstellen und anzeigen*/
void BernsteinBlauAnaglyphe::Hochzeit(float Fktr)
{
    int GroesseAnaglyphe[2];
    AnzeigeFaktor = Fktr;

    /*Exceptionhandling*/
    if(!(LinkesBild.Ok()))throw wxT("Linkes Bild nicht OK");
    if(!(RechtesBild.Ok()))throw wxT("Rechtes Bild nicht OK");
    /*Ende Exeptionhandling*/

    wxImage TempLinkesBild = LinkesBild.Scale((int)(LinkesBild.GetWidth()*AnzeigeFaktor), (int)(LinkesBild.GetHeight()*AnzeigeFaktor));
    wxImage TempRechtesBild = RechtesBild.Scale((int)(RechtesBild.GetWidth()*AnzeigeFaktor), (int)(RechtesBild.GetHeight()*AnzeigeFaktor));

    Vektor TempOffset = ((Offset + deltaOffset) * AnzeigeFaktor * AnzeigeVergroesserung);

    int TempOffsetLinks[2], TempOffsetRechts[2];
    TempOffsetLinks[0] = (TempOffset.x() > 0) ?  (int)(TempOffset.x()+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/
    TempOffsetLinks[1] = (TempOffset.z() > 0) ?  (int)(TempOffset.z()+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/

    TempOffsetRechts[0] = (TempOffset.x() < 0) ? (int)(-(TempOffset.x())+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/
    TempOffsetRechts[1] = (TempOffset.z() < 0) ? (int)(-(TempOffset.z())+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/

    if(TempLinkesBild.Ok() && TempRechtesBild.Ok())
    {
        /*neue Groesse der Anaglyphe*/
        if((TempLinkesBild.GetWidth()-TempOffsetLinks[0])<(TempRechtesBild.GetWidth()-TempOffsetRechts[0]))
        {
            GroesseAnaglyphe[0]=(TempLinkesBild.GetWidth()-TempOffsetLinks[0]);
        }else{
            GroesseAnaglyphe[0]=(TempRechtesBild.GetWidth()-TempOffsetRechts[0]);
        }

        if((TempLinkesBild.GetHeight()-TempOffsetLinks[1])<(TempRechtesBild.GetHeight()-TempOffsetRechts[1]))
        {
            GroesseAnaglyphe[1]=(TempLinkesBild.GetHeight()-TempOffsetLinks[1]);
        }else{
            GroesseAnaglyphe[1]=(TempRechtesBild.GetHeight()-TempOffsetRechts[1]);
        }

        if(GroesseAnaglyphe[0]<=0){
            Anaglyphe.Resize(wxSize(1, 1), wxPoint(0, 0));
            return;
        }
        if(GroesseAnaglyphe[1]<=0){
            Anaglyphe.Resize(wxSize(1, 1), wxPoint(0, 0));
            return;
        }

        Anaglyphe.Resize(wxSize(GroesseAnaglyphe[0], GroesseAnaglyphe[1]), wxPoint(0, 0));

        //Width = wxString::Format(wxT("%i"),Zahl);

        /*Data der Teilbilder holen und in einen Datensatz für die Anaglyphe "mischen"*/
        unsigned char* RechtsData;
        unsigned char* LinksData;
        unsigned char* AnaglypheData;
        RechtsData = TempRechtesBild.GetData();
        LinksData = TempLinkesBild.GetData();
        AnaglypheData = (unsigned char *) malloc(3 * GroesseAnaglyphe[0] * GroesseAnaglyphe[1]);

        //nur wenn Speicherallozierung erfolgreich war - sonst ist AnaglyphData = NULL
        if(AnaglypheData)
        {
            int StelleRechts=(TempOffsetRechts[1]*TempRechtesBild.GetWidth()+TempOffsetRechts[0])*3;
            int StelleLinks=(TempOffsetLinks[1]*TempLinkesBild.GetWidth()+TempOffsetLinks[0])*3;
            int Stelle=0;

            for(int Reihe=0; Reihe<GroesseAnaglyphe[1]; Reihe++)
            {
                for(int Spalte=0; Spalte<GroesseAnaglyphe[0]; Spalte++)
                {
                    /*Hier wird das neue Bild geschrieben*/
                    AnaglypheData[Stelle]=LinksData[StelleLinks];
                    AnaglypheData[(Stelle+1)]=LinksData[(StelleLinks+1)];//*0.75;
                    AnaglypheData[(Stelle+2)]=RechtsData[(StelleRechts+2)];
                    Stelle+=3;
                    StelleRechts+=3;
                    StelleLinks+=3;
                }
            StelleRechts+=((TempRechtesBild.GetWidth()-GroesseAnaglyphe[0])*3);
            StelleLinks+=((TempLinkesBild.GetWidth()-GroesseAnaglyphe[0])*3);
            }
        Anaglyphe.SetData(AnaglypheData);
        }
    }
    return;
}

/*************/
/*Stereogramm*/
/*************/
StereoBild::StereoBild() : Bild3D()
{
    /*Log starten*/
    logName = ("StereoBild.log");
	Logbuch = new ofstream(logName, ios_base::out|ios_base::trunc);
    if(Logbuch->good())
    {
        *Logbuch<<"Logbuch gestartet\n";
        Logbuch->close();
    }
    MinGroessenFinden();
}

StereoBild::StereoBild(const Bild3D& Obj) : Bild3D(Obj)
{
    if(this != &Obj)
    {
        /*Log starten*/
        logName = ("StereoBild.log");
        Logbuch = new ofstream(logName, ios_base::out|ios_base::trunc);
        if(Logbuch->good())
        {
            *Logbuch<<"Logbuch gestartet\n";
            Logbuch->close();
        }
    MinGroessenFinden();
    }
}

StereoBild::~StereoBild()
{
    logSchreiben("\nLogbuch geschlossen\n");
	delete Logbuch;
}

void StereoBild::Hochzeit(float Fktr)
{
    int GroesseAnaglyphe[2] = {0, 0};
    AnzeigeFaktor = Fktr;

    int lokalerRand = (int)(Rand * AnzeigeVergroesserung);

    /*Exceptionhandling*/
    if(!(LinkesBild.Ok()))throw wxT("Linkes Bild nicht OK");
    if(!(RechtesBild.Ok()))throw wxT("Rechtes Bild nicht OK");
    /*Ende Exeptionhandling*/

    wxImage TempLinkesBild = LinkesBild.Scale((int)(LinkesBild.GetWidth()*AnzeigeFaktor), (int)(LinkesBild.GetHeight()*AnzeigeFaktor));
    wxImage TempRechtesBild = RechtesBild.Scale((int)(RechtesBild.GetWidth()*AnzeigeFaktor), (int)(RechtesBild.GetHeight()*AnzeigeFaktor));

    Vektor TempOffset = ((Offset + deltaOffset) * AnzeigeFaktor * AnzeigeVergroesserung);

    int TempOffsetLinks[2], TempOffsetRechts[2];
    TempOffsetLinks[0] = (TempOffset.x() > 0) ?  (int)(TempOffset.x()+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/
    TempOffsetLinks[1] = (TempOffset.z() > 0) ?  (int)(TempOffset.z()+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/

    TempOffsetRechts[0] = (TempOffset.x() < 0) ? (int)(-(TempOffset.x())+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/
    TempOffsetRechts[1] = (TempOffset.z() < 0) ? (int)(-(TempOffset.z())+0.5) : 0; /*+0.5 = Aufrunden beim cast nach int*/

    int a_oben=0, a_links=0, a_breite=0, a_hoehe=0;/*Ausschnitt*/

    if(TempLinkesBild.Ok() && TempRechtesBild.Ok())
    {
        /*neue Groesse der Anaglyphe*/
        if((TempLinkesBild.GetWidth()-TempOffsetLinks[0])<(TempRechtesBild.GetWidth()-TempOffsetRechts[0]))
        {
            GroesseAnaglyphe[0]=(TempLinkesBild.GetWidth()-TempOffsetLinks[0]);
        }else{
            GroesseAnaglyphe[0]=(TempRechtesBild.GetWidth()-TempOffsetRechts[0]);
        }

        if((TempLinkesBild.GetHeight()-TempOffsetLinks[1])<(TempRechtesBild.GetHeight()-TempOffsetRechts[1]))
        {
            GroesseAnaglyphe[1]=(TempLinkesBild.GetHeight()-TempOffsetLinks[1]);
        }else{
            GroesseAnaglyphe[1]=(TempRechtesBild.GetHeight()-TempOffsetRechts[1]);
        }

        /*Ausschnitt*/
        if(zuschnittAktiv)
        {
            wxRect bildGroesse = Zuschnitt->GetAuswahl();
            a_links = bildGroesse.GetLeft() * AnzeigeVergroesserung * AnzeigeFaktor + GroesseAnaglyphe[0]/2;
            a_oben = bildGroesse.GetTop() * AnzeigeVergroesserung * AnzeigeFaktor + GroesseAnaglyphe[1]/2;
            a_breite = bildGroesse.GetWidth() * AnzeigeVergroesserung * AnzeigeFaktor;
            a_hoehe = bildGroesse.GetHeight() * AnzeigeVergroesserung * AnzeigeFaktor;
            GroesseAnaglyphe[0] = a_breite;
            GroesseAnaglyphe[1] = a_hoehe;

            if(a_links<0)
            {
                a_links = 0;
            }
            if(a_oben<0)
            {
                a_oben = 0;
            }
        }/*Ausschnitt Ende*/
        logSchreiben("\nAusschnitt:\nlinks-oben:\t");logSchreiben(a_links);logSchreiben(" / ");logSchreiben(a_oben);
        logSchreiben("\nGroesse:\t");logSchreiben(a_breite);logSchreiben(" / ");logSchreiben(a_hoehe);logSchreiben("\n");

        if(GroesseAnaglyphe[0]<=0){
            Anaglyphe.Resize(wxSize(1, 1), wxPoint(0, 0));
            return;
        }
        if(GroesseAnaglyphe[1]<=0){
            Anaglyphe.Resize(wxSize(1, 1), wxPoint(0, 0));
            return;
        }
        Anaglyphe.Resize(wxSize(GroesseAnaglyphe[0]*2+3*lokalerRand, GroesseAnaglyphe[1]+2*lokalerRand), wxPoint(0, 0));
    }

    wxBitmap Leinwand = wxBitmap(Anaglyphe.GetWidth(), Anaglyphe.GetHeight());
    TempLinkesBild = TempLinkesBild.GetSubImage(wxRect(TempOffsetLinks[0]+a_links, TempOffsetLinks[1]+a_oben, GroesseAnaglyphe[0], GroesseAnaglyphe[1]));
    TempRechtesBild = TempRechtesBild.GetSubImage(wxRect(TempOffsetRechts[0]+a_links, TempOffsetRechts[1]+a_oben, GroesseAnaglyphe[0], GroesseAnaglyphe[1]));
    if(Leinwand.Ok())
    {
        wxMemoryDC Staffelei;
        Staffelei.SelectObject(Leinwand);
        Staffelei.DrawBitmap(wxBitmap(TempRechtesBild), lokalerRand, lokalerRand);
        Staffelei.DrawBitmap(wxBitmap(TempLinkesBild), GroesseAnaglyphe[0]+2*lokalerRand, lokalerRand);
    }

    Anaglyphe = Leinwand.ConvertToImage();
    return;
}

void StereoBild::MinGroessenFinden(void)
{
    if(LinkesOriginal.Ok())
    {
        MinSizeX = LinkesOriginal.GetWidth();
        MinSizeY = LinkesOriginal.GetHeight();
    }
    if(RechtesOriginal.Ok())
    {
        MinSizeX = RechtesOriginal.GetWidth();
        MinSizeY = RechtesOriginal.GetHeight();
    }
    if(LinkesOriginal.Ok() && RechtesOriginal.Ok())
    {
        MinSizeX = LinkesOriginal.GetWidth();
        MinSizeY = LinkesOriginal.GetHeight();
        if(RechtesOriginal.GetWidth()<MinSizeX)  MinSizeX = RechtesOriginal.GetWidth();
        if(RechtesOriginal.GetHeight()<MinSizeY) MinSizeY = RechtesOriginal.GetHeight();
    }
    Rand = (MinSizeX>MinSizeY) ? (MinSizeX*randProzent) : (MinSizeY*randProzent);

    MinSizeX += 3*Rand + MinSizeX;
    MinSizeY += 2*Rand;
    return;
}

void StereoBild::SetzeDrehpunkt(wxPoint Pkt)
{
    Bild3D::SetzeDrehpunkt(Pkt);
    if(LinksAktiv)
    {
        DrehPunkt -= Vektor((Anaglyphe.GetWidth()/(4*AnzeigeVergroesserung)-Rand/4), 0, 0);
    }else{
        DrehPunkt += Vektor((Anaglyphe.GetWidth()/(4*AnzeigeVergroesserung)-Rand/4), 0, 0);
    }
    logSchreiben("Drehpunkt: ");logSchreiben(DrehPunkt.x(), 2);logSchreiben(" / ");logSchreiben(DrehPunkt.z(), 2); logSchreiben("\n");
    return;
}

wxPoint StereoBild::Drehen(double Winkel)
{
    wxPoint Rueckgabe = Bild3D::Drehen(Winkel);
    if(LinksAktiv)
    {
        Rueckgabe = wxPoint(Rueckgabe.x+(int)((Anaglyphe.GetWidth()-Rand*AnzeigeVergroesserung)/4), Rueckgabe.y);
    }else{
        Rueckgabe = wxPoint(Rueckgabe.x-(int)((Anaglyphe.GetWidth()-Rand*AnzeigeVergroesserung)/4), Rueckgabe.y);
    }
    logSchreiben("Drehen in Stereobild\n");
    logSchreiben("Drehpunkt: ");logSchreiben(DrehPunkt.x(), 2);logSchreiben(" / ");logSchreiben(DrehPunkt.z(), 2); logSchreiben("\n");
    logSchreiben("Drehpunkt (Rueckgabe): ");logSchreiben(Rueckgabe.x, 0);logSchreiben(" / ");logSchreiben(Rueckgabe.y, 0); logSchreiben("\n");
    return Rueckgabe;
}

void StereoBild::ZuschnittEingrenzen()
{
    if(LinkesBild.Ok()&&RechtesBild.Ok())
    {
        int oEx, oEy, uEx, uEy; //obere Ecke (x,y) und untere Ecke (x,y)
        oEx = F_Max(0, Offset.x());
        oEy = F_Max(0, Offset.z());
        uEx = F_Min(LinkesBild.GetWidth()/AnzeigeVergroesserung, RechtesBild.GetWidth()/AnzeigeVergroesserung + Offset.x());
        uEy = F_Min(LinkesBild.GetHeight()/AnzeigeVergroesserung, RechtesBild.GetHeight()/AnzeigeVergroesserung + Offset.z());
        Zuschnitt->SetGrenzen(wxRect(-(uEx-oEx)/2, -(uEy-oEy)/2, (uEx-oEx), (uEy-oEy)));
        logSchreiben("Zuschnitt eingegrenzt\n");
    } else
    if(LinkesBild.Ok())
    {
        Zuschnitt->SetGrenzen(wxRect(0, 0, 1, 1));
        logSchreiben("Nur ein Teilbild vorhanden, Grenzen minimal\n");
    } else
    if(RechtesBild.Ok())
    {
        Zuschnitt->SetGrenzen(wxRect(0, 0, 1, 1));
        logSchreiben("Nur ein Teilbild vorhanden, Grenzen minimal\n");
    } else throw(wxT("keine Bilder vorhanden"));
}

void StereoBild::SetZuschnittStatus(wxPoint schirmMarke)
{
    int markeX = (int)(((float)schirmMarke.x - (float)GroesseDCX/2) / AnzeigeVergroesserung);
    int markeY = (int)(((float)schirmMarke.y - (float)GroesseDCY/2) / AnzeigeVergroesserung);
    if(markeX <0)
    {
        markeX -= (-(Anaglyphe.GetWidth()/(2*AnzeigeVergroesserung) - Rand) - Zuschnitt->GetLinks());
    }else{
        markeX -= (Rand/2 - Zuschnitt->GetLinks());
    }
    markeY -= -(Anaglyphe.GetHeight() / (2*AnzeigeVergroesserung) - Rand) - Zuschnitt->GetOben();
    Zuschnitt->SetStatus(wxPoint(markeX, markeY));
    return;
}

wxCursor StereoBild::MausStellung(wxPoint schirmMarke)
{
    int markeX = (int)(((float)schirmMarke.x - (float)GroesseDCX/2) / AnzeigeVergroesserung);
    int markeY = (int)(((float)schirmMarke.y - (float)GroesseDCY/2) / AnzeigeVergroesserung);
    if(markeX <0)
    {
        markeX -= (-(Anaglyphe.GetWidth()/(2*AnzeigeVergroesserung) - Rand) - Zuschnitt->GetLinks());
    }else{
        markeX -= (Rand/2 - Zuschnitt->GetLinks());
    }
    markeY -= -(Anaglyphe.GetHeight() / (2*AnzeigeVergroesserung) - Rand) - Zuschnitt->GetOben();
    return(Zuschnitt->MausStellung(wxPoint(markeX, markeY)));
}

void StereoBild::Draw3DBild(wxDC &dc)
{
    wxImage DrawImage;

    ZuschnittEingrenzen();
    Hochzeit(1);

    if(Anaglyphe.Ok())
    {
        logSchreiben("\nAnaglyphe ist i. O.\n");
        logSchreiben("Groeße: ");logSchreiben((float)Anaglyphe.GetWidth(), 0);
        logSchreiben(" / ");logSchreiben((float)Anaglyphe.GetHeight(), 0);
        logSchreiben("\n");
        DrawImage = Anaglyphe.Copy();
    } else
    if(LinkesBild.Ok())
    {
        logSchreiben("\nAnaglyphe nicht OK\n");
        DrawImage = LinkesBild.Copy();
    } else
    if(RechtesBild.Ok())
    {
        logSchreiben("\nAnaglyphe nicht OK\n");
        DrawImage = RechtesBild.Copy();
    } else throw(wxT("keine Bilder vorhanden"));

    wxSize dcSize = wxSize(GroesseDCX, GroesseDCY);//dc.GetSize();
    int DrawSizeX = (int)(DrawImage.GetWidth() / AnzeigeFaktor);
    int DrawSizeY = (int)(DrawImage.GetHeight() / AnzeigeFaktor);

    dc.DrawBitmap(wxBitmap(DrawImage.Rescale(DrawSizeX, DrawSizeY)),
                    ((dcSize.GetWidth()-DrawSizeX)/2),
                    ((dcSize.GetHeight()-DrawSizeY)/2), true);

    return;
}


bool StereoBild::SpeicherAnaglyphe(wxString Pfad)
{
    logSchreiben("\nSpeicherung der Anaglyphe\n");
    bool Erfolg;
    float tempVergroesserung = AnzeigeVergroesserung;
    AnzeigeVergroesserung = 1.0;

    LinkesBild = LinkesOriginal.Rotate((LinkeDrehung+deltaLinkeDrehung), wxPoint(0, 0), true);
    RechtesBild = RechtesOriginal.Rotate((RechteDrehung+deltaRechteDrehung), wxPoint(0, 0), true);
    EffektPruefen();
    Hochzeit(1);
    logSchreiben("Hochzeit erfolgreich\n");
    AnzeigeVergroesserung = tempVergroesserung;

    Erfolg = Anaglyphe.SaveFile(Pfad, wxBITMAP_TYPE_PNG);
    if(Erfolg)
    {
        logSchreiben("Anaglyphe wurde gespeichert\n");
        logSchreiben(static_cast<const char*>(Pfad));
    }
    /*Bilder skaliert neu laden*/
    NeuLaden();
    EffektPruefen();
    return Erfolg;
}
