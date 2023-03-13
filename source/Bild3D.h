/*  3DGenerator - merges two pictures into one anaglyph using
    the 'red' value of the 'right' picture and the 'blue' and 'green' values of the left picture for red-cyan-glasses
    the 'red' and 'blue' value of the 'right' picture and the 'green' values of the left picture for magenta-green-glasses
    Copyright (C) 2012-2013  Ansgar R�tten

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

#ifndef __Bild3D_H_
#define __Bild3D_H_
#include <wx/wx.h>
#include <wx/image.h>
#include "Vektor/Vektor.h"
#include "Auswahl.h"
#include <iostream>
#include <fstream>

using namespace std;

enum EffektID { ID_3DBILD_GRAU=550, ID_3DBILD_SW, ID_3DBILD_NORMAL } ;
enum { ID_3DBILD_LINKS = 500, ID_3DBILD_RECHTS, ID_3DBILD_BEIDE };

class Bild3D{
    protected:
        /*Logbuch*/
        ofstream *Logbuch;
        wxString logName;
        void logSchreiben(const char*);
        void logSchreiben(const float msg, int i=0);
        /*Bild-Variablen*/
        wxString LinkerPfad, RechterPfad;
        wxImage LinkesBild, LinkesOriginal, RechtesBild, RechtesOriginal;
        wxImage Anaglyphe;
        Vektor DrehPunkt;
        Vektor Offset, deltaOffset, GroesseLinks, GroesseRechts; /*Offset = Abstand zwischen den linken oberen Ecken der Teilbilder
                                                        - Vektor von linkem Bild nach rechtem Bild; Width - Height*/
        double LinkeDrehung, RechteDrehung, deltaLinkeDrehung, deltaRechteDrehung;
        int MinSizeX, MinSizeY;
        int GroesseDCX, GroesseDCY;

        double randProzent;/*zun�chst nur Stereogramm*/
        int Rand;

        /*Anzeigensteuerung*/
        float ZoomLinks, ZoomRechts;
        float AnzeigeVergroesserung;/*zum Anzeigen im DC*/
        float AnzeigeFaktor;/*Verkleinerung zur Datenreduzierung*/
        virtual void MinGroessenFinden(void);

        /*Statussteuerung*/
        bool LinksAktiv;
        wxString DebugString;

        /*Bildzuschnitt*/
        AuswahlRechteck* Zuschnitt;
        wxCursor *PfeilDefault;
        bool zuschnittAktiv;

		/*Schwarz-Wei�-Effekt*/
        unsigned char SWSchwellen;/*Anzahl der Schwellen!*/

		/*interne "Modulo" Operation - Modulo f�r float*/
        float Rest(float, float);

		/*Funktionen*/
        /************/
        /*Effektsteuerung*/
        void InGrauUmwandeln(void);
        void InSWUmwandeln(void);
        void NeuLaden(void);
		    EffektID aktuellerEffekt;
        void EffektPruefen(void);
        /**/
        virtual void Hochzeit(float)=0;
        void Hochzeit(void);
        /**/
        void AnzeigeGroesseAnpassen();
    public:
        Bild3D();
        Bild3D(const Bild3D&);
        virtual ~Bild3D();
        void DrehungBeenden(void);
        wxRect GetAnzeigeGroesse(wxDC&);
        float GetAnzeigeFaktor(void);
        int HoleRand(void){return Rand;};

		/*laden, speichern und loeschen*/
        bool LadeLinkesBild(wxString);
        bool LadeRechtesBild(wxString);
        void LoescheLinkesBild(void);
        void LoescheRechtesBild(void);
        virtual bool SpeicherAnaglyphe(wxString);

		/*bearbeiten*/
        void SetzeSWSchwellen(unsigned char);/*Setzt Anzahl der Schwellen*/
        void InkrSWSchwellen(int);/*Inkrementiere Anzahl der Schwellen*/
        unsigned char HoleSWSchwellen(void);/*Holt Anzahl der Schwellen*/
        void TauscheLinksRechts(void);
        void ZoomeTeilBild(float);
        void AktiviereLinkesBild(void){LinksAktiv=1;}
        void AktiviereRechtesBild(void){LinksAktiv=0;}
        bool IstLinksAktiv(void) const {return LinksAktiv;}
        void Verschieben(int, int);
        virtual void SetzeDrehpunkt(wxPoint);
        virtual wxPoint Drehen(double);
        void SetzeRand(double);
        void InkrRand(double);
        void Effekt(EffektID);

        /*Bild zuschneiden*/
        void Zuschneiden(bool);
        bool Zuschneiden(void);
        void ZuschnittAendern(wxPoint, wxPoint);
        virtual void SetZuschnittStatus(wxPoint);
        virtual void ZuschnittEingrenzen(void);
        virtual wxCursor MausStellung(wxPoint);

		/*Anzeigesteuerung*/
        virtual void Draw3DBild(wxDC&);/*hier wird alles vorbereitet um das Bild korrekt anzuzeigen und ausgegeben*/
        void AnzeigeAnpassen(wxSize);
        /*Debuginformationen zur Anzeige im DC*/
        int DebugInfo(wxDC&, int);
};

class RotCyanAnaglyphe : public Bild3D
{
public:
    RotCyanAnaglyphe();
    RotCyanAnaglyphe(const Bild3D&);
    virtual ~RotCyanAnaglyphe();
private:
    virtual void Hochzeit(float);
};

class MagentaGruenAnaglyphe : public Bild3D
{
public:
    MagentaGruenAnaglyphe();
    MagentaGruenAnaglyphe(const Bild3D&);
    virtual ~MagentaGruenAnaglyphe();
private:
    virtual void Hochzeit(float);
};

class BernsteinBlauAnaglyphe : public Bild3D
{
public:
    BernsteinBlauAnaglyphe();
    BernsteinBlauAnaglyphe(const Bild3D&);
    virtual ~BernsteinBlauAnaglyphe();
private:
    virtual void Hochzeit(float);
};

class StereoBild : public Bild3D
{
public:
    StereoBild();
    StereoBild(const Bild3D&);
    virtual ~StereoBild();
private:
    void Hochzeit(float);
    void MinGroessenFinden(void);
    void SetzeDrehpunkt(wxPoint);
    /*Zuschnitt*/
    wxCursor MausStellung(wxPoint);
    void SetZuschnittStatus(wxPoint);
    void ZuschnittEingrenzen(void);
    wxPoint Drehen(double);
    /*Anzeige*/
    void Draw3DBild(wxDC&);
    bool SpeicherAnaglyphe(wxString);
};

#endif
