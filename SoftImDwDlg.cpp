// SoftImDwDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "SoftImDwDlg.h"

// CSoftImDwDlg 對話方塊

IMPLEMENT_DYNAMIC(CSoftImDwDlg, CDialog)
CSoftImDwDlg::CSoftImDwDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoftImDwDlg::IDD, pParent)
{
}

CSoftImDwDlg::~CSoftImDwDlg()
{
}

void CSoftImDwDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_PROMPT_MSG, m_msg);
	DDX_Control(pDX, IDC_PROGRESS_MSG, m_percent);
//	DDX_Control(pDX, IDC_BUTTON1, m_close);
}


BEGIN_MESSAGE_MAP(CSoftImDwDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_MESSAGE(UWM_SETTIMEOUT, OnSetTimeout)
	ON_MESSAGE(UWM_SETPROGRESS, OnSetProgress)
	ON_MESSAGE(UWM_SETPROMPT_MSG, OnSetPromptMessage)

END_MESSAGE_MAP()


// CSoftImDwDlg 訊息處理常式

BOOL CSoftImDwDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_progress.SetRange(0,100);
	m_progress.SetPos(0);
	m_msg.SetWindowText("Waiting for Flash Loading...");
	SetTimer(1, 0, NULL);
	m_percent.SetWindowText("0");

	isFinish = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CSoftImDwDlg::OnTimer(UINT nIDEvent)
{
	if(isFinish)
	{
		KillTimer(1);

		OnOK();
	}

	CDialog::OnTimer(nIDEvent);
}

void CSoftImDwDlg::OnCancel()
{	
	CDialog::OnCancel();
}

void CSoftImDwDlg::OnClose()
{	
	//CDialog::OnClose();
}

LRESULT CSoftImDwDlg::OnSetTimeout(WPARAM wParam, LPARAM lParam)
{
	CString text;
	text.Format("Waiting ACK...%d sec.", wParam / 1000);
	m_percent.SetWindowText(text);	
	return 0;
}

LRESULT CSoftImDwDlg::OnSetProgress(WPARAM wParam, LPARAM lParam)
{
	CString text;
	text.Format("Progress : %d   %%", (int)(wParam * 100.0 / lParam));
	m_percent.SetWindowText(text);
	m_progress.SetRange(0, 100);
	m_progress.SetPos((int)(wParam * 100.0 / lParam));
	return 0;
}

LRESULT CSoftImDwDlg::OnSetPromptMessage(WPARAM wParam, LPARAM lParam)
{
	CString text;
	text.LoadString((UINT)wParam);
	m_msg.SetWindowText(text);	
	return 0;
}

BOOL CSoftImDwDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN && 
		(pMsg->wParam==VK_ESCAPE || pMsg->wParam==VK_RETURN))
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}	