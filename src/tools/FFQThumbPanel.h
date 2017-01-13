/*************************************************************************
*                                                                        *
*  Copyright (c) 2014 Torben Bruchhaus                                   *
*  http://ffqueue.bruchhaus.dk/                                          *
*  File: FFQThumbPanel.h                                                 *
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

#ifndef FFQTHUMBPANEL_H
#define FFQTHUMBPANEL_H

//#include "wx_pch.h"

#ifndef WX_PRECOMP
	//(*HeadersPCH(FFQThumbPanel)
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	#include <wx/checkbox.h>
	#include <wx/radiobut.h>
	#include <wx/panel.h>
	#include <wx/hyperlink.h>
	#include <wx/choice.h>
	#include <wx/combobox.h>
	//*)
#endif
//(*Headers(FFQThumbPanel)
//*)

#include "../FFQTimeEdit.h"
#include "../utils/FFQThumbs.h"
#include <wx/hyperlink.h>

class FFQThumbPanel: public wxPanel
{

	public:

		FFQThumbPanel(wxWindow* parent, wxWindowID id=wxID_ANY);
		virtual ~FFQThumbPanel();
		void ClearValues(bool load_config);
		bool GetValues(THUMBS_AND_TILES &tnt, bool ThumbOrTileRequired);
		void SetValues(THUMBS_AND_TILES &tnt);
		void UpdateControls();

		//(*Declarations(FFQThumbPanel)
		wxFlexGridSizer* WidthSizer;
		wxFlexGridSizer* TimeSizer;
		wxHyperlinkCtrl* StartTime;
		wxRadioButton* EveryCustom;
		wxChoice* ImgDest;
		wxTextCtrl* Columns;
		wxRadioButton* EveryScene;
		wxFlexGridSizer* MosaicSizer;
		wxFlexGridSizer* AmountSizer;
		wxFlexGridSizer* FreqSizer;
		wxStaticText* ST6;
		wxChoice* CustFreqType;
		wxStaticText* ST8;
		wxStaticText* ST5;
		wxStaticText* ST2;
		wxCheckBox* MakeThumbs;
		wxStaticText* ST4;
		wxTextCtrl* Rows;
		wxComboBox* ImgFormat;
		wxStaticText* ST3;
		wxStaticText* ST1;
		wxFlexGridSizer* DestSizer;
		wxTextCtrl* Width;
		wxTextCtrl* CustFreq;
		wxCheckBox* MakeTiles;
		wxTextCtrl* MaxThumbs;
		wxTextCtrl* MaxTiles;
		wxStaticText* ST7;
		wxFlexGridSizer* FGS_Main;
		//*)

	protected:

		//(*Identifiers(FFQThumbPanel)
		static const long ID_MAKETHUMBS;
		static const long ID_MAKETILES;
		static const long ID_ST1;
		static const long ID_IMGDEST;
		static const long ID_ST2;
		static const long ID_IMGFORMAT;
		static const long ID_ST3;
		static const long ID_WIDTH;
		static const long ID_ST4;
		static const long ID_STARTTIME;
		static const long ID_EVERYCUSTOM;
		static const long ID_CUSTFREQ;
		static const long ID_CUSTFREQTYPE;
		static const long ID_EVERYSCENE;
		static const long ID_ST5;
		static const long ID_MAXTHUMBS;
		static const long ID_ST6;
		static const long ID_MAXTILES;
		static const long ID_ST7;
		static const long ID_ROWS;
		static const long ID_ST8;
		static const long ID_COLUMNS;
		//*)

	private:

		FFQTimeEdit *m_TimeEdit;
		TIME_VALUE m_StartTime;

		bool EditTime(TIME_VALUE &tv);

		//(*Handlers(FFQThumbPanel)
		void OnAction(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif // FFQTHUMBPANEL_H
