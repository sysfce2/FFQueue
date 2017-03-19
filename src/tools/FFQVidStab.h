/*************************************************************************
*                                                                        *
*  Copyright (c) 2014 Torben Bruchhaus                                   *
*  http://ffqueue.bruchhaus.dk/                                          *
*  File: FFQVidStab.h                                                    *
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

#ifndef FFQVIDSTAB_H
#define FFQVIDSTAB_H

//#include "wx_pch.h"

#ifndef WX_PRECOMP
	//(*HeadersPCH(FFQVidStab)
	#include <wx/checkbox.h>
	#include <wx/dialog.h>
	#include <wx/sizer.h>
	#include <wx/button.h>
	#include <wx/filedlg.h>
	#include <wx/panel.h>
	#include <wx/slider.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	#include <wx/choice.h>
	//*)
#endif
//(*Headers(FFQVidStab)
#include <wx/spinctrl.h>
#include "../FFQPresetPanel.h"
#include <wx/notebook.h>
//*)

#include "../utils/FFQProbing.h"
#include "../utils/FFQToolJobs.h"

class FFQVidStab: public wxDialog
{
	public:

		FFQVidStab(wxWindow* parent);
		virtual ~FFQVidStab();
		bool Execute(LPFFQ_VIDSTAB_JOB job);

		//(*Declarations(FFQVidStab)
		wxButton* CancelButton;
		wxChoice* OptAlgo;
		wxStaticText* ZoomSpeedText;
		wxCheckBox* KeepTransforms;
		wxSlider* StepSize;
		wxFileDialog* OpenFileDlg;
		wxButton* DefaultsButton;
		wxNotebook* Pages;
		wxCheckBox* Verbose;
		wxSpinCtrl* MaxAngle;
		wxStaticText* ST5;
		wxPanel* Page1;
		wxChoice* OptZoom;
		wxSlider* MinContrast;
		wxCheckBox* SaveLog;
		wxCheckBox* Only2nd;
		wxPanel* Page2;
		wxCheckBox* CropBlack;
		wxTextCtrl* DestFile;
		FFQPresetPanel* Preset;
		wxCheckBox* Only1st;
		wxPanel* Page0;
		wxStaticText* ShakinessText;
		wxTextCtrl* SrcFile;
		wxStaticText* ST10;
		wxButton* OkButton;
		wxSpinCtrl* ZoomSpeed;
		wxStaticText* StepSizeText;
		wxStaticText* FileInfo;
		wxSpinCtrl* Smoothing;
		wxCheckBox* Relative;
		wxCheckBox* Invert;
		wxSlider* Shakiness;
		wxStaticText* MinContrastText;
		wxCheckBox* VirtualTripod;
		wxButton* BrowseDest;
		wxStaticText* AccuracyText;
		wxChoice* Interpolation;
		wxFlexGridSizer* Sizer3;
		wxSpinCtrl* Zoom;
		wxSpinCtrl* MaxShift;
		wxSpinCtrl* TripodFrame;
		wxButton* BrowseSrc;
		wxFileDialog* SaveFileDlg;
		wxSlider* Accuracy;
		//*)

	protected:

		//(*Identifiers(FFQVidStab)
		static const long ID_SRCFILE;
		static const long ID_BROWSESRC;
		static const long ID_FILEINFO;
		static const long ID_SHAKINESS;
		static const long ID_ACCURACY;
		static const long ID_STEPSIZE;
		static const long ID_MINCONTRAST;
		static const long ID_TRIPODFRAME;
		static const long ID_PAGE0;
		static const long ID_SMOOTHING;
		static const long ID_MAXSHIFT;
		static const long ID_MAXANGLE;
		static const long ID_ZOOM;
		static const long ID_ZOOMSPEED;
		static const long ID_OPTALGO;
		static const long ID_INTERPOLATION;
		static const long ID_OPTZOOM;
		static const long ID_CROPBLACK;
		static const long ID_INVERT;
		static const long ID_RELATIVE;
		static const long ID_VIRTUALTRIPOD;
		static const long ID_PAGE1;
		static const long ID_ONLY1ST;
		static const long ID_VERBOSE;
		static const long ID_ONLY2ND;
		static const long ID_KEEPTRANSFORMS;
		static const long ID_PAGE2;
		static const long ID_PAGES;
		static const long ID_DESTFILE;
		static const long ID_BROWSEDEST;
		static const long ID_PRESET;
		static const long ID_SAVELOG;
		static const long ID_DEFAULTSBUTTON;
		static const long ID_OKBUTTON;
		static const long ID_CANCELBUTTON;
		//*)

	private:

		wxString /*m_Command, m_SaveLogFor,*/ m_LastSrc, m_StreamMap, m_LastPreset/*, m_TempFile*/;
		TIME_VALUE m_Duration;
		bool m_SrcValid;
		LPFFQ_VIDSTAB_JOB m_EditJob;
        FFProbeInfoParser m_PIP;

		//void ClearTempFile();
		void DefaultSettings(wxString load_cfg = "");
		//bool MakeCommands();
		void SetValuesFrom(LPFFQ_VIDSTAB_JOB job);
		void UpdateControls(bool SliderLabels = false);

        void OnIdle(wxIdleEvent &event);


		//(*Handlers(FFQVidStab)
		void OnSliderChange(wxScrollEvent& event);
		void OnAction(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif // FFQVIDSTAB_H
