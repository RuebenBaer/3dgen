/* TODO (ANSI#1#): Implementierung Rand einstellen*/
/* TODO (ANSI#1#): Speicherfunktion fuer Stereogramm*/
/* TODO (ANSI#2#): Menus fuer Stereogrammanzeige*/
/* TODO (ANSI#2#): Keys fuer Stereogrammanzeige*/
/* TODO (ANSI#3#): Farbe des Randes einstellen Option*/
/* DONE (Ansi#1#): Fehler beim drehen suchen - Debug Infos einbauen*/
/* DONE (Ansi#3#): SW_Slider als eigenes Fenster*/
/* DONE (Ansi#1#): alles mit new erzeugt mit delete entfernen*/
/*  3DGenerator - merges two pictures into one anaglyph using
    the 'red' value of the 'right' picture and the 'blue' and 'green' values of the left picture for red-cyan-glasses
    the 'red' and 'blue' value of the 'right' picture and the 'green' values of the left picture for magenta-green-glasses
    Copyright (C) 2012-2013  Ansgar Ruetten

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
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "Auswahl.h"
#include "base.h"
#include <wx/imagpng.h>
#include <wx/imagjpeg.h>
#include <wx/dcbuffer.h>
#include <cmath>

IMPLEMENT_APP(MainApp);
wxDECLARE_APP(MainApp);
bool ARU_DEBUG = 1;

wxDEFINE_EVENT(LIZENZ_SCHLIESST, wxCommandEvent);

bool MainApp::OnInit()
{
    MainFrame* hauptFrame = new MainFrame(_("3D Generator - Version 1.0"),wxPoint(0,0),wxSize(400,400),wxDEFAULT_FRAME_STYLE);
    SetExitOnFrameDelete(TRUE);
    hauptFrame->Show(TRUE);
    SetTopWindow(hauptFrame);
    return TRUE;
}

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
  EVT_KEY_DOWN(MyPanel::OnPanelKey)
  EVT_KEY_UP(MyPanel::OnPanelKeyRelease)
END_EVENT_TABLE()

MyPanel::MyPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name):wxPanel(parent, id, pos, size, style, name)
{
  mama = (MainFrame*)parent;
}

void MyPanel::OnPanelKey(wxKeyEvent &event)
{
  event.ResumePropagation(1);
  event.Skip();
}

void MyPanel::OnPanelKeyRelease(wxKeyEvent &event)
{
  event.ResumePropagation(1);
  event.Skip();
}

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_MAINWIN_QUIT, MainFrame::OnQuit)
    EVT_MENU(ID_MAINWIN_OPEN_PIC_LEFT, MainFrame::OnOpenLeftPic)
    EVT_MENU(ID_MAINWIN_OPEN_PIC_RIGHT, MainFrame::OnOpenRightPic)
    EVT_MENU(ID_MAINWIN_SAVE_ANA, MainFrame::OnSavePic)
    EVT_MENU(ID_MAINWIN_GRAUSTUFEN, MainFrame::OnGraustufen)
    EVT_MENU(ID_MAINWIN_SCHWAWEI, MainFrame::OnSchwarzWeiss)
    EVT_MENU(ID_MAINWIN_VERSCHIEBEN, MainFrame::OnVerschieben)
    EVT_MENU(ID_MAINWIN_DREHEN, MainFrame::OnDrehen)
    EVT_MENU(ID_MAINWIN_TAUSCHEN, MainFrame::OnTauscheSeiten)
    EVT_MENU(ID_MAINWIN_WECHSEL_AKTIV, MainFrame::OnWechselAktivesBild)
    EVT_MENU(ID_MAINWIN_AUSSCHNITT, MainFrame::OnBildAusschnitt)
    EVT_MENU(ID_MAINWIN_INFO, MainFrame::OnInfo)
    EVT_MENU(ID_MAINWIN_RC_BRILLE, MainFrame::OnBrilleWechseln)
    EVT_MENU(ID_MAINWIN_MG_BRILLE, MainFrame::OnBrilleWechseln)
    EVT_MENU(ID_MAINWIN_AB_BRILLE, MainFrame::OnBrilleWechseln)
    EVT_MENU(ID_MAINWIN_STEREO, MainFrame::OnBrilleWechseln)
    EVT_PAINT(MainFrame::OnPaint)
    EVT_SIZE(MainFrame::OnSize)
    EVT_LEFT_DOWN(MainFrame::OnLeftDown)
    EVT_LEFT_UP(MainFrame::OnLeftUp)
    EVT_MOTION(MainFrame::OnMouseMove)
    EVT_KEY_DOWN(MainFrame::OnKey)
    EVT_KEY_UP(MainFrame::OnKeyRelease)
    EVT_ERASE_BACKGROUND(MainFrame::OnEraseBackground)
END_EVENT_TABLE()

MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame((wxFrame *) NULL, -1, title, pos, size, style)
{
    /*Log starten*/
	Logbuch = new ofstream("FensterLog.log", ios_base::out|ios_base::trunc);
  if(Logbuch->good())
  {
      *Logbuch<<"Logbuch gestartet\n";
      Logbuch->close();
  }

  /*Menus vorbereiten*/
  DateiMenu = new wxMenu;
  BearbeitenMenu = new wxMenu;
  InfoMenu = new wxMenu;
  MenuBar = new wxMenuBar;

  //Datei-Menu
  DateiMenu->Append(ID_MAINWIN_OPEN_PIC_RIGHT, wxT("&Rechtes Bild oeffnen"));
  DateiMenu->Append(ID_MAINWIN_OPEN_PIC_LEFT, wxT("&Linkes Bild oeffnen"));
  DateiMenu->AppendSeparator();
  DateiMenu->Append(ID_MAINWIN_SAVE_ANA, _("&3D-Bild speichern"));
  DateiMenu->AppendSeparator();
  DateiMenu->Append(ID_MAINWIN_QUIT, _("&Beenden"));

  MenuBar->Append(DateiMenu, _("&Datei"));

  //Bearbeiten-Menu
  Menu_RC_Brille = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_RC_BRILLE, _("&Rot-Cyan-Brille"), _(""), wxITEM_CHECK);
  BearbeitenMenu->Append(Menu_RC_Brille);
  Menu_RC_Brille->Check();
  Menu_MG_Brille = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_MG_BRILLE, _("&Magenta-Gruen-Brille"), _(""), wxITEM_CHECK);
  BearbeitenMenu->Append(Menu_MG_Brille);
  Menu_AB_Brille = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_AB_BRILLE, _("&Bernstein-Blau-Brille"), _(""), wxITEM_CHECK);
  BearbeitenMenu->Append(Menu_AB_Brille);
  MenuStereogramm = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_STEREO, _("&Stereogramm anzeigen"), _(""), wxITEM_CHECK);
  BearbeitenMenu->Append(MenuStereogramm);
  BearbeitenMenu->AppendSeparator();
  BearbeitenMenu->Append(ID_MAINWIN_TAUSCHEN, _("Ver&tausche Links und Rechts"));
  MenuWechselBild = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_WECHSEL_AKTIV, _("&Rechtes Bild bearbeiten (links aktiv)"), _(""));
  BearbeitenMenu->Append(MenuWechselBild);
  MenuVerschieben = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_VERSCHIEBEN, _("Teilbild &verschieben"), _("F6"));
  BearbeitenMenu->Append(MenuVerschieben);
  MenuDrehen = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_DREHEN, _("Teilbild &drehen"), _("F5"));
  BearbeitenMenu->Append(MenuDrehen);
  BearbeitenMenu->AppendSeparator();
  MenuPunktGrau = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_GRAUSTUFEN, _("&Graustufen"), _(""), wxITEM_CHECK);
  BearbeitenMenu->Append(MenuPunktGrau);
  MenuPunktSW = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_SCHWAWEI, _("&Schwarz-Weiss"), _(""), wxITEM_CHECK);
  BearbeitenMenu->Append(MenuPunktSW);
  BearbeitenMenu->AppendSeparator();
  MenuAusschnitt = new wxMenuItem(BearbeitenMenu, ID_MAINWIN_AUSSCHNITT, _("Bild &zuschneiden"), _(""));
  BearbeitenMenu->Append(MenuAusschnitt);

  MenuBar->Append(BearbeitenMenu, _("&Bearbeiten"));

  //InfoMenu
  MenuLizenz = new wxMenuItem(InfoMenu, ID_MAINWIN_INFO, _("Lizenz&info"), _(""));
  InfoMenu->Append(MenuLizenz);

  MenuBar->Append(InfoMenu, _("&Info"));

  SetMenuBar(MenuBar);

  try
  {
      DasBild = new RotCyanAnaglyphe();
  }
  catch(wxString FehlerText)
  {
      wxMessageDialog(this, FehlerText, _("Probleme Probleme")).ShowModal();
  }

  /*Schwarz-Weiss-Regler*/
  SW_Slider = new SliderFrame(this, ID_CHILD_SWSLIDER, _("Anzahl der Grautoene: ")
              + wxString::Format(_("%i"), int(DasBild->HoleSWSchwellen())), wxPoint(0, 0), wxSize(30, 522), wxCAPTION);
  SW_Slider->Hide();

/*Datei - Auswahl*/
  PictureOpener = new wxFileDialog(this, _("Bild auswaehlen"),_(""),_(""),_("*.jpg"),1);
  PictureSaver = new wxFileDialog(this, _("Anaglyphe speichern"),_(""),_(""),_("*.png"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

  CreateStatusBar(2);
  SetStatusText(_("aru-soft - Anaglyph Generator"));
  ModusID = ID_MODUS_LEERLAUF;

  panel = new MyPanel(this);

  Maximize(true);
  /*Icon laden*/
  SetIcon(wxIcon(_("Bilder/3DGenerator.ico"), wxBITMAP_TYPE_ICO));
  Cursor_Malen();
  Refresh();
}

MainFrame::~MainFrame()
{
    delete DasBild;
    PictureOpener->Destroy();
    PictureSaver->Destroy();
    SW_Slider->Destroy();
    logSchreiben("Logbuch geschlossen");
    delete Logbuch;
    delete panel;
}

void MainFrame::Cursor_Malen(void)
{
	static char bullseye_bits[]={
   (char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,
   (char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0x3F,
   (char)0xFE,(char)0xFF,(char)0xFF,(char)0x7,(char)0xFE,(char)0xFF,(char)0xFF,(char)0xC3,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xF1,(char)0xFF,
   (char)0xFF,(char)0xFF,(char)0xF8,(char)0xFF,(char)0xFF,(char)0x7F,(char)0xFC,(char)0xFF,(char)0xFF,(char)0x7F,(char)0xFE,(char)0xFF,(char)0xF3,
   (char)0x7F,(char)0xFE,(char)0xFF,(char)0xE1,(char)0x3F,(char)0xFF,(char)0xFF,(char)0xE1,(char)0x3F,(char)0xFF,(char)0xFF,(char)0xC0,(char)0x3F,
   (char)0xFF,(char)0x7D,(char)0x92,(char)0x3F,(char)0xFF,(char)0x7F,(char)0x92,(char)0x3F,(char)0xFF,(char)0xFF,(char)0xF3,(char)0x3F,(char)0xFF,
   (char)0xFF,(char)0xF3,(char)0x7F,(char)0xFE,(char)0xFF,(char)0xF9,(char)0x7F,(char)0xFE,(char)0xFF,(char)0xF9,(char)0x7F,(char)0xFC,(char)0xFF,
   (char)0xF8,(char)0xFF,(char)0xF8,(char)0x7F,(char)0xFC,(char)0xFF,(char)0xF1,(char)0x3F,(char)0xFE,(char)0xFF,(char)0xC3,(char)0xF,(char)0xFF,
   (char)0xFF,(char)0x7,(char)0x80,(char)0xFF,(char)0xFF,(char)0x3F,(char)0xF0,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,
   (char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF };

	static char bullseye_mask[]={
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF,
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0x3F, (char)0xFE, (char)0xFF,
    (char)0xFF, (char)0x7, (char)0xFC, (char)0xFF, (char)0xFF, (char)0x3, (char)0xFC, (char)0xFF, (char)0xFF, (char)0x1, (char)0xFE, (char)0xFF,
    (char)0xFF, (char)0xC0, (char)0xFF, (char)0xFF, (char)0x7F, (char)0xF0, (char)0xFF, (char)0xFF, (char)0x3F, (char)0xF8, (char)0xFF, (char)0xF3,
    (char)0x3F, (char)0xFC, (char)0xFF, (char)0xE1, (char)0x3F, (char)0xFC, (char)0xFF, (char)0xC0, (char)0x1F, (char)0xFE, (char)0xFF, (char)0xC0,
    (char)0x1F, (char)0xFE, (char)0x7D, (char)0x80, (char)0x1F, (char)0xFE, (char)0x38, (char)0x0, (char)0x1F, (char)0xFE, (char)0x3D, (char)0x0,
    (char)0x1F, (char)0xFE, (char)0x7F, (char)0x80, (char)0x1F, (char)0xFE, (char)0xFF, (char)0xE1, (char)0x3F, (char)0xFC, (char)0xFF, (char)0xF0,
    (char)0x3F, (char)0xFC, (char)0xFF, (char)0xF0, (char)0x3F, (char)0xF8, (char)0x7F, (char)0xF0, (char)0x7F, (char)0xF0, (char)0x3F, (char)0xF8,
    (char)0xFF, (char)0xC0, (char)0xF, (char)0xFC, (char)0xFF, (char)0x1, (char)0x0, (char)0xFE, (char)0xFF, (char)0x3, (char)0x0, (char)0xFF,
    (char)0xFF, (char)0x7, (char)0x80, (char)0xFF, (char)0xFF, (char)0x3F, (char)0xF0, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF,
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF };

	wxBitmap bullseye_bitmap(bullseye_bits, 32, 32);
	wxBitmap bullseye_mask_bitmap(bullseye_mask, 32, 32);

	bullseye_bitmap.SetMask(new wxMask(bullseye_mask_bitmap));
	static wxImage bullseye_image = bullseye_bitmap.ConvertToImage();
	bullseye_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 17);
	bullseye_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 16);

	Bullseye_Cursor = wxCursor(bullseye_image);
	//Bullseye_Cursor = wxCursor(bullseye_bits,32,32,-1,-1,bullseye_mask);
	return;
}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MainFrame::OnSavePic(wxCommandEvent& event)
{
    int Rueck = PictureSaver->ShowModal();
    if(Rueck == wxID_CANCEL)return;

    wxCursor Temp = GetCursor();
    DerCursor = wxCURSOR_WAIT;
    SetCursor(DerCursor);
    Refresh();

    DasBild->SpeicherAnaglyphe(PictureSaver->GetPath());

    DerCursor = Temp;
    SetCursor(DerCursor);
    Refresh();
    return;
}

void MainFrame::OnOpenLeftPic(wxCommandEvent& event)
{
    PictureOpener->SetMessage(_("Linkes Bild oeffnen"));
    int Rueckgabe = PictureOpener->ShowModal();

    if(Rueckgabe==wxID_CANCEL)return;

	SetStatusText(PictureOpener->GetPath());
    DasBild->LadeLinkesBild(PictureOpener->GetPath());
    wxClientDC dc(this);
    DasBild->AnzeigeAnpassen(dc.GetSize());

    Refresh();
    return;
}

void MainFrame::OnOpenRightPic(wxCommandEvent& event)
{
    PictureOpener->SetMessage(_("Rechtes Bild oeffnen"));
    int Rueckgabe = PictureOpener->ShowModal();

    if(Rueckgabe==wxID_CANCEL)return;

	SetStatusText(PictureOpener->GetPath());
    DasBild->LadeRechtesBild(PictureOpener->GetPath());
    wxClientDC dc(this);
    DasBild->AnzeigeAnpassen(dc.GetSize());

    Refresh();
    return;
}
void MainFrame::OnTauscheSeiten(wxCommandEvent& event)
{
    ModusUmschalten(ID_MODUS_LEERLAUF);
    DasBild->TauscheLinksRechts();
    Refresh();
    return;
}

void MainFrame::OnEraseBackground(wxEraseEvent& event)
{
    return;
}

void MainFrame::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);

    /*Hintergrund zeichnen*/
    wxRect rect(wxPoint(0, 0), GetClientSize());
    wxColour back = GetBackgroundColour();
    dc.SetBrush(wxBrush(back));
    dc.SetPen(wxPen(back, 1));
    dc.DrawRectangle(rect);

    try
    {
        DasBild->Draw3DBild(dc);
    }
    catch(wxString FehlerText)
    {
        logSchreiben("Exeptionhandling Draw3DBild\n");
        //wxMessageDialog(this, FehlerText, _("Probleme Probleme")).ShowModal;
    }

    /*Drehpunkt zeichnen*/
    if(ModusID == ID_MODUS_DREHEN)
    {
        wxColour Kreuz(0, 0, 0);
        dc.SetBrush(wxBrush(Kreuz));
        dc.SetPen(wxPen(Kreuz, 2));
        dc.DrawLine(wxCoord (DrehPunkt.x-5), wxCoord (DrehPunkt.y), wxCoord (DrehPunkt.x+4), wxCoord (DrehPunkt.y));
        dc.DrawLine(wxCoord (DrehPunkt.x), wxCoord (DrehPunkt.y-5), wxCoord (DrehPunkt.x), wxCoord (DrehPunkt.y+4));
    }
    if(ARU_DEBUG)
    {
        wxFont TextFont = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(TextFont);
        wxColour TextColour = wxColour(255, 255, 255);
        dc.SetTextForeground(TextColour);
        TextColour = wxColour(0,0,0);
        dc.SetTextBackground(TextColour);

        int Zeilen = 0;
        Zeilen = DasBild->DebugInfo(dc, Zeilen);
        Zeilen++;
        /*dc.DrawText(_("Mausposition auf dem Schirm: \t") + wxString::Format(_("%i"), AlteMousePosition.x) + _(", ") + wxString::Format(_("%i"), AlteMousePosition.y), 10, Zeilen*10);*/
    }
    return;
}

void MainFrame::OnSize(wxSizeEvent& event)
{
    static bool ersterZugriff = true;
    if (ersterZugriff)
    {
        ersterZugriff = false;
        return;
    }

    wxClientDC dc(this);
    try
    {
        DasBild->AnzeigeAnpassen(dc.GetSize());
    }
    catch(wxString FehlerText)
    {
        SetStatusText(FehlerText);
    }

    wxRect SliderPlatz = wxRect(wxPoint(0, 0), wxSize(522, 30));
    SW_Slider->SetSize(SliderPlatz);

    Refresh();
    return;
}

void MainFrame::OnLeftDown(wxMouseEvent& event)
{
    wxClientDC dc(this);
    if((ModusID == ID_MODUS_VERSCHIEBEN)||(ModusID == ID_MODUS_DREHEN))
    {
        AlteMousePosition = event.GetLogicalPosition(dc);
        event.Skip();
        return;
    }
    if(DasBild->Zuschneiden())
    {
        AlteMousePosition = event.GetLogicalPosition(dc);
        DasBild->SetZuschnittStatus(AlteMousePosition);
        event.Skip();
        return;
    }
    event.Skip();
    return;
}

void MainFrame::OnMouseMove(wxMouseEvent& event)
{
    /*if(MausAussetzen)
    {
        AlteMousePosition = NeueMousePosition;
        event.Skip();
        return;
    }*/

	if(ModusID ==ID_MODUS_DREHEN)
	{
		event.Skip();
		return;
	}

    //MausAussetzen = TRUE;

    if(ModusID == ID_MODUS_VERSCHIEBEN)
    {
        if(event.Dragging() && event.LeftIsDown())
        {
            //wxMessageDialog(this, _("Maus bewegt sich"), _("in Verschieben")).ShowModal();
            wxClientDC dc(this);
            NeueMousePosition = event.GetLogicalPosition(dc);
            wxPoint Differenz = (NeueMousePosition-AlteMousePosition);
            DasBild->Verschieben(Differenz.x, Differenz.y);
            AlteMousePosition = NeueMousePosition;
            Refresh();
        }
        event.Skip();
        MausAussetzen = FALSE;
        return;
    }
    if(DasBild->Zuschneiden())
    {
        wxClientDC dc(this);
        DerCursor = DasBild->MausStellung(event.GetLogicalPosition(dc));
        SetCursor(DerCursor);
        if(event.Dragging() && event.LeftIsDown())
        {
            //wxMessageDialog(this, _("Maus bewegt sich"), _("in Zuschneiden")).ShowModal();
            NeueMousePosition = event.GetLogicalPosition(dc);
            DasBild->ZuschnittAendern(AlteMousePosition, NeueMousePosition);
            AlteMousePosition = NeueMousePosition;
            Refresh();
            event.Skip();
            MausAussetzen = FALSE;
            return;
        }
        event.Skip();
        MausAussetzen = FALSE;
        return;
    }

    MausAussetzen = FALSE;

    event.Skip();
    return;
}

void MainFrame::OnLeftUp(wxMouseEvent& event)
{
    if(ModusID == ID_MODUS_DREHEN)
    {
        wxClientDC dc(this);
        DrehPunkt = event.GetLogicalPosition(dc);
        DasBild->DrehungBeenden();
        DasBild->SetzeDrehpunkt(wxPoint(DrehPunkt.x-dc.GetSize().GetWidth()/2, DrehPunkt.y-dc.GetSize().GetHeight()/2));
        //wxMessageDialog(this, _("Drehpunkt: [") + wxString::Format(_("%i"),DrehPunkt.x-dc.GetSize().GetWidth()/2) + _("; ") + wxString::Format(_("%i"),DrehPunkt.y-dc.GetSize().GetHeight()/2) + _("]")).ShowModal();
    }
    Refresh();
    event.Skip();
    return;
}

void MainFrame::OnKey(wxKeyEvent &event)
{
    //wxMessageDialog(this, wxT("KeyEvent")).ShowModal();
    int Fktr;
    if(event.ShiftDown())
    {
        Fktr = 10;
    }else{
        Fktr = 1;
    }

    if(event.GetKeyCode()==WXK_F2)
    {
        /*LEER*/
    }

    if(event.GetKeyCode()==WXK_F3)
    {
        switch(ARU_DEBUG)//Debugmodus umschalten
        {
            case 0:
            ARU_DEBUG = 1;
            break;
            case 1:
            ARU_DEBUG = 0;
        }
    }

    if(event.GetKeyCode()==WXK_F5)
    {
        ProgrammModus neuerModus = (ModusID==ID_MODUS_VERSCHIEBEN) ? ID_MODUS_LEERLAUF : ID_MODUS_VERSCHIEBEN;
        ModusUmschalten(neuerModus);
    }

    if(event.GetKeyCode()==WXK_F6)
    {
        ProgrammModus neuerModus = (ModusID==ID_MODUS_DREHEN) ? ID_MODUS_LEERLAUF : ID_MODUS_DREHEN;
        ModusUmschalten(neuerModus);
    }

    wxClientDC dc(this);

    if(event.GetKeyCode()=='+' || event.GetKeyCode()==WXK_NUMPAD_ADD)
    {
        if(taste_RAND_unten==1)
        {
            DasBild->InkrRand(0.01*Fktr);

        }else
        if(taste_SW_unten==1)
        {
            DasBild->InkrSWSchwellen(1*Fktr);
        }else
        if(ModusID == ID_MODUS_DREHEN)
        {
            try{
                DrehPunkt = DasBild->Drehen(0.001*Fktr) + wxPoint((int)dc.GetSize().GetWidth()/2, (int)dc.GetSize().GetHeight()/2);
            }
            catch(wxString Text){
                wxMessageDialog(this, Text).ShowModal();
            }
        }
        Refresh();
    }
    if(event.GetKeyCode()=='-' || event.GetKeyCode()==WXK_NUMPAD_SUBTRACT)
    {
        if(taste_RAND_unten==1)
        {
            DasBild->InkrRand(-0.01*Fktr);
        }else
        if(taste_SW_unten==1)
        {
            DasBild->InkrSWSchwellen(-1*Fktr);
        }else
        if(ModusID == ID_MODUS_DREHEN)
        {
            try{
                DrehPunkt = DasBild->Drehen(-(0.001*Fktr)) + wxPoint((int)dc.GetSize().GetWidth()/2, (int)dc.GetSize().GetHeight()/2);
            }
            catch(wxString Text){
                wxMessageDialog(this, Text).ShowModal();
            }
        }
        Refresh();
    }

    if(event.GetKeyCode()=='r'||event.GetKeyCode()=='R')taste_RAND_unten = 1;
    if(event.GetKeyCode()=='s'||event.GetKeyCode()=='S')taste_SW_unten = 1;

    if(event.GetKeyCode()==WXK_LEFT)DasBild->Verschieben(-(Fktr), 0);
    if(event.GetKeyCode()==WXK_UP) DasBild->Verschieben(0, -(Fktr));
    if(event.GetKeyCode()==WXK_RIGHT) DasBild->Verschieben((Fktr), 0);
    if(event.GetKeyCode()==WXK_DOWN) DasBild->Verschieben(0, (Fktr));

    Refresh();
    return;
}

void MainFrame::OnKeyRelease(wxKeyEvent &event)
{
    if(event.GetKeyCode()=='r'||event.GetKeyCode()=='R')taste_RAND_unten = 0;
    if(event.GetKeyCode()=='s'||event.GetKeyCode()=='S')taste_SW_unten = 0;
}

void MainFrame::OnGraustufen(wxCommandEvent& event)
{
    UnCheckAll();
    if(event.IsChecked())
    {
        DasBild->Effekt(ID_3DBILD_GRAU);
        MenuPunktGrau->Check(true);
    }else{
        DasBild->Effekt(ID_3DBILD_NORMAL);
	}
    Refresh();
}

void MainFrame::OnSchwarzWeiss(wxCommandEvent& event)
{
    UnCheckAll();
    if(event.IsChecked())
    {
        DasBild->Effekt(ID_3DBILD_SW);
        MenuPunktSW->Check(true);
        //SW_Slider->SetValue((int)(DasBild->HoleSWSchwellen()));
        //SW_Slider->Show();
    }else{
        DasBild->Effekt(ID_3DBILD_NORMAL);
	}
    Refresh();
}

void MainFrame::OnWechselAktivesBild(wxCommandEvent& event)
{
    if(ModusID == ID_MODUS_DREHEN)
    {
        MenuDrehen->SetItemLabel(_("Teilbild &drehen"));
        ModusUmschalten(ID_MODUS_LEERLAUF);
        MenuVerschieben->Enable(true);
    }
    if(ModusID == ID_MODUS_VERSCHIEBEN)
    {
        MenuVerschieben->SetItemLabel(_("Teilbild &verschieben"));
        ModusUmschalten(ID_MODUS_LEERLAUF);
        MenuDrehen->Enable(true);
    }
    if(DasBild->IstLinksAktiv())
    {
        DasBild->AktiviereRechtesBild();
        MenuWechselBild->SetItemLabel(_("&Linkes Bild bearbeiten (rechts aktiv)"));
    }
    else
    {
        DasBild->AktiviereLinkesBild();
        MenuWechselBild->SetItemLabel(_("&Rechtes Bild bearbeiten (links aktiv)"));
    }
}

void MainFrame::OnBrilleWechseln(wxCommandEvent& event)
{
    if(event.GetId()==ID_MAINWIN_RC_BRILLE)
    {
        Menu_MG_Brille->Check(false);
        Menu_AB_Brille->Check(false);
        MenuStereogramm->Check(false);
        if(event.IsChecked())
        {
            NeuAnlegen(0);
        }else{
            Menu_RC_Brille->Check(true);
        }
    }
    if(event.GetId()==ID_MAINWIN_MG_BRILLE)
    {
        Menu_RC_Brille->Check(false);
        Menu_AB_Brille->Check(false);
        MenuStereogramm->Check(false);
        if(event.IsChecked())
        {
            NeuAnlegen(1);
        }else{
            Menu_MG_Brille->Check(true);
        }
    }
    if(event.GetId()==ID_MAINWIN_AB_BRILLE)
    {
        Menu_RC_Brille->Check(false);
        Menu_MG_Brille->Check(false);
        MenuStereogramm->Check(false);
        if(event.IsChecked())
        {
            NeuAnlegen(2);
        }else{
            Menu_AB_Brille->Check(true);
        }
    }
    if(event.GetId()==ID_MAINWIN_STEREO)
    {
        Menu_RC_Brille->Check(false);
        Menu_MG_Brille->Check(false);
        Menu_AB_Brille->Check(false);
        if(event.IsChecked())
        {
            NeuAnlegen(3);
        }else{
            MenuStereogramm->Check(true);
        }
    }
    Refresh();
    return;
}

void MainFrame::OnVerschieben(wxCommandEvent& event)
{
    ModusUmschalten(ModusID==ID_MODUS_VERSCHIEBEN ? ID_MODUS_LEERLAUF : ID_MODUS_VERSCHIEBEN);
    return;
}

void MainFrame::OnDrehen(wxCommandEvent& event)
{
    ModusUmschalten(ModusID==ID_MODUS_DREHEN ? ID_MODUS_LEERLAUF : ID_MODUS_DREHEN);
    return;
}

void MainFrame::OnBildAusschnitt(wxCommandEvent& event)
{
    if(DasBild->Zuschneiden())
    {
        DasBild->Zuschneiden(false);
        MenuAusschnitt->SetItemLabel(_("Bild &zuschneiden"));
    }else{
        DasBild->Zuschneiden(true);
        MenuAusschnitt->SetItemLabel(_("&Zuschnitt beenden"));
    }
    Refresh();
    return;
}


void MainFrame::ModusUmschalten(ProgrammModus NeuerModus)
{
    ModusID = NeuerModus;
    DasBild->DrehungBeenden();
    if(ModusID == ID_MODUS_LEERLAUF)
    {
        MenuDrehen->SetItemLabel(_("Teilbild &drehen"));
        MenuVerschieben->SetItemLabel(_("Teilbild &verschieben"));
        MenuAusschnitt->SetItemLabel(_("Bild &zuschneiden"));
        MenuVerschieben->Enable(true);
        MenuDrehen->Enable(true);
        DerCursor = wxCURSOR_ARROW;
		SetCursor(DerCursor);
        Refresh();
        return;
    }
    if(ModusID == ID_MODUS_DREHEN)
    {
        MenuDrehen->SetItemLabel(_("&Drehung abschliessen"));
        MenuVerschieben->SetItemLabel(_("Teilbild &verschieben"));
		SetCursor(Bullseye_Cursor);
		Refresh();
        return;
    }
    if(ModusID == ID_MODUS_VERSCHIEBEN)
    {
        MenuVerschieben->SetItemLabel(_("&Verschiebung abschliessen"));
        MenuDrehen->SetItemLabel(_("Teilbild &drehen"));
        DerCursor = wxCURSOR_ARROW;
		SetCursor(DerCursor);
		Refresh();
        return;
    }
    return;
}

void MainFrame::OnInfo(wxCommandEvent& event)
{
    LizenzAnzeige* DieLizenz = new LizenzAnzeige(this, _("Lizenzinfo"), wxPoint(400,50), wxSize(600,400) ,wxDEFAULT_FRAME_STYLE);
    DieLizenz->Show();
    return;
}

void MainFrame::UnCheckAll(void)
{
    MenuPunktGrau->Check(false);
    MenuPunktSW->Check(false);
    SW_Slider->Hide();
    return;
}

void MainFrame::OnSWSliderChange(unsigned char Wert)
{
	/*int modWert = (int)(exp(Wert*log(256)/255)-1);*/
	DasBild->SetzeSWSchwellen(Wert);
	Refresh();
	return;
}

unsigned char MainFrame::HoleSWSchwellen(void)
{
    return(DasBild->HoleSWSchwellen());
}

void MainFrame::NeuAnlegen(int typ)
{
    Bild3D* tempBild;

    switch(typ)
    {
    case 0:
        tempBild = new RotCyanAnaglyphe(*DasBild);
        break;
    case 1:
        tempBild = new MagentaGruenAnaglyphe(*DasBild);
        break;
    case 2:
        tempBild = new BernsteinBlauAnaglyphe(*DasBild);
        break;
    default:
        tempBild = new StereoBild(*DasBild);
        break;
    }
    delete DasBild;

    DasBild = tempBild;

    wxClientDC dc(this);
    DasBild->AnzeigeAnpassen(dc.GetSize());

    Refresh();
    return;
}

void MainFrame::logSchreiben(const char*msg)
{
    Logbuch->open("FensterLog.log",ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        *Logbuch<<msg;
        Logbuch->close();
    }
    return;
}

void MainFrame::logSchreiben(const float msg, int i)
{
    Logbuch->open("FensterLog.log",ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        Logbuch->setf( ios::fixed, ios::floatfield );
        Logbuch->precision(i);
        *Logbuch<<msg;
        Logbuch->close();
    }
    return;
}



/*Implementierung Lizenzanzeige*/

BEGIN_EVENT_TABLE(LizenzAnzeige, wxFrame)
    EVT_SIZE(LizenzAnzeige::OnSize)
END_EVENT_TABLE()

#include <fstream>
#include <cstring>
using namespace std;

LizenzAnzeige::LizenzAnzeige(wxWindow* parent, const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame(parent, -1, title, pos, size, style)
{
    CreateStatusBar(1);
    SetStatusText(_("GNU General Public License"));

    LizenzText = new wxTextCtrl(this, 1, _("") , wxPoint(0,0), GetClientSize(), wxTE_MULTILINE | wxTE_RICH | wxTE_READONLY);
    LizenzEinlesen();
}

LizenzAnzeige::~LizenzAnzeige()
{
    delete LizenzText;
}

void LizenzAnzeige::LizenzEinlesen()
{
    int ZeichenZaehler = 0;
    char zeile[101];
    ifstream datei("Lizenz.txt", ios::in);
    datei.exceptions(std::ifstream::failbit|std::ifstream::badbit);
    if (datei.good())
    {
        while(!datei.eof())
        {
            try{
                datei.getline(zeile, 101, ' ');
                LizenzText->AppendText(_(zeile));
                LizenzText->AppendText(_(" "));
                //if(zeile == '\n'){
                         ZeichenZaehler ++;//= (strlen(zeile)+1);
                //}
                Refresh();
            }
            catch(std::ifstream::failure &e){
                SetStatusText(wxString::Format(_("%i"), ZeichenZaehler) + _(" - Fehler beim Lesen der Lizenzdatei"));
            }
        }
        if (datei.eof()){
            //SetStatusText(wxString::Format(_("%i"), ZeichenZaehler) + _(" - Dateiende erreicht"));
        }

    } else {
        LizenzText->AppendText(_("Ein Fehler ist aufgetreten."));
    }
    //SetStatusText(wxString::Format(_("%i"), ZeichenZaehler));
    datei.close();
    LizenzText->SetSelection(0L, 0L);
    //LizenzText->SetInsertionPoint(0);
    Refresh();
}

void LizenzAnzeige::OnSize(wxSizeEvent& event)
{
    LizenzText->SetSize(GetClientSize());
    Refresh();

}

/*Implementierung SWSlider*/

BEGIN_EVENT_TABLE(SWSlider, wxSlider)
	EVT_SCROLL(SWSlider::OnSWSliderChange)
END_EVENT_TABLE()

SWSlider::SWSlider(SliderFrame* parent, wxWindowID id, int value , int minValue, int maxValue, const wxPoint& point, const wxSize& size)
	: wxSlider(parent, id, value, minValue, maxValue, point, size)
{
	Eltern = parent;
	SetTickFreq(5);
	return;
}

SWSlider::~SWSlider()
{
}

void SWSlider::OnSWSliderChange(wxScrollEvent& event)
{
	Eltern->OnSWSliderChange(GetValue());
	return;
}

/*Implementierung SliderFrame*/

BEGIN_EVENT_TABLE(SliderFrame, wxFrame)
    EVT_SIZE(SliderFrame::OnSize)
END_EVENT_TABLE()

SliderFrame::SliderFrame(MainFrame* parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    Eltern = parent;
    Slider = new SWSlider(this, id, (int)(Eltern->HoleSWSchwellen()), 1, 255, wxPoint(0, 0), wxSize(512, 20));
    return;
}

SliderFrame::~SliderFrame()
{
    delete Slider;
}

void SliderFrame::OnSWSliderChange(unsigned char Wert)
{
    Eltern->OnSWSliderChange(Wert);
    SetTitle(_("Anzahl der Grautoene: ") + wxString::Format(_("%i"), Wert));
    return;
}

void SliderFrame::OnSize(wxSizeEvent& event)
{
    static bool ersterZugriff = true;
    if (ersterZugriff)
    {
        ersterZugriff = false;
        return;
    }

    wxRect SliderPlatz = wxRect(wxPoint(0, 0), wxSize(512, 20));
    Slider->SetSize(SliderPlatz);

    this->SetSize(522, 50);

    return;
}

void SliderFrame::SetValue(int Wert)
{
    Slider->SetValue(Wert);
    return;
}
