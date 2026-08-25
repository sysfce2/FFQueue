//#include "wx_pch.h"
#include "FFQProgressDlg.h"
#include "utils/FFQLang.h"
#include "utils/FFQMisc.h"

#ifdef DEBUG
#include "utils/FFQConsole.h"
#define LOGMSG(msg) FFQConsole::Get()->AppendLine(wxString("ProgressDlg: ") + msg, COLOR_BLUE); wxYield()
#else
#define LOGMSG(msg)
#endif // DEBUG

#ifndef WX_PRECOMP
    //(*InternalHeadersPCH(FFQProgressDlg)
    #include <wx/string.h>
    //*)
#endif
//(*InternalHeaders(FFQProgressDlg)
//*)

//(*IdInit(FFQProgressDlg)
const long FFQProgressDlg::ID_PROGRESS = wxNewId();
const long FFQProgressDlg::ID_ABORTBTN = wxNewId();
//*)

BEGIN_EVENT_TABLE(FFQProgressDlg,wxDialog)
    //(*EventTable(FFQProgressDlg)
    //*)
END_EVENT_TABLE()

FFQProgressDlg::FFQProgressDlg(wxWindow* parent, wxString title)
{
    //(*Initialize(FFQProgressDlg)
    wxBoxSizer* BoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    FlexGridSizer1 = new wxFlexGridSizer(2, 1, 0, 0);
    Progress = new wxGauge(this, ID_PROGRESS, 1000, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_PROGRESS"));
    FlexGridSizer1->Add(Progress, 1, wxALL|wxEXPAND, 5);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer1->Add(-1,-1,1, wxALL|wxEXPAND, 5);
    AbortBtn = new wxButton(this, ID_ABORTBTN, _T("A"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_ABORTBTN"));
    AbortBtn->SetDefault();
    AbortBtn->SetFocus();
    AbortBtn->SetLabel(FFQS(SID_COMMON_ABORT));
    BoxSizer1->Add(AbortBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(-1,-1,1, wxALL|wxEXPAND, 5);
    FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxEXPAND, 5);
    SetSizer(FlexGridSizer1);
    FlexGridSizer1->SetSizeHints(this);

    Connect(ID_ABORTBTN,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FFQProgressDlg::OnAction);
    //*)

    m_Receiver = dynamic_cast<FFQProgressDlgReceiver*>(parent);
    if (m_Receiver == nullptr) throw new FFQError("Parent window must be a FFQProgressDlgReceiver");

    SetTitle(title);//FFQS(SID_PROGRESS_DLG_TITLE));
    Bind(wxEVT_IDLE, (wxObjectEventFunction)&FFQProgressDlg::OnIdle, this);
    Bind(wxEVT_SHOW, (wxObjectEventFunction)&FFQProgressDlg::OnShow, this);

}

//---------------------------------------------------------------------------------------

FFQProgressDlg::~FFQProgressDlg()
{
    //(*Destroy(FFQProgressDlg)
    //*)
}

//---------------------------------------------------------------------------------------

bool FFQProgressDlg::Execute(unsigned int steps, unsigned int task_id)
{

    //m_Receiver = rcv;
    m_Step = 0;
    m_Steps = steps;
    m_TaskID = task_id;
    m_DoIdle = true;
    m_Shown = false;
    AbortBtn->Enable();
    Progress->SetValue(0);
    return (ShowModal() == wxID_OK);

}

//---------------------------------------------------------------------------------------

unsigned int FFQProgressDlg::GetTaskID()
{
    return m_TaskID;
}

//---------------------------------------------------------------------------------------

bool FFQProgressDlg::ProgressNext()
{
    if ((m_Step >= m_Steps)/* && (m_Steps > 0)*/)
    {
        LOGMSG("ProgressNext Done!");
        return false;
    }
    m_Step++;
    LOGMSG(wxString::Format("ProgressNext %u/%u", m_Step, m_Steps));
    ProgressShow();
    return true;
}

//---------------------------------------------------------------------------------------

void FFQProgressDlg::ProgressShow()
{
    if (m_Steps == 0) Progress->SetValue(0);
    else
    {
        int pct = droundi((double)m_Step / (double)m_Steps * 1000.0);
        LOGMSG(wxString::Format("Progress = %i", pct));
        Progress->SetValue(pct);
    }
}

//---------------------------------------------------------------------------------------

void FFQProgressDlg::SetProgressSteps(unsigned int steps, unsigned int step)
{
    LOGMSG(wxString::Format("SetProgressSteps %u/%u", steps, step));
    //if (m_Steps <= m_Step) ShowError(nullptr, wxString::Format("Steps (%u) cannot be smaller or equal to that step (%u)", m_Steps, m_Step));
    m_Steps = steps;
    if (step != INVALID_STEP) m_Step = step;
    ProgressShow();
}

//---------------------------------------------------------------------------------------

void FFQProgressDlg::DoAbort(bool by_button)
{
    //if (AbortBtn->IsEnabled())
    //{
        LOGMSG(wxString::Format("Aborted, button=%i", (int)by_button));
        if (AbortBtn->IsEnabled()) AbortBtn->Disable();
        if (by_button) m_Receiver->ProgressAbort();
        else EndModal(wxID_ABORT);
    //}
}

//---------------------------------------------------------------------------------------

void FFQProgressDlg::OnIdle(wxIdleEvent &event)
{

    if ((!m_Shown) || (!m_DoIdle))
    {
        if (!m_Shown) event.RequestMore();
        return;
    }
    m_DoIdle = false;

    LOGMSG("OnIdle");

    while (ProgressNext())
    {

        if (!m_Receiver->ProgressStep(m_Step - 1))
        {
            DoAbort(false);
            return;
        }

    }

    LOGMSG("Progress finished");
    EndModal(wxID_OK);

}

//---------------------------------------------------------------------------------------

void FFQProgressDlg::OnShow(wxShowEvent &event)
{
    m_Shown = true;
}

//---------------------------------------------------------------------------------------

void FFQProgressDlg::OnAction(wxCommandEvent& event)
{
    if (event.GetId() == ID_ABORTBTN)
    {
        DoAbort(true);
    }
}
