// WaitAckDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "WaitAckDlg.h"

// CWaitAckDlg 對話方塊

IMPLEMENT_DYNAMIC(CWaitAckDlg, CDialog)
CWaitAckDlg::CWaitAckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitAckDlg::IDD, pParent)
{
}

CWaitAckDlg::~CWaitAckDlg()
{

}

void CWaitAckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_msg);
}


BEGIN_MESSAGE_MAP(CWaitAckDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWaitAckDlg 訊息處理常式

void CWaitAckDlg::OnTimer(UINT nIDEvent)
{
	if(IsFinish)
	{
		OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CWaitAckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	IsFinish = false;
	m_msg.SetWindowText("Waiting Ack...");
	SetTimer(1,0,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
}
