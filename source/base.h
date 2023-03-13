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

#ifndef __BASE_H
#define __BASE_H
#include <wx/spinctrl.h>
#include "Bild3D.h"

class MainFrame;
class SliderFrame;
class LizenzAnzeige;
class StGrAnzeige;

class MainApp: public wxApp
{
  public:
      virtual bool OnInit();
};

class SWSlider: public wxSlider
{
  public:
	SWSlider(SliderFrame* parent, wxWindowID id, int value, int minValue, int maxValue, const wxPoint& point, const wxSize& size);
	~SWSlider();
    void OnSWSliderChange(wxScrollEvent& event);
  private:
	SliderFrame* Eltern;
	DECLARE_EVENT_TABLE()
};

class SliderFrame: public wxFrame
{
  public:
    SliderFrame(MainFrame* parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long);
    ~SliderFrame();
    void OnSize(wxSizeEvent&);
    void OnSWSliderChange(unsigned char);
    void SetValue(int);
  private:
	MainFrame* Eltern;
	SWSlider* Slider;
	DECLARE_EVENT_TABLE()
};

class LizenzAnzeige: public wxFrame
{
  public:
    LizenzAnzeige(wxWindow* parent, const wxString &title, const wxPoint &pos, const wxSize &size, long);
    ~LizenzAnzeige();
    void OnSize(wxSizeEvent& event);
    void OnClose(wxCloseEvent& event);
    void LizenzEinlesen();
  private:
    wxButton* Akzeptieren;
    wxButton* Ablehnen;
    wxTextCtrl* LizenzText;
    DECLARE_EVENT_TABLE()
};

class MyPanel: public wxPanel
{
public:
  MyPanel(wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxSize(1,1), long style=wxTAB_TRAVERSAL, const wxString &name=wxPanelNameStr);
  void OnPanelKey(wxKeyEvent &event);
  void OnPanelKeyRelease(wxKeyEvent &event);
private:
  wxFrame* mama;
  DECLARE_EVENT_TABLE()
};

class MainFrame: public wxFrame
{
  private:
    enum ProgrammModus { ID_MODUS_VERSCHIEBEN = 100, ID_MODUS_DREHEN, ID_MODUS_LEERLAUF, ID_MODUS_BILDAUSSCHNITT, ID_MODUS_AUSSCHNITT } ModusID;
  public:
    MainFrame(MainFrame*);
    MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long);
    ~MainFrame();
    void OnQuit(wxCommandEvent& event);
    void OnOpenLeftPic(wxCommandEvent& event);
    void OnOpenRightPic(wxCommandEvent& event);
    void OnSavePic(wxCommandEvent& event);
    void OnTauscheSeiten(wxCommandEvent& event);
    void OnGraustufen(wxCommandEvent& event);
    void OnSchwarzWeiss(wxCommandEvent& event);
    void OnVerschieben(wxCommandEvent& event);
    void OnDrehen(wxCommandEvent& event);
    void OnBildAusschnitt(wxCommandEvent& event);
    void OnWechselAktivesBild(wxCommandEvent& event);
    void OnInfo(wxCommandEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKey(wxKeyEvent &event);
    void OnKeyRelease(wxKeyEvent &event);
    void OnSWSliderChange(unsigned char);
    void OnBrilleWechseln(wxCommandEvent& event);
    void ModusUmschalten(ProgrammModus);
    void UnCheckAll(void);
    unsigned char HoleSWSchwellen(void);
  private:
    /*Logbuch*/
    ofstream *Logbuch;
    void logSchreiben(const char*);
    void logSchreiben(const float msg, int i=0);
    MyPanel* panel;
    //wxBitmap Anzeige;
    Bild3D*  DasBild;
    wxFileDialog *PictureOpener;
    wxFileDialog *PictureSaver;
    wxMenu* DateiMenu;
    wxMenu* BearbeitenMenu;
    wxMenu* InfoMenu;
    wxMenuBar* MenuBar;
    wxMenuItem* MenuPunktGrau;
    wxMenuItem* MenuPunktSW;
    wxMenuItem* MenuWechselBild;
    wxMenuItem* Menu_RC_Brille;
    wxMenuItem* Menu_MG_Brille;
    wxMenuItem* Menu_AB_Brille;
    wxMenuItem* MenuStereogramm;
    wxMenuItem* MenuVerschieben;
    wxMenuItem* MenuDrehen;
    wxMenuItem* MenuAusschnitt;
    wxMenuItem* MenuLizenz;
    SliderFrame* SW_Slider;

    wxPoint AlteMousePosition, NeueMousePosition, DrehPunkt;
    float BildFaktor;
    bool Graustufen;
    bool MausAussetzen, taste_RAND_unten=0, taste_SW_unten=0;
    wxCursor DerCursor, Bullseye_Cursor;
    void Cursor_Malen(void);
    void NeuAnlegen(int);
    int ANZ_SCHATTIEREN;
    DECLARE_EVENT_TABLE()
};

enum
{ ID_MAINWIN_QUIT = 100, ID_MAINWIN_OPEN_PIC_LEFT, ID_MAINWIN_OPEN_PIC_RIGHT, ID_MAINWIN_TAUSCHEN, ID_MAINWIN_GRAUSTUFEN, ID_MAINWIN_SCHWAWEI,
    ID_MAINWIN_WECHSEL_AKTIV, ID_MAINWIN_VERSCHIEBEN, ID_MAINWIN_DREHEN, ID_MAINWIN_SAVE_ANA, ID_MAINWIN_AUSSCHNITT,
    ID_MAINWIN_INFO, ID_MAINWIN_RC_BRILLE, ID_MAINWIN_MG_BRILLE, ID_MAINWIN_AB_BRILLE, ID_MAINWIN_STEREO };

enum
{ ID_CHILD_SWSLIDER = 500 };

#endif
