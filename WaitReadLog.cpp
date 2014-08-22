// WaitReadLog.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "WaitReadLog.h"
#include "GPSDlg.h"

extern HANDLE	waitlog;

// CWaitReadLog 對話方塊

IMPLEMENT_DYNAMIC(CWaitReadLog, CDialog)
CWaitReadLog::CWaitReadLog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_WAITLOGREAD, pParent)
{
}

CWaitReadLog::~CWaitReadLog()
{
}

void CWaitReadLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, msg);
}


BEGIN_MESSAGE_MAP(CWaitReadLog, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CWaitReadLog 訊息處理常式

void CWaitReadLog::OnTimer(UINT nIDEvent)
{
	if(IsFinish)
	{
		KillTimer(1);
		OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CWaitReadLog::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetTimer(1,100,NULL);
	IsFinish = false;
	msg.SetWindowText("Please wait for log read!");
	if(!SetEvent(waitlog ))   DWORD error = GetLastError();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CWaitReadLog::OnBnClickedButton1()
{
	CGPSDlg::gpsDlg->cancel_log_read();
	// TODO: 在此加入控制項告知處理常式程式碼
}

void CWaitReadLog::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	//CDialog::OnClose();
}
