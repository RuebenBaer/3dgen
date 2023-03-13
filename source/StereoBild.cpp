#include "StereoBild.h"

StereoBild::StereoBild(wxImage links, wxImage rechts, double l_Dr, double r_Dr, Vektor Verschub, wxRect Auswahl, bool auswahlAktiv, double _randProzent)
{
    /*Log starten*/
	Logbuch = new ofstream("StereoBildLog.log", ios_base::out|ios_base::trunc);
    if(Logbuch->good())
    {
        *Logbuch<<"Logbuch gestartet\n";
        Logbuch->close();
    }

    linkesBild = links;
    rechtesBild = rechts;
    linkeDrehung = l_Dr;
    rechteDrehung = r_Dr;
    Offset = Verschub;
    Zuschnitt = Auswahl;
    zuschnittAktiv = auswahlAktiv;
    randProzent = _randProzent;
}

wxImage StereoBild::HoleStereoGramm(void)
{
    Hochzeit();
    return stereoGramm;
}

void StereoBild::Hochzeit(void)
{
    int Xmin, Zmin, Xmax, Zmax, Rand, deltaX, deltaZ;
    wxImage tempLinks, tempRechts;
    wxBitmap Leinwand;
    wxSize tempGroesse;

    Xmin = (Offset.x()> 0) ? 0 : (int)(-Offset.x());
    Zmin = (Offset.z()> 0) ? 0 : (int)(-Offset.z());

    Xmax = (linkesBild.GetWidth()-Offset.x() < rechtesBild.GetWidth())
            ? (int)(linkesBild.GetWidth()-Offset.x()) : rechtesBild.GetWidth();
    Zmax = (linkesBild.GetHeight()-Offset.z()< rechtesBild.GetHeight())
            ? (int)(linkesBild.GetHeight()-Offset.z()) : rechtesBild.GetHeight();

    if(zuschnittAktiv)
    {
        int breite, hoehe;
        (Zuschnitt.GetSize().GetWidth() < (Xmax-Xmin)) ? breite = Zuschnitt.GetSize().GetWidth() : breite = (Xmax-Xmin);
        (Zuschnitt.GetSize().GetHeight() < (Zmax-Zmin)) ? hoehe = Zuschnitt.GetSize().GetHeight() : hoehe = (Zmax-Zmin);
        tempGroesse = wxSize(breite, hoehe);
        deltaX = ((Xmax-Xmin) / 2 + Zuschnitt.GetLeft());
        deltaZ = ((Zmax-Zmin) / 2 + Zuschnitt.GetTop());

        logSchreiben("\nXmax: ");logSchreiben(float(Xmax), 0);logSchreiben("\nXmin: ");logSchreiben(float(Xmin), 0);
        logSchreiben("\nZuschnitt.x: ");logSchreiben(float(Zuschnitt.GetLeft()), 0);
        logSchreiben("\ndeltaX: ");logSchreiben(float(deltaX), 0);

        logSchreiben("\nZmax: ");logSchreiben(float(Zmax), 0);logSchreiben("\nZmin: ");logSchreiben(float(Zmin), 0);
        logSchreiben("\nZuschnitt.y: ");logSchreiben(float(Zuschnitt.GetTop()), 0);
        logSchreiben("\ndeltaZ: ");logSchreiben(float(deltaZ), 0);
    }else{
        tempGroesse = wxSize(Xmax-Xmin, Zmax-Zmin);
        deltaX = deltaZ = 0;
    }

    Rand = ((Xmax-Xmin) < (Zmax-Zmin)) ? ((Xmax-Xmin)*randProzent) : ((Zmax-Zmin)*randProzent);

    /*Rechtes Bild*/
    Xmin += deltaX;
    Zmin += deltaZ;
    if(Xmin < 0)Xmin = 0;
    if(Zmin < 0)Zmin = 0;

    wxPoint tempStart = wxPoint(Xmin, Zmin);
    tempRechts = rechtesBild.Rotate(rechteDrehung, wxPoint(0, 0), false).GetSubImage(wxRect(tempStart, tempGroesse));
    /*Linkes Bild*/
    tempStart = wxPoint((int)(Xmin + Offset.x()), (int)(Zmin + Offset.z()));
    tempLinks = linkesBild.Rotate(linkeDrehung, wxPoint(0, 0), false).GetSubImage(wxRect(tempStart, tempGroesse));

    Leinwand = wxBitmap(tempGroesse.GetWidth()*2+3*Rand, tempGroesse.GetHeight()+2*Rand);
    if(Leinwand.Ok())
    {
        wxMemoryDC Staffelei;
        Staffelei.SelectObject(Leinwand);
        Staffelei.DrawBitmap(wxBitmap(tempRechts), Rand, Rand);
        Staffelei.DrawBitmap(wxBitmap(tempLinks), tempGroesse.GetWidth()+2*Rand, Rand);
    }

    stereoGramm = Leinwand.ConvertToImage();
    return;
}

void StereoBild::logSchreiben(const char*msg)
{
    Logbuch->open("StereoBildLog.log",ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        *Logbuch<<msg;
        Logbuch->close();
    }
    return;
}

void StereoBild::logSchreiben(const float msg, int i)
{
    Logbuch->open("StereoBildLog.log",ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        Logbuch->setf( ios::fixed, ios::floatfield );
        Logbuch->precision(i);
        *Logbuch<<msg;
        Logbuch->close();
    }
    return;
}
