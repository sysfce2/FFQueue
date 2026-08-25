/*************************************************************************
*                                                                        *
*  Copyright (c) 2014 Torben Bruchhaus                                   *
*  http://ffqueue.bruchhaus.dk/                                          *
*  File: FFQLangEdit.cpp                                                 *
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

#include "FFQLangEdit.h"
#include <wx/file.h>
#include "utils/FFQMisc.h"
#include "utils/FFQConst.h"
#include "utils/FFQConfig.h"
#include "utils/FFQHash.h"
#include "utils/FFQParsing.h"
#include "utils/FFQConsole.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(FFQLangEdit)
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(FFQLangEdit)
//*)


//(*IdInit(FFQLangEdit)
const long FFQLangEdit::ID_ST2 = wxNewId();
const long FFQLangEdit::ID_DESCRIPTION = wxNewId();
const long FFQLangEdit::ID_ST3 = wxNewId();
const long FFQLangEdit::ID_PASSWORD1 = wxNewId();
const long FFQLangEdit::ID_ST4 = wxNewId();
const long FFQLangEdit::ID_PASSWORD2 = wxNewId();
const long FFQLangEdit::ID_LISTVIEW = wxNewId();
const long FFQLangEdit::ID_STATICTEXT1 = wxNewId();
const long FFQLangEdit::ID_FILTERTEXT = wxNewId();
const long FFQLangEdit::ID_STREDIT = wxNewId();
const long FFQLangEdit::ID_INFO = wxNewId();
const long FFQLangEdit::ID_EXTCMD = wxNewId();
const long FFQLangEdit::ID_EXTCMDBTN = wxNewId();
const long FFQLangEdit::ID_STATICLINE1 = wxNewId();
const long FFQLangEdit::ID_STATICTEXT2 = wxNewId();
const long FFQLangEdit::ID_EXPINTERNAL = wxNewId();
const long FFQLangEdit::ID_EXPNEW = wxNewId();
const long FFQLangEdit::ID_EXPFILE = wxNewId();
const long FFQLangEdit::ID_STATICTEXT3 = wxNewId();
const long FFQLangEdit::ID_IMPFILE = wxNewId();
const long FFQLangEdit::ID_TABPANEL = wxNewId();
const long FFQLangEdit::ID_SB = wxNewId();
const long FFQLangEdit::ID_FILTERTIMER = wxNewId();
//*)

BEGIN_EVENT_TABLE(FFQLangEdit,wxFrame)
	//(*EventTable(FFQLangEdit)
	//*)
END_EVENT_TABLE()

//---------------------------------------------------------------------------------------

const wxString DUMMY_PASSWORD = "\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02\t\x02";

//---------------------------------------------------------------------------------------

const wxString STRING_STATUS[5] = { "New", "Internal", "Translated", "Modified", "External" };

//---------------------------------------------------------------------------------------

const wxString COMMAND_REPLACE = "$$";
const unsigned int COMMAND_TIMEOUT = 5000; // 30 sec
const unsigned int FILTER_TIMEOUT = 750;
const unsigned int TRANSLATE_INTERVAL = 500;

const wxString FILEDLG_IDENT = "langedit";
//const wxString DEFAULT_TRANS_FILENAME = "translate.txt";

const wxString STR_ILLEGAL_CHAR     = "Illegal character \"|\" in string %i";
const wxString STR_BAD_STRING_ID    = "Bad string ID %i";
const wxString STR_EMPTY_STRING     = "Empty string for %i";
const wxString STR_BAD_HASH         = "String hash mismatch for %i";
const wxString STR_BAD_ARRAY_SIZE   = "Array size mismatch (%u != %u) in string %i";
const wxString STR_BAD_FORMAT_SPEC  = "Bad format specifiers \"%s\" in string %i";
const wxString STR_EXT_CMD_BTN_EXEC = "Execute (Ctrl+E)";
const wxString STR_EXT_CMD_BTN_ABRT = "Abort (Ctrl+A)";

//---------------------------------------------------------------------------------------

bool ExtractFormatSpecifiers(wxString &from, wxArrayString &to, int date_fmt, bool stop_on_error = true)
{

    //This is a very crude method to extract valid format specifiers in order to
    //compare them and thus allowing validation of formatters in translated strings

    const wxString MODIFIERS[2]  = {
        "hljztL.0123456789", //printf
        "E0" //strftime
    };

    const wxString SPECIFIERS[2] = {
        "csdioxXufFeEaAgGnp", //printf
        "ntYyCGgbBhmUWVjdeaAwuHIMScxXDFrRTpzZ" //strftime
    };

    int pos = 0, len = from.Len(), errs = 0;

    while (pos < len)
    {

        if (from.at(pos++) == '%')
        {

            if (from.at(pos) == '%') pos++;
            else
            {

                wxString fs;
                int ok = 0;

                while ((ok == 0) && (pos < len))
                {

                    wxUniCharRef c = from.at(pos++);
                    if (SPECIFIERS[date_fmt].Find(c) >= 0) ok = 1; //Specifier found = success
                    else if (MODIFIERS[date_fmt].Find(c) < 0) ok = 2; //Invalid modifier
                    fs += c;

                }

                if (ok == 1) to.Add(fs);
                else
                {
                    to.Add(fs);
                    //to.Add("!" + fs);
                    errs++;
                    if (stop_on_error) pos = len + 1;
                }

            }

        }

    }

    return (errs == 0);

}

//---------------------------------------------------------------------------------------

bool ValidateFormatSpecifiers(LPFFQ_STRING org, wxString &str, wxString *info = nullptr)
{

    wxArrayString a1, a2;

    //Validate date and time format
    if (org->sid == SID_DATE_TIME_FORMAT)
    {
        bool res = ExtractFormatSpecifiers(str, a1, 1);
        if ((!res) && info) *info = wxString::Format("%%%s is invalid for date & time", a1[a1.Count() - 1]);
        return res;
    }

    //Validate string format specifiers
    if (!ExtractFormatSpecifiers(str, a1, 0))
    {
        if (info) *info = wxString::Format("%%%s is invalid for strings", a1[a1.Count() - 1]);
        return false;
    }
    ExtractFormatSpecifiers(org->str, a2, 0);

    //Numbers mismatch?
    if (a1.Count() != a2.Count())
    {
        if (info) *info = wxString::Format("Count mismatch: Want %u, got %u", a2.Count(), a1.Count());
        return false;
    }

    //Compare format specifiers
    for (size_t i = 0; i < a1.Count(); i++)
    {
        if (a1[i] != a2[i])
        {
            if (info) *info = wxString::Format("Type mismatch: Want %%%s, got %%%s", a2[i], a1[i]);
            return false;
        }
    }

    //All peachy!
    return true;

}

//---------------------------------------------------------------------------------------

unsigned int PreArray(wxString &str, bool Replace_CRLF)
{
    //Prepare an array for editing, or count number of items
    str.Replace("||", "\a"); //Prevent "||" from being recognized as two array separators
    int lc = Replace_CRLF ? str.Replace("|", CRLF) : (unsigned int)str.Freq('|');
    str.Replace("\a", "||"); //Return "||" to the string
    return lc;
}

//---------------------------------------------------------------------------------------

unsigned int PostArray(wxString &str)
{
    //After editing, restore the array
    wxString t, ls = "";
    FFQLineParser lp(str);
    while (lp.has_more())
    {
        t = StrTrim(lp.next());
        if (t.Len() > 0) ls += t + "|";
    }
    str = ls.RemoveLast(); //Remove last "|"
    return (unsigned int)str.Freq('|');
}

//---------------------------------------------------------------------------------------

FFQLangEdit* FFQLangEdit::m_Instance = NULL;

//---------------------------------------------------------------------------------------

FFQLangEdit* FFQLangEdit::Get(wxWindow *parent)
{
    if (m_Instance == NULL) m_Instance = new FFQLangEdit(NULL);//parent);
    return m_Instance;
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::Release()
{
    DELETE_OBJ(m_Instance);
}

//---------------------------------------------------------------------------------------

FFQLangEdit::FFQLangEdit(wxWindow* parent)
{
	//(*Initialize(FFQLangEdit)
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer5;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer7;
	wxFlexGridSizer* FlexGridSizer8;
	wxStaticBoxSizer* SBS1;
	wxStaticBoxSizer* SBS2;
	wxStaticBoxSizer* SBS3;
	wxStaticText* ST5;

	Create(parent, wxID_ANY, _T("FFQueue language editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL|wxWANTS_CHARS, _T("wxID_ANY"));
	SetMinSize(wxSize(1000,800));
	TabPanel = new wxPanel(this, ID_TABPANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_TABPANEL"));
	MainSizer = new wxFlexGridSizer(5, 1, 0, 0);
	MainSizer->AddGrowableCol(0);
	MainSizer->AddGrowableRow(1);
	MainSizer->AddGrowableRow(3);
	SBS1 = new wxStaticBoxSizer(wxVERTICAL, TabPanel, _T("Language information"));
	FlexGridSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	ST2 = new wxStaticText(TabPanel, ID_ST2, _T("Description:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_ST2"));
	FlexGridSizer2->Add(ST2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 3);
	Description = new wxTextCtrl(TabPanel, ID_DESCRIPTION, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DESCRIPTION"));
	Description->SetMaxLength(100);
	FlexGridSizer2->Add(Description, 1, wxALL|wxEXPAND, 3);
	ST3 = new wxStaticText(TabPanel, ID_ST3, _T("Password:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_ST3"));
	FlexGridSizer2->Add(ST3, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 3);
	FlexGridSizer3 = new wxFlexGridSizer(1, 3, 0, 0);
	FlexGridSizer3->AddGrowableCol(0);
	FlexGridSizer3->AddGrowableCol(2);
	Password1 = new wxTextCtrl(TabPanel, ID_PASSWORD1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD, wxDefaultValidator, _T("ID_PASSWORD1"));
	Password1->SetMaxLength(50);
	FlexGridSizer3->Add(Password1, 1, wxALL|wxEXPAND, 0);
	ST4 = new wxStaticText(TabPanel, ID_ST4, _T("Re-type password:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_ST4"));
	FlexGridSizer3->Add(ST4, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Password2 = new wxTextCtrl(TabPanel, ID_PASSWORD2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD, wxDefaultValidator, _T("ID_PASSWORD2"));
	FlexGridSizer3->Add(Password2, 1, wxALL|wxEXPAND, 0);
	FlexGridSizer2->Add(FlexGridSizer3, 1, wxALL|wxEXPAND, 3);
	SBS1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 3);
	MainSizer->Add(SBS1, 1, wxALL|wxEXPAND, 5);
	ListView = new wxListView(TabPanel, ID_LISTVIEW, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_NO_SORT_HEADER, wxDefaultValidator, _T("ID_LISTVIEW"));
	MainSizer->Add(ListView, 2, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	FlexGridSizer5 = new wxFlexGridSizer(1, 2, 0, 0);
	FlexGridSizer5->AddGrowableCol(1);
	StaticText1 = new wxStaticText(TabPanel, ID_STATICTEXT1, _T("Filter:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer5->Add(StaticText1, 1, wxTOP|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FilterText = new wxTextCtrl(TabPanel, ID_FILTERTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTERTEXT"));
	FlexGridSizer5->Add(FilterText, 1, wxTOP|wxLEFT|wxEXPAND, 5);
	MainSizer->Add(FlexGridSizer5, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
	SBS2 = new wxStaticBoxSizer(wxHORIZONTAL, TabPanel, _T("Edit selected string"));
	FlexGridSizer4 = new wxFlexGridSizer(2, 1, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	FlexGridSizer4->AddGrowableRow(0);
	StrEdit = new wxTextCtrl(TabPanel, ID_STREDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_MULTILINE|wxTE_DONTWRAP, wxDefaultValidator, _T("ID_STREDIT"));
	FlexGridSizer4->Add(StrEdit, 1, wxALL|wxEXPAND, 3);
	Info = new wxStaticText(TabPanel, ID_INFO, _T("Use [Ctrl]+[Enter] to store && next, [Ctrl]+[Up/Down] to navigate list (skipping changes), [Ctrl]+[S] to store changes, [Ctrl]+[I] to load integrated string."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_INFO"));
	Info->Disable();
	FlexGridSizer4->Add(Info, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	SBS2->Add(FlexGridSizer4, 1, wxALL|wxEXPAND, 3);
	MainSizer->Add(SBS2, 1, wxALL|wxEXPAND, 5);
	SBS3 = new wxStaticBoxSizer(wxVERTICAL, TabPanel, _T("External translation"));
	FlexGridSizer8 = new wxFlexGridSizer(3, 1, 0, 0);
	FlexGridSizer8->AddGrowableCol(0);
	FlexGridSizer6 = new wxFlexGridSizer(1, 3, 0, 0);
	FlexGridSizer6->AddGrowableCol(1);
	ST5 = new wxStaticText(TabPanel, wxID_ANY, _T("Command:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	FlexGridSizer6->Add(ST5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	ExtCmd = new wxTextCtrl(TabPanel, ID_EXTCMD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_EXTCMD"));
	FlexGridSizer6->Add(ExtCmd, 1, wxALL|wxEXPAND, 3);
	ExtCmdBtn = new wxButton(TabPanel, ID_EXTCMDBTN, _T("E"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_EXTCMDBTN"));
	ExtCmdBtn->SetLabel(STR_EXT_CMD_BTN_EXEC);
	FlexGridSizer6->Add(ExtCmdBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	FlexGridSizer8->Add(FlexGridSizer6, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 3);
	StaticLine1 = new wxStaticLine(TabPanel, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	FlexGridSizer8->Add(StaticLine1, 1, wxALL|wxEXPAND, 3);
	FlexGridSizer7 = new wxFlexGridSizer(2, 7, 0, 0);
	StaticText2 = new wxStaticText(TabPanel, ID_STATICTEXT2, _T("Export:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer7->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	ExpInternal = new wxRadioButton(TabPanel, ID_EXPINTERNAL, _T("Everything"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_EXPINTERNAL"));
	ExpInternal->SetValue(true);
	FlexGridSizer7->Add(ExpInternal, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	ExpNew = new wxRadioButton(TabPanel, ID_EXPNEW, _T("Only untranslated && modified"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_EXPNEW"));
	FlexGridSizer7->Add(ExpNew, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	ExpFile = new wxButton(TabPanel, ID_EXPFILE, _T("Select file..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_EXPFILE"));
	FlexGridSizer7->Add(ExpFile, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer7->Add(25,-1,1, wxALL|wxEXPAND, 5);
	StaticText3 = new wxStaticText(TabPanel, ID_STATICTEXT3, _T("Import:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer7->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	ImpFile = new wxButton(TabPanel, ID_IMPFILE, _T("Select file..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_IMPFILE"));
	FlexGridSizer7->Add(ImpFile, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	FlexGridSizer8->Add(FlexGridSizer7, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 3);
	SBS3->Add(FlexGridSizer8, 1, wxALL|wxEXPAND, 3);
	MainSizer->Add(SBS3, 1, wxALL|wxEXPAND, 5);
	TabPanel->SetSizer(MainSizer);
	SB = new wxStatusBar(this, ID_SB, wxST_SIZEGRIP, _T("ID_SB"));
	int __wxStatusBarWidths_1[4] = { 1, -200, -200, -200 };
	int __wxStatusBarStyles_1[4] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
	SB->SetFieldsCount(4,__wxStatusBarWidths_1);
	SB->SetStatusStyles(4,__wxStatusBarStyles_1);
	SetStatusBar(SB);
	Timer.SetOwner(this, ID_FILTERTIMER);
	SaveFile = new wxFileDialog(this, _T("Export translation file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
	OpenFile = new wxFileDialog(this, _T("Import translation file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
	Center();

	Connect(ID_LISTVIEW,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&FFQLangEdit::OnListViewItemSelect);
	Connect(ID_FILTERTEXT,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&FFQLangEdit::OnAction);
	Connect(ID_EXTCMDBTN,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FFQLangEdit::OnAction);
	Connect(ID_EXPFILE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FFQLangEdit::OnAction);
	Connect(ID_IMPFILE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FFQLangEdit::OnAction);
	Connect(ID_FILTERTIMER,wxEVT_TIMER,(wxObjectEventFunction)&FFQLangEdit::OnFilterTimer);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&FFQLangEdit::OnClose);
	//*)

    ListView->AppendColumn("ID", wxLIST_FORMAT_LEFT, 100);
    ListView->AppendColumn("Status", wxLIST_FORMAT_LEFT, 100);
    ListView->AppendColumn("Content", wxLIST_FORMAT_LEFT, 550);

    m_Password = "";
    m_ExpImpFile = FFQCFG()->GetConfigPath("translate.txt");
    m_OrgLang = nullptr;
    m_EditLang = nullptr;
	m_EditStr = nullptr;
	m_OrgStr = nullptr;
	m_EditIndex = -1;
	m_SkipEvents = false;
	//m_ExtTranslate = 0;
	m_Modified = 0;
    m_Process = new FFQProcess();
    m_ProgressDlg = nullptr;

    //Get command from config
    ExtCmd->ChangeValue(FFQCFG()->translate_cmd);

    BindKeyEvents(this);

	//Connect(wxID_ANY, wxEVT_IDLE, (wxObjectEventFunction)&FFQLangEdit::OnIdle);
	Bind(wxEVT_IDLE, (wxObjectEventFunction)&FFQLangEdit::OnIdle, this);
	//StrEdit->Bind(wxEVT_KEY_DOWN, (wxObjectEventFunction)&FFQLangEdit::OnKeyDown, this, ID_STREDIT);
	//ExtCmdBtn->Bind(wxEVT_KEY_DOWN, (wxObjectEventFunction)&FFQLangEdit::OnKeyDown, this, ID_EXTCMDBTN);
	//Bind(wxEVT_KEY_DOWN, (wxObjectEventFunction)&FFQLangEdit::OnKeyDown, this);//, ID_EXTCMDBTN);

    CenterOnScreen();

}

//---------------------------------------------------------------------------------------

FFQLangEdit::~FFQLangEdit()
{
	//(*Destroy(FFQLangEdit)
	SaveFile->Destroy();
	OpenFile->Destroy();
	//*)
    if (m_EditLang && (m_Modified > 0)) m_EditLang->SaveLanguage();
	DELETE_OBJ(m_OrgLang);
	DELETE_OBJ(m_EditLang);
	DELETE_OBJ(m_Process);
	DELETE_OBJ(m_ProgressDlg);
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::Execute()
{

    //Ask for password (if necessary)
    bool new_lang = false;
    //FilterText->ChangeValue(wxEMPTY_STRING);

    if (FFQL()->HasPassword())
    {
        if (!FFQL()->CheckPassword(m_Password))
        {
            m_Password = wxGetPasswordFromUser("Please enter the password to the language file", "Authentication");
            if (!FFQL()->CheckPassword(m_Password))
            {
                if (DoConfirm("The password does not match! Do you want to create a new language file?")) new_lang = true;
                else return;
                //ShowError("Bad password");
                //return;
            }
        }
    }

    if (m_OrgLang == NULL)
    {
        //Prepare a copy of the internal language and a copy for edit
        m_OrgLang = new FFQLang(false);
        m_EditLang = new FFQLang(!new_lang);

        //Fill the list (only done once)
        LoadList();
    }

    //Display the window
    if (IsIconized()) Restore();
    if (IsVisible()) Raise();
    else Show();

    //ExportTranslationFile();

}

//---------------------------------------------------------------------------------------

void FFQLangEdit::LoadList()
{
    Description->SetValue(m_EditLang->GetDescription());
    //Description->SetModified(false);
    if (m_EditLang->HasPassword())
    {
        Password1->SetValue(DUMMY_PASSWORD);
        //Password1->SetModified(false);
        Password2->SetValue(DUMMY_PASSWORD);
        //Password2->SetModified(false);
    }
    ListView->Freeze();
    ListView->DeleteAllItems();
    m_Filter = StrTrim(FilterText->GetValue()).Lower();
    for (unsigned int i = 0; i < m_EditLang->GetCount(); i++)
    {
        LPFFQ_STRING ps = m_EditLang->GetPtrAtIndex(i);
        if ((m_Filter.Len() == 0) || (ps->str.Lower().Find(m_Filter) >= 0)) SetListItem(-1, ps);
    }
    ListView->Thaw();
    if (ListView->GetItemCount() > 0) ListView->Select(0);
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::ProgressAbort()
{
    m_Process->Abort(false);
}

//---------------------------------------------------------------------------------------

bool FFQLangEdit::ProgressStep(unsigned int step)
{

    //This is invoked from ProgressDlg and must return "true"
    //to continue to next step or false to abort
    bool res = true;
    //return res;

    //try
    //{

        #ifdef DEBUG
        FFQConsole::Get()->AppendLine(wxString::Format("Step: %lu, %s, %s", (unsigned long)m_OrgStr, m_OrgStr->str, m_ExtCmd), COLOR_BLUE);
        #endif

        wxString val = m_OrgStr->str;

        //val.Replace("<br>", CRLF);

        unsigned int array_size = PreArray(val, true);
        //if (array_size == 0) val.Replace("<br>", CRLF);

        if (m_ExtCmd.Find(COMMAND_REPLACE) < 0)
        {

            //Use stdin to transfer the entire value to external translate
            res = ExternalTranslate(&val);

        }
        else
        {

            //Perform external translation for each non-empty line
            wxString tl = "";
            FFQLineParser lp(val);
            m_ProgressDlg->SetProgressSteps(lp.count(), 0);

            while (res && lp.has_more())
            {

                m_ProgressDlg->ProgressNext();

                wxString ll = lp.next();
                if (ll.Len() > 0)
                {

                    res = ExternalTranslate(&ll);
                    if (res)
                    {
                        if (tl.Len() > 0) tl += CRLF + ll;
                        else tl = ll;
                    }
                    else break;

                }
                else tl += CRLF; //Blank line

            }

            val = tl;

        }

        #ifdef DEBUG
        FFQConsole::Get()->AppendLine(wxString::Format("Result: %u, aborted=%u", (unsigned int)res, (unsigned int)m_Process->WasAborted()), COLOR_BLUE);
        #endif

        if (res)
        {

            //Validate array size?
            SetEditStr(val, array_size);
            m_ExtValue = val; //Value stored to be able to set "SF_EXTERNAL" in SaveEditStr()

        }
        /*else if (!m_Process->WasAborted())
        {

            val = StrTrim(m_Process->GetProcessOutput(true, true));
            if (val.Len() > 0) ShowError(StrEdit, val);

        }*/

    //} catch (...) {}

    return res;// && (!m_Process->WasAborted());
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::SetListItem(long idx, LPFFQ_STRING ffqs)
{
    if (idx < 0) idx = ListView->InsertItem(ListView->GetItemCount(), ToStr(ffqs->sid)); //New item
    else ListView->SetItemText(idx, ToStr(ffqs->sid)); //Existing item

    //Set status
    int status = 0; //New item
    if ((ffqs->flags & SF_STORED) != 0)
    {
        if ((ffqs->flags & SF_MODIFIED) != 0) status = 3; //Modified
        else if ((ffqs->flags & SF_EXTERNAL) != 0) status = 4; //Externally translated
        else if ((ffqs->flags & SF_TRANSLATED) != 0) status = 2; //Translated
        else status = 1; //Internal item
    }
    ListView->SetItem(idx, 1, STRING_STATUS[status]);
    wxString s = ffqs->str;
    s.Replace(CRLF, "<br>");
    ListView->SetItem(idx, 2, s);
    ListView->SetItemPtrData(idx, (wxUIntPtr)ffqs);
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::BindKeyEvents(wxWindow *wnd)
{
	wnd->Bind(wxEVT_KEY_DOWN, (wxObjectEventFunction)&FFQLangEdit::OnKeyDown, this);
	wxWindowListNode *n = wnd->GetChildren().GetFirst();
	while (n)
    {
        wxWindow *cw = dynamic_cast<wxWindow*>(n->GetData());
        if (cw) BindKeyEvents(cw);
        n = n->GetNext();
    }
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::ExportTranslationFile()//wxString filename)
{

    try
    {
        if (wxFileExists(m_ExpImpFile)) wxRemoveFile(m_ExpImpFile);
        wxFile file(m_ExpImpFile, wxFile::write);
        wxString s;
        for (unsigned int i = 0; i < m_OrgLang->GetCount(); i++)
        {
            LPFFQ_STRING org = m_OrgLang->GetPtrAtIndex(i);
            if (ExpNew->GetValue())
            {
                LPFFQ_STRING ed = m_EditLang->FindString(org->sid);
                if (ed && (ed->flags != 0) && ((ed->flags & SF_MODIFIED) != 0)) continue; //Skip if not new or modified
            }
            s = org->str;
            s.Replace("<br>", CRLF);
            s = wxString::Format("[%u:%s]"+CRLF+"%s"+CRLF+CRLF, org->sid, BufToHexStr(org->str_hash, sizeof(org->str_hash)), s);
            wxScopedCharBuffer cb = s.ToUTF8();
            file.Write(cb.data(), cb.length());
        }
        file.Close();
    }
    catch (std::exception &err)
    {
        ShowError(_(err.what()));
    }
}

//---------------------------------------------------------------------------------------

int StoreImportedString(FFQLang *org_lang, FFQLang *edit_lang, FFQ_SID sid, STR_HASH &sh, wxString &str)
{

    //Return 0 = OK, 1 = ERR, -1 = skipped / unmodified

    LPFFQ_STRING org = org_lang->FindString(sid);
    str.Trim(); //Remove trailing CRLF

    if (org == nullptr) FFQConsole::Get()->AppendLine(wxString::Format(STR_BAD_STRING_ID, sid), COLOR_RED);
    else if (str.Len() == 0) FFQConsole::Get()->AppendLine(wxString::Format(STR_EMPTY_STRING, sid), COLOR_RED);
    else if (memcmp(&org->str_hash, &sh, sizeof(STR_HASH)) != 0) FFQConsole::Get()->AppendLine(wxString::Format(STR_BAD_HASH, sid), COLOR_RED);
    else if (str == org->str) return -1; //No error if untranslated
    else
    {

        //Check for illegal character(s)
        unsigned int nl = PreArray(str, false);
        if (nl > 0) FFQConsole::Get()->AppendLine(wxString::Format(STR_ILLEGAL_CHAR, sid), COLOR_RED);
        else
        {

            //Check if array size matches
            unsigned int ol = PreArray(org->str, false);
            if (ol > 0)
            {
                nl = PostArray(str);
                if (ol != nl)
                {
                    FFQConsole::Get()->AppendLine(wxString::Format(STR_BAD_ARRAY_SIZE, nl, ol, sid), COLOR_RED);
                    return 1;
                }
            }

            wxString info;
            if (!ValidateFormatSpecifiers(org, str, &info))
            {
                FFQConsole::Get()->AppendLine(wxString::Format(STR_BAD_FORMAT_SPEC, info, sid), COLOR_RED);
                return 1;
            }

            LPFFQ_STRING ed = edit_lang->FindString(sid);
            ed->str = str;

            //Update hashes
            //memcpy(&ed->org_hash, &org->str_hash, sizeof(STR_HASH));
            HashString(org->str, ed->org_hash);
            HashString(ed->str, ed->str_hash);

            //Set flag
            ed->flags = SF_STORED | SF_TRANSLATED | SF_EXTERNAL;
            return 0;

        }

    }

    return 1;

}

bool ParseImportHeader(wxString &header, int *sid = nullptr, STR_HASH *sh = nullptr)
{
    if (header.StartsWith('[') && header.EndsWith(']'))
    {
        int p = header.Find(':');
        if (p > 0)
        {
            int i = Str2Long(header.SubString(1, p - 1), -1);
            if ((i >= 0) && (i <= std::numeric_limits<FFQ_SID>::max()))
            {
                wxString hs = header.SubString(p + 1, header.Len() - 2);
                STR_HASH h;
                if (HexToBuf(hs, h, sizeof(STR_HASH)) == sizeof(STR_HASH))
                {
                    if (sid) *sid = i;
                    if (sh) memcpy(*sh, h, sizeof(STR_HASH));
                    return true;
                }
            }
        }
    }
    return false;
}

void FFQLangEdit::ImportTranslationFile()//wxString filename)
{

    if (!wxFileExists(m_ExpImpFile)) return;

    try
    {
        wxFile file(m_ExpImpFile, wxFile::read);
        wxString all;
        file.ReadAll(&all);
        file.Close();

        int okcnt = 0, errcnt = 0, sid = -1, sres;
        STR_HASH shash;
        wxString str, s, t, v;
        FFQLineParser lp(all);

        while (lp.has_more())
        {

            s = StrTrim(lp.next());

            if (ParseImportHeader(s))//s.StartsWith('[') && s.EndsWith(']'))
            {

                if (sid >= 0)
                {
                    sres = StoreImportedString(m_OrgLang, m_EditLang, sid, shash, str);
                    if (sres == 0) okcnt++;
                    else if (sres == 1) errcnt++;
                    sid = -1;
                    str.Clear();
                }

                ParseImportHeader(s, &sid, &shash);

            }
            else if (sid >= 0)
            {
                if (str.Len() > 0) str += CRLF;
                str += s;
            }

        }

        if (sid >= 0)
        {
            //Store final string
            sres = StoreImportedString(m_OrgLang, m_EditLang, sid, shash, str);
            if (sres == 0) okcnt++;
            else if (sres == 1) errcnt++;
        }
        if (okcnt > 0)
        {

		    m_EditStr = m_OrgStr = nullptr;
		    m_EditIndex = -1;
            LoadList();
		    m_Modified = okcnt;

        }
        ShowInfo(wxString::Format("%i imported, %i errors (details in console)", okcnt, errcnt));

    }
    catch (std::exception &err)
    {
        ShowError(_(err.what()));
    }

}

//---------------------------------------------------------------------------------------

bool FFQLangEdit::ExternalTranslate(wxString *value)
{

    if (value)
    {

        //Perform external translation of a string
        try
        {

            wxString cmd = m_ExtCmd;
            bool use_stdin = (cmd.Replace(COMMAND_REPLACE, *value) == 0);
            m_Process->SetCommand(cmd, wxEmptyString, false);
            m_Process->Execute(false, true, true);

            if (use_stdin)
            {
                wxScopedCharBuffer cb = value->utf8_str();
                m_Process->WriteToStdin(cb.data(), cb.length(), true);
            }

            if (!m_Process->WaitFor(COMMAND_TIMEOUT, false))
            {
                m_ExtCmdRes = ecrTimeout;
                m_Process->Abort(false, 0);
            }
            else if (!m_Process->WasAborted())
            {

                *value = StrTrim(m_Process->GetProcessOutput(false, true));
                if ((*value).Len() > 0) return true;
                m_ExtCmdRes = ecrError;

            }

        }
        catch (...)
        {
            m_ExtCmdRes = ecrError;
        }

        return false;

    }
    else
    {

        //Start external translation of m_EditStr
        m_ExtCmd = StrTrim(ExtCmd->GetValue());
        if (m_EditStr && (m_ExtCmd.Len() > 0))
        {

            //Save command to config
            if (FFQCFG()->translate_cmd !=  m_ExtCmd)
            {
                FFQCFG()->translate_cmd =  m_ExtCmd;
                FFQCFG()->SaveConfig();
            }

            //Run progress dialog
            if (m_ProgressDlg == nullptr) m_ProgressDlg = new FFQProgressDlg(this, "Translating...");
            m_ExtCmdRes = ecrNone;
            m_ProgressDlg->Execute(1);
            if (m_ExtCmdRes == ecrError) ShowError(ExtCmd, "The command failed or did not return anything. Any typos or missing quotes?");
            else if (m_ExtCmdRes == ecrTimeout) ShowError(ExtCmd, "The command timed out");
            return true;

        }

    }

    return false;

}

//---------------------------------------------------------------------------------------

void FFQLangEdit::LoadEditStr(bool internal)
{
    m_EditIndex = ListView->GetFirstSelected();
    if (m_EditIndex < 0) return;
    m_EditStr = (LPFFQ_STRING)ListView->GetItemData(m_EditIndex);
    m_OrgStr = m_OrgLang->GetPtrAtIndex(m_EditLang->IndexOf(m_EditStr));
    SetEditStr(internal ? m_OrgStr->str : m_EditStr->str);
}

//---------------------------------------------------------------------------------------

bool FFQLangEdit::SaveEditStr(bool force, bool clear)
{

    if (m_EditStr != NULL)
    {

        //Get value
        wxString s = StrTrim(StrEdit->GetValue());

        if (force || (m_EditStr->str != s))// StrEdit->IsModified())
        {

            //The content of the string has been modified - full verification is needed
            if (PreArray(s, false) > 0) return SkipItemChange(wxString::Format(STR_ILLEGAL_CHAR, m_EditStr->sid));

            if ((s.Len() > 0) && (m_EditListSize > 1))
            {

                //Item is a list of items - validate the number of items
                unsigned int items = PostArray(s);
                if (items != m_EditListSize) return SkipItemChange(wxString::Format(STR_BAD_ARRAY_SIZE, m_EditListSize, items, m_EditStr->sid) + "\n\n" + s);

            }

            wxString info;
            if (!ValidateFormatSpecifiers(m_OrgStr, s, &info)) return SkipItemChange(wxString::Format(STR_BAD_FORMAT_SPEC, info, m_EditStr->sid));

            //s.Shrink();

            if ((s.Len() == 0) || (s == m_OrgStr->str))
            {

                //Revert to internal value
                m_EditStr->str = m_OrgStr->str;
                m_EditStr->flags = SF_STORED;

            }

            else
            {

                //Set translated string value
                m_EditStr->str = s;
                m_EditStr->flags = SF_STORED | SF_TRANSLATED;

                //Externally translated?
                if (m_ExtValue == s) m_EditStr->flags |= SF_EXTERNAL;

            }

            HashString(m_OrgStr->str, m_EditStr->org_hash);
            HashString(m_EditStr->str, m_EditStr->str_hash);

            //Update list item
            SetListItem(m_EditIndex, m_EditStr);

            //Increase modification counter to ensure file write
            m_Modified++;

        }

    }

    if (clear)
    {

        //Clear the item currently being edited
        m_EditStr = nullptr;
        m_OrgStr = nullptr;
        m_EditIndex = -1;
        StrEdit->Clear();

    }

    //StrEdit->SetModified(false);
    UpdateStatus();
    return true;

}

//---------------------------------------------------------------------------------------

bool FFQLangEdit::SavePwdAndDesc()
{

    //Set description, if modified
    wxString s = StrTrim(Description->GetValue());
    unsigned int r;
    do { r = s.Replace("<br>", SPACE); } while (r > 0);
    do { r = s.Replace("  ", SPACE); } while (r > 0);
    s.Trim();
    if (s != m_EditLang->GetDescription())
    {
        m_EditLang->SetDescription(s);
        m_Modified++;
    }

    //Verify password
    s = Password1->GetValue();
    if (s != Password2->GetValue()) return ShowError(Password2, "The passwords are not equal and cannot be set");

    //Store password, if changed
    if ((s != DUMMY_PASSWORD) && (!m_EditLang->CheckPassword(s)))
    {
        m_EditLang->ChangePassword(s);
        m_Modified++;
    }

    //All OK
    return true;
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::SetEditStr(wxString str, unsigned int array_size)
{
    m_EditListSize = (array_size == 0) ? PreArray(str, true) : array_size;
    StrEdit->ChangeValue(str);
    m_ExtValue.Clear();
    if (m_EditIndex >= 0)
    {
        ListView->EnsureVisible(m_EditIndex);
        UpdateStatus();
    }
}

//---------------------------------------------------------------------------------------

bool FFQLangEdit::SkipItemChange(wxString reason)
{
    m_SkipEvents = true;
    ListView->Select(m_EditIndex, true);
    return ShowError(StrEdit, reason);
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::UpdateStatus()
{
    SB->SetStatusText("", 0);
    SB->SetStatusText(wxString::Format("Editing %i of %u", (int)(ListView->GetFirstSelected()+1), SIZEFMT(ListView->GetItemCount()), 1), 1);
    int pct = (int)( (float)m_EditLang->GetFlagCount(SF_STORED) / (float)m_EditLang->GetCount() * 100.0  );
    SB->SetStatusText(wxString::Format("%i%% translated", pct), 2);
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::OnClose(wxCloseEvent& event)
{

    if (event.CanVeto())
    {
        event.Veto();
        if (!SavePwdAndDesc()) return;
        if (!SaveEditStr(false)) return;
        Show(false);
    }
    else
    {
        SavePwdAndDesc();
        SaveEditStr(false);
    }
    if (Destroy()) m_Instance = nullptr;

}

//---------------------------------------------------------------------------------------

void FFQLangEdit::OnListViewItemSelect(wxListEvent& event)
{
    if (m_SkipEvents || (event.GetIndex() == m_EditIndex)) return;
    if (SaveEditStr(false)) LoadEditStr();
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::OnIdle(wxIdleEvent &event)
{
    if ((m_EditIndex >= 0) && (ListView->GetFirstSelected() != m_EditIndex)) ListView->Select(m_EditIndex, true);
    else m_SkipEvents = false;
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::OnKeyDown(wxKeyEvent &event)
{

    if (event.ControlDown())
    {

        int key = ((int)event.GetUnicodeKey() == 0) ? event.GetKeyCode() : (int)event.GetUnicodeKey();

        long sel = ListView->GetFirstSelected();

        if ((key == WXK_UP) && (sel > 0))
        {
            //if (StrEdit->IsModified()) LoadEditStr();
            //StrEdit->SetModified(false);
            ListView->Select(sel-1, true);

        }

        else if ((key == WXK_DOWN) && (sel < ListView->GetItemCount() - 1))
        {
            //if (StrEdit->IsModified()) LoadEditStr();
            //StrEdit->SetModified(false);
            ListView->Select(sel+1, true);

        }

        else if (key == WXK_RETURN)
        {

            if (!SaveEditStr(true, false)) return;

            while ((unsigned int)sel++ < m_EditLang->GetCount())
            {
                LPFFQ_STRING ffqs = m_EditLang->GetPtrAtIndex(sel);
                if ((ffqs->flags & SF_STORED) == 0)
                {
                     ListView->Select(sel, true);
                     return;
                }
            }

            ShowInfo(StrEdit, "The end has been reached, my friend :-)");

        }

        else if (key == 83) SaveEditStr(true, false); //S

        else if (key == 73) LoadEditStr(true); //I

        else if (/*(key == 65) ||*/ (key == 69)) ExternalTranslate(); //A or E

        else event.Skip();

    }

    else event.Skip();

}

//---------------------------------------------------------------------------------------

void FFQLangEdit::OnFilterTimer(wxTimerEvent& event)
{
    SaveEditStr(false, true);
    LoadList();
}

//---------------------------------------------------------------------------------------

void FFQLangEdit::OnAction(wxCommandEvent& event)
{

    int id = event.GetId();

    if (id == ID_FILTERTEXT)
    {
        wxString filter = StrTrim(FilterText->GetValue()).Lower();
        if (m_Filter.Cmp(filter) != 0) Timer.StartOnce(FILTER_TIMEOUT);
        else Timer.Stop();
    }
    else if (id == ID_EXTCMDBTN)
    {
        ExternalTranslate();
    }
    else if (id == ID_EXPFILE)
    {
        if (FFQCFG()->FileDlgExecute("lang", SaveFile, nullptr, true, &m_ExpImpFile))
        {
            m_ExpImpFile = SaveFile->GetPath();
            ExportTranslationFile();
            Raise();
        }
    }
    else if (id == ID_IMPFILE)
    {
        //if ((m_Modified) > 0 && (!DoConfirm(EditStr, "You have modified strings")))) return;
        if (FFQCFG()->FileDlgExecute("lang", OpenFile, nullptr, true, &m_ExpImpFile))
        {
            m_ExpImpFile = OpenFile->GetPath();
            ImportTranslationFile();
            Raise();
        }
    }
}
