#ifndef FFQPROGRESSDLG_H
#define FFQPROGRESSDLG_H

#ifndef WX_PRECOMP
    //(*HeadersPCH(FFQProgressDlg)
    #include <wx/button.h>
    #include <wx/dialog.h>
    #include <wx/gauge.h>
    #include <wx/sizer.h>
    //*)
#endif
//(*Headers(FFQProgressDlg)
//*)

const unsigned int INVALID_STEP = std::numeric_limits<unsigned int>::max();

class FFQProgressDlgReceiver {
    public:
        virtual void ProgressAbort() = 0;
        virtual bool ProgressStep(unsigned int step) = 0;
};

class FFQProgressDlg: public wxDialog
{
    public:

        FFQProgressDlg(wxWindow* parent, wxString title);
        virtual ~FFQProgressDlg();
        bool Execute(unsigned int steps = 0, unsigned int task_id = 0);
        unsigned int GetTaskID();
        bool ProgressNext();
        void ProgressShow();
        void SetProgressSteps(unsigned int steps = 0, unsigned int step = INVALID_STEP);

        //(*Declarations(FFQProgressDlg)
        wxButton* AbortBtn;
        wxGauge* Progress;
        //*)

    protected:

        //(*Identifiers(FFQProgressDlg)
        static const long ID_PROGRESS;
        static const long ID_ABORTBTN;
        //*)

    private:

        unsigned int m_Step, m_Steps, m_TaskID;
        bool m_DoIdle, m_Shown;
        FFQProgressDlgReceiver *m_Receiver;

        void DoAbort(bool by_button);
        void OnIdle(wxIdleEvent &event);
        void OnShow(wxShowEvent &event);

        //(*Handlers(FFQProgressDlg)
        void OnAction(wxCommandEvent& event);
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
