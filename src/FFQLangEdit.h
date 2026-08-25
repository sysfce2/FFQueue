/*************************************************************************
*                                                                        *
*  Copyright (c) 2014 Torben Bruchhaus                                   *
*  http://ffqueue.bruchhaus.dk/                                          *
*  File: FFQLangEdit.h                                                   *
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

#ifndef FFQLANGEDIT_H
#define FFQLANGEDIT_H

//#include "wx_pch.h"

#ifndef WX_PRECOMP
	//(*HeadersPCH(FFQLangEdit)
	#include <wx/button.h>
	#include <wx/filedlg.h>
	#include <wx/frame.h>
	#include <wx/listctrl.h>
	#include <wx/panel.h>
	#include <wx/radiobut.h>
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/statusbr.h>
	#include <wx/textctrl.h>
	#include <wx/timer.h>
	//*)
#endif
//(*Headers(FFQLangEdit)
#include <wx/statline.h>
//*)

#include "utils/FFQLang.h"
#include "utils/FFQProcess.h"
#include "utils/FFQParsing.h"
#include "FFQProgressDlg.h"
#include <wx/listctrl.h>

//const wxString DUMMY_PASSWORD = "\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02";

enum EXT_CMD_RESULT {ecrNone, ecrError, ecrTimeout};

class FFQLangEdit: public wxFrame, public FFQProgressDlgReceiver
{
	public:

		static FFQLangEdit* Get(wxWindow *parent);
		static void Release();

		FFQLangEdit(wxWindow* parent);
		virtual ~FFQLangEdit();
		void Execute();
		void LoadList();
		void ProgressAbort();
		bool ProgressStep(unsigned int step);
        void SetListItem(long idx, LPFFQ_STRING ffqs);


		//(*Declarations(FFQLangEdit)
		wxButton* ExpFile;
		wxButton* ExtCmdBtn;
		wxButton* ImpFile;
		wxFileDialog* OpenFile;
		wxFileDialog* SaveFile;
		wxFlexGridSizer* MainSizer;
		wxListView* ListView;
		wxPanel* TabPanel;
		wxRadioButton* ExpInternal;
		wxRadioButton* ExpNew;
		wxStaticLine* StaticLine1;
		wxStaticText* Info;
		wxStaticText* ST2;
		wxStaticText* ST3;
		wxStaticText* ST4;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStatusBar* SB;
		wxTextCtrl* Description;
		wxTextCtrl* ExtCmd;
		wxTextCtrl* FilterText;
		wxTextCtrl* Password1;
		wxTextCtrl* Password2;
		wxTextCtrl* StrEdit;
		wxTimer Timer;
		//*)

	protected:

		//(*Identifiers(FFQLangEdit)
		static const long ID_ST2;
		static const long ID_DESCRIPTION;
		static const long ID_ST3;
		static const long ID_PASSWORD1;
		static const long ID_ST4;
		static const long ID_PASSWORD2;
		static const long ID_LISTVIEW;
		static const long ID_STATICTEXT1;
		static const long ID_FILTERTEXT;
		static const long ID_STREDIT;
		static const long ID_INFO;
		static const long ID_EXTCMD;
		static const long ID_EXTCMDBTN;
		static const long ID_STATICLINE1;
		static const long ID_STATICTEXT2;
		static const long ID_EXPINTERNAL;
		static const long ID_EXPNEW;
		static const long ID_EXPFILE;
		static const long ID_STATICTEXT3;
		static const long ID_IMPFILE;
		static const long ID_TABPANEL;
		static const long ID_SB;
		static const long ID_FILTERTIMER;
		//*)

		static FFQLangEdit *m_Instance;

	private:

		//(*Handlers(FFQLangEdit)
		void OnClose(wxCloseEvent& event);
		void OnListViewItemSelect(wxListEvent& event);
		void OnFilterTimer(wxTimerEvent& event);
		void OnAction(wxCommandEvent& event);
		//*)

		FFQProcess *m_Process;
		FFQProgressDlg *m_ProgressDlg;
		wxString m_Password, m_Filter, m_ExpImpFile, m_ExtCmd, m_ExtValue;
		FFQLang *m_EditLang, *m_OrgLang;
		LPFFQ_STRING m_EditStr, m_OrgStr;
		long m_EditIndex, m_Modified;
		//short m_ExtTranslate;
		EXT_CMD_RESULT m_ExtCmdRes;
		bool m_SkipEvents;
		unsigned int m_EditListSize;

		void BindKeyEvents(wxWindow *wnd);

		void ExportTranslationFile();//wxString filename = wxEmptyString);
		void ImportTranslationFile();//wxString filename = wxEmptyString);

		bool ExternalTranslate(wxString *value = nullptr);
		//bool ExternalTranslate(wxString &val);

		void LoadEditStr(bool internal = false);
		bool SaveEditStr(bool force, bool clear = true);
		bool SavePwdAndDesc();
		void SetEditStr(wxString str, unsigned int array_size = 0);
		bool SkipItemChange(wxString reason);

		void UpdateStatus();

		void OnIdle(wxIdleEvent &event);
		void OnKeyDown(wxKeyEvent &event);

		DECLARE_EVENT_TABLE()
};

#endif // FFQLANGEDIT_H
