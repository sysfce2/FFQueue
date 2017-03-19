/*************************************************************************
*                                                                        *
*  Copyright (c) 2014 Torben Bruchhaus                                   *
*  http://ffqueue.bruchhaus.dk/                                          *
*  File: FFQJobEditAdvCuts.h                                             *
*                                                                        *
*  This file is part of FFQueue.                                         *
*                                                                        *
*  FFQueue is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
*  FFQueue is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*  GNU General Public License for more details.                          *
*                                                                        *
*  You should have received a copy of the GNU General Public License     *
*  along with FFQueue.  If not, see <http://www.gnu.org/licenses/>.      *
*                                                                        *
*************************************************************************/

#ifndef FFQJOBEDITADVCUTS_H
#define FFQJOBEDITADVCUTS_H

//#include "wx_pch.h"

#ifndef WX_PRECOMP
	//(*HeadersPCH(FFQJobEditAdvCuts)
	#include <wx/checkbox.h>
	#include <wx/dialog.h>
	#include <wx/sizer.h>
	#include <wx/button.h>
	#include <wx/radiobut.h>
	#include <wx/panel.h>
	#include <wx/slider.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	#include <wx/timer.h>
	#include <wx/listbox.h>
	//*)
#endif
//(*Headers(FFQJobEditAdvCuts)
//*)

#include "utils/FFQLang.h"
#include "utils/FFQQueueItem.h"
#include "utils/FFQProbing.h"

const wxString CUT_SEP = " => ";
const wxString ADV_CUTS_REQUIRED_FILTERS = "trim,atrim,setpts,asetpts,concat";

class FFQJobEditAdvCuts: public wxDialog
{
	public:

		FFQJobEditAdvCuts(wxWindow* parent);
		virtual ~FFQJobEditAdvCuts();
		bool Execute(FFQ_CUTS &cuts, wxString file_path, FFProbeInfoParser *probe);

		//(*Declarations(FFQJobEditAdvCuts)
		wxSlider* CfgAccuracy;
		wxButton* CancelButton;
		wxListBox* CutList;
		wxStaticText* ST2;
		wxStaticText* ST11;
		wxStaticText* ST7;
		wxButton* SetTo;
		wxTextCtrl* ToTime;
		wxButton* AddCut;
		wxFlexGridSizer* PreviewSizer2;
		wxStaticText* ST8;
		wxRadioButton* FilterFirst;
		wxStaticText* ST5;
		wxRadioButton* FilterLast;
		wxSlider* CfgDelay;
		wxCheckBox* FrameConv;
		wxPanel* FrameView;
		wxTextCtrl* TimePreviewAt;
		wxStaticText* ST4;
		wxFlexGridSizer* PreviewSizer1;
		wxSlider* FrameSlider;
		wxButton* NextFrame;
		wxStaticText* FramePos;
		wxTextCtrl* FromTime;
		wxButton* SetFrom;
		wxTimer FrameTimer;
		wxStaticText* ST10;
		wxButton* OkButton;
		wxButton* PrevFrame;
		wxButton* RemoveCut;
		wxStaticText* ST3;
		wxStaticText* ST9;
		wxRadioButton* RemoveCuts;
		wxRadioButton* KeepCuts;
		wxButton* TimePreview;
		wxStaticText* DurationLab;
		wxStaticText* ST6;
		//*)

	protected:

		//(*Identifiers(FFQJobEditAdvCuts)
		static const long ID_CUTLIST;
		static const long ID_FILTERFIRST;
		static const long ID_FILTERLAST;
		static const long ID_FROMTIME;
		static const long ID_TOTIME;
		static const long ID_ADDCUT;
		static const long ID_REMOVECUT;
		static const long ID_FRAMECONV;
		static const long ID_TIMEPREVIEWAT;
		static const long ID_TIMEPREVIEW;
		static const long ID_FRAMESLIDER;
		static const long ID_PREVFRAME;
		static const long ID_NEXTFRAME;
		static const long ID_SETFROM;
		static const long ID_SETTO;
		static const long ID_OKBUTTON;
		static const long ID_CANCELBUTTON;
		static const long ID_FRAMETIMER;
		//*)

	private:

		//(*Handlers(FFQJobEditAdvCuts)
		void ActionClick(wxCommandEvent& event);
		void OnCutListDClick(wxCommandEvent& event);
		void OnFrameViewPaint(wxPaintEvent& event);
		void OnFrameTimerTrigger(wxTimerEvent& event);
		//*)
		TIME_VALUE m_Duration, m_FramePos;
		wxString m_FilePath, m_SrtPath;
		wxImage *m_CurFrame;
		wxBitmap *m_DrawBuf;
		FFQProcess *m_Process;
		unsigned int m_FrameTime;
		FFProbeInfoParser *m_Probe;
		LPFFPROBE_STREAM_INFO m_VidInfo;
		double m_VidRate;

		void ExtractFrame();
		bool GetCut(wxString &cut, long &pos);
		void* MakePreviewJob(TIME_VALUE start_time, FFQ_CUTS cuts, bool encode);
		void ShowTimePreview(TIME_VALUE &at_time);
		void UpdateControls();

		DECLARE_EVENT_TABLE()

};

#endif // FFQJOBEDITADVCUTS_H
