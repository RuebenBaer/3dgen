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

#include <cstdlib>
#include "Auswahl.h"

AuswahlRechteck::AuswahlRechteck()
{
    links = -2000;
    oben = -2000;
    rechts = 2000;
    unten = 2000;
    PfeilWO = new wxCursor(wxCURSOR_SIZEWE);
    PfeilNS = new wxCursor(wxCURSOR_SIZENS);
    PfeilNOSW = new wxCursor(wxCURSOR_SIZENESW);
    PfeilNWSO = new wxCursor(wxCURSOR_SIZENWSE);
    PfeilKreuz = new wxCursor(wxCURSOR_SIZING);
    PfeilDefault = new wxCursor(wxCURSOR_ARROW);
}

AuswahlRechteck::AuswahlRechteck(wxRect BereichGroesse)
{
    links = BereichGroesse.GetPosition().x;
    oben = BereichGroesse.GetPosition().y;
	rechts = BereichGroesse.GetWidth() + links;
    unten = BereichGroesse.GetHeight() + oben;
    PfeilWO = new wxCursor(wxCURSOR_SIZEWE);
    PfeilNS = new wxCursor(wxCURSOR_SIZENS);
    PfeilNOSW = new wxCursor(wxCURSOR_SIZENESW);
    PfeilNWSO = new wxCursor(wxCURSOR_SIZENWSE);
    PfeilKreuz = new wxCursor(wxCURSOR_SIZING);
    PfeilDefault = new wxCursor(wxCURSOR_ARROW);
}

AuswahlRechteck::~AuswahlRechteck()
{
    delete PfeilWO;
    delete PfeilNS;
    delete PfeilNOSW;
    delete PfeilNWSO;
    delete PfeilKreuz;
    delete PfeilDefault;
}

void AuswahlRechteck::Aendern(wxPoint von, wxPoint nach)
{
    switch(AuswahlStatusID)
    {
        case ID_STATUS_LINKS:
            links += nach.x-von.x;
            break;
        case ID_STATUS_RECHTS:
            rechts += nach.x-von.x;
            break;
        case ID_STATUS_OBEN:
            oben += nach.y-von.y;
            break;
        case ID_STATUS_UNTEN:
            unten += nach.y-von.y;
            break;
        case ID_STATUS_LINKSOBEN:
            links += nach.x-von.x;
            oben += nach.y-von.y;
            break;
        case ID_STATUS_LINKSUNTEN:
            links += nach.x-von.x;
            unten += nach.y-von.y;
            break;
        case ID_STATUS_RECHTSOBEN:
            rechts += nach.x-von.x;
            oben += nach.y-von.y;
            break;
        case ID_STATUS_RECHTSUNTEN:
            rechts += nach.x-von.x;
            unten += nach.y-von.y;
            break;
        case ID_STATUS_ALLES:
            links += nach.x-von.x;
            rechts += nach.x-von.x;
            oben += nach.y-von.y;
            unten += nach.y-von.y;
            break;
        case ID_STATUS_LEER:
            ;
    }
    GrenzenPruefen();
    return;
}

void AuswahlRechteck::GrenzenPruefen()
{
    if(links>rechts)
    {
        int tempLinks = links;
        links = rechts;
        rechts = tempLinks;
    }
    if(oben>unten)
    {
        int tempOben = oben;
        oben = unten;
        unten = tempOben;
    }
    if(links<minX)links=minX;
    if(oben<minY)oben=minY;
    if(rechts>maxX)rechts=maxX;
    if(unten>maxY)unten=maxY;
    return;
}


void AuswahlRechteck::SetStatus(wxPoint MausOrt)
{
    if((abs(MausOrt.x - links)<=10)&&((MausOrt.y - oben)>10)&&((MausOrt.y - unten)<-10))
    {
        AuswahlStatusID = ID_STATUS_LINKS;
        return;
    }
    if((abs(MausOrt.x - rechts)<=10)&&((MausOrt.y - oben)>10)&&((MausOrt.y - unten)<-10))
    {
        AuswahlStatusID = ID_STATUS_RECHTS;
        return;
    }
    if((abs(MausOrt.y - oben)<=10)&&((MausOrt.x - links)>10)&&((MausOrt.x - rechts)<-10))
    {
        AuswahlStatusID = ID_STATUS_OBEN;
        return;
    }
    if((abs(MausOrt.y - unten)<=10)&&((MausOrt.x - links)>10)&&((MausOrt.x - rechts)<-10))
    {
        AuswahlStatusID = ID_STATUS_UNTEN;
        return;
    }
    if(((MausOrt.x - links)>10)&&((MausOrt.x - rechts)<-10)&&((MausOrt.y - oben)>10)&&((MausOrt.y - unten)<-10))
    {
        AuswahlStatusID = ID_STATUS_ALLES;
        return;
    }
    if((abs(MausOrt.x - links)<=10)&&(abs(MausOrt.y - oben)<=10))
    {
        AuswahlStatusID = ID_STATUS_LINKSOBEN;
        return;
    }
    if((abs(MausOrt.x - links)<=10)&&(abs(MausOrt.y - unten)<=10))
    {
        AuswahlStatusID = ID_STATUS_LINKSUNTEN;
        return;
    }
    if((abs(MausOrt.x - rechts)<=10)&&(abs(MausOrt.y - oben)<=10))
    {
        AuswahlStatusID = ID_STATUS_RECHTSOBEN;
        return;
    }
    if((abs(MausOrt.x - rechts)<=10)&&(abs(MausOrt.y - unten)<=10))
    {
        AuswahlStatusID = ID_STATUS_RECHTSUNTEN;
        return;
    }
    AuswahlStatusID = ID_STATUS_LEER;
    return;
}

wxCursor AuswahlRechteck::MausStellung(wxPoint MausOrt)
{
	/*Alles in Koordinaten mit Vergroesserung*/
    if((abs(MausOrt.x - links)<=10)&&((MausOrt.y - oben)>10)&&((MausOrt.y - unten)<-10)) return (*PfeilWO);
    if((abs(MausOrt.x - rechts)<=10)&&((MausOrt.y - oben)>10)&&((MausOrt.y - unten)<-10)) return (*PfeilWO);

    if((abs(MausOrt.y - oben)<=10)&&((MausOrt.x - links)>10)&&((MausOrt.x - rechts)<-10)) return(*PfeilNS);
    if((abs(MausOrt.y - unten)<=10)&&((MausOrt.x - links)>10)&&((MausOrt.x - rechts)<-10)) return(*PfeilNS);

    if(((MausOrt.x - links)>10)&&((MausOrt.x - rechts)<-10)&&((MausOrt.y - oben)>10)&&((MausOrt.y - unten)<-10)) return(*PfeilKreuz);

    if((abs(MausOrt.x - links)<=10)&&(abs(MausOrt.y - oben)<=10)) return(*PfeilNWSO);
    if((abs(MausOrt.x - rechts)<=10)&&(abs(MausOrt.y - unten)<=10)) return(*PfeilNWSO);

    if((abs(MausOrt.x - links)<=10)&&(abs(MausOrt.y - unten)<=10)) return(*PfeilNOSW);
    if((abs(MausOrt.x - rechts)<=10)&&(abs(MausOrt.y - oben)<=10)) return(*PfeilNOSW);

    return(*PfeilDefault);
}

void AuswahlRechteck::SetAuswahl(wxRect tempGroesse)
{
    links = tempGroesse.GetPosition().x;
    oben = tempGroesse.GetPosition().y;
    rechts = tempGroesse.GetWidth() + links;
    unten = tempGroesse.GetHeight() + oben;
    GrenzenPruefen();
    return;
}

void AuswahlRechteck::SetGrenzen(wxRect tempGroesse)
{
    minX = tempGroesse.GetPosition().x;
    minY = tempGroesse.GetPosition().y;
    maxX = tempGroesse.GetWidth() + minX;
    maxY = tempGroesse.GetHeight() + minY ;
    GrenzenPruefen();
    return;
}

wxRect AuswahlRechteck::GetAuswahl(void)
{
    return(wxRect(links, oben, rechts-links, unten-oben));
}

void AuswahlRechteck::DrawAuswahl(wxDC& dc, wxSize dcSize, float fkt)
{
    int t_links, t_rechts, t_oben, t_unten;

    t_links = int(dcSize.GetWidth()/2 + links * fkt - 3);
    t_rechts = int(dcSize.GetWidth()/2 + rechts * fkt + 3);
    t_oben = int(dcSize.GetHeight()/2 + oben * fkt - 3);
    t_unten = int(dcSize.GetHeight()/2 + unten * fkt + 3);

    wxColour Linie(128, 128, 0);
    dc.SetBrush(wxBrush(Linie));
    dc.SetPen(wxPen(Linie, 3));
    dc.DrawLine(wxCoord (t_links), wxCoord (t_oben), wxCoord (t_rechts), wxCoord (t_oben));
    dc.DrawLine(wxCoord (t_links), wxCoord (t_unten), wxCoord (t_rechts), wxCoord (t_unten));
    dc.DrawLine(wxCoord (t_links), wxCoord (t_oben), wxCoord (t_links), wxCoord (t_unten));
    dc.DrawLine(wxCoord (t_rechts), wxCoord (t_oben), wxCoord (t_rechts), wxCoord (t_unten));

    t_links += 3;
    t_rechts -= 3;
    t_oben += 3;
    t_unten -= 3;

    dc.SetClippingRegion(wxRect(t_links, t_oben, t_rechts-t_links, t_unten-t_oben));

    return;
}

int AuswahlRechteck::DebugInfo(wxDC& dc, int Zeilen)
{
    wxFont TextFont = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dc.SetFont(TextFont);
    wxColour TextColour = wxColour(255, 255, 255);
    dc.SetTextForeground(TextColour);
    TextColour = wxColour(0,0,0);
    dc.SetTextBackground(TextColour);

    dc.DrawText(wxT("Zuschnitt:"), 10, Zeilen*12);
    Zeilen++;
    dc.DrawText(wxT("Links oben:\t") + wxString::Format(wxT("%i"),links) + wxT(" / ") + wxString::Format(wxT("%i"),oben), 10, Zeilen*12);
    Zeilen++;
    dc.DrawText(wxT("Rechts unten:\t") + wxString::Format(wxT("%i"),rechts) + wxT(" / ") + wxString::Format(wxT("%i"),unten), 10, Zeilen*12);
    Zeilen++;
    /*dc.DrawText(wxT("Grenzen:\t") + wxString::Format(wxT("%i"),minX) + wxT(",\t") + wxString::Format(wxT("%i"),minY) + wxT(",\t")
									+ wxString::Format(wxT("%i"),maxX) + wxT(",\t") + wxString::Format(wxT("%i"),maxY), 10, Zeilen*10);*/
    return (Zeilen);
}
