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

#ifndef __Auswahl_
#define __Auswahl_
#include <wx/wx.h>

/*Verwaltet den Auswahlbereich eines Bereichs von dessen Mitte aus gesehen*/

class AuswahlRechteck{
  private:
    int links, rechts, oben, unten;
    int minX, minY, maxX, maxY;
    wxCursor *PfeilWO, *PfeilNS, *PfeilNOSW, *PfeilNWSO, *PfeilKreuz, *PfeilDefault;
    enum AuswahlID { ID_STATUS_LINKS = 100, ID_STATUS_RECHTS, ID_STATUS_OBEN, ID_STATUS_UNTEN, ID_STATUS_LEER,
                            ID_STATUS_LINKSOBEN, ID_STATUS_LINKSUNTEN, ID_STATUS_RECHTSOBEN, ID_STATUS_RECHTSUNTEN, ID_STATUS_ALLES } AuswahlStatusID;
    void GrenzenPruefen();

  public:
    AuswahlRechteck();
    AuswahlRechteck(wxRect);
    ~AuswahlRechteck();

	/*Änderungen vornehmen*/
    void Aendern(wxPoint, wxPoint);

    /*Festlegungen*/
    void SetStatus(wxPoint);
    void SetAuswahl(wxRect);
    void SetGrenzen(wxRect);

    /*Ausgaben von Werten*/
    wxRect GetAuswahl(void);
    int GetOben(void){return oben;};
    int GetLinks(void){return links;};
    wxCursor MausStellung(wxPoint);

    /*Ausgabe auf einem DC*/
    void DrawAuswahl(wxDC&, wxSize, float);
    int DebugInfo(wxDC&, int);
  };

#endif
