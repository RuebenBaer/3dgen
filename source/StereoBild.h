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

#ifndef __StereoBild_H_
#define __StereoBild_H_
#include <wx/wx.h>
#include <wx/image.h>
#include "Vektor/Vektor.h"
#include <iostream>
#include <fstream>

using namespace std;

class StereoBild{
    private:
        /*Logbuch*/
        ofstream *Logbuch;
        void logSchreiben(const char*);
        void logSchreiben(const float msg, int i=0);
        /*Variablen*/
        wxImage rechtesBild, linkesBild, stereoGramm;
        double linkeDrehung, rechteDrehung;
        Vektor Offset;
        wxRect Zuschnitt;
        bool zuschnittAktiv;
        double randProzent;
        /*Stereogramm berechnen*/
        void Hochzeit(void);
    public:
        StereoBild(wxImage, wxImage, double, double, Vektor, wxRect, bool, double);
        /*Stereogramm ausgeben*/
        wxImage HoleStereoGramm(void);
};
#endif
