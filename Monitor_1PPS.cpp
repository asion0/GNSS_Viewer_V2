// Monitor_1PPS.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Monitor_1PPS.h"
#include "GPSDlg.h"

// CMonitor_1PPS 對話方塊

IMPLEMENT_DYNAMIC(CMonitor_1PPS, CDialog)
CMonitor_1PPS::CMonitor_1PPS(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_1PPS_MONITOR, pParent)
{
}

CMonitor_1PPS::~CMonitor_1PPS()
{
}

void CMonitor_1PPS::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_1PPS_MONITOR_MODE,m_mode);
	DDX_Control(pDX,IDC_1PPS_MONITOR_LEN,m_length);
	DDX_Control(pDX,IDC_1PPS_MONITOR_ERROR,m_error);

	DDX_Control(pDX,IDC_1PPS_MONITOR_LBL_SET_STD,m_lbl_set_std);
	DDX_Control(pDX,IDC_1PPS_MONITOR_LBL_NOW_STD,m_lbl_now_std);
	DDX_Control(pDX,IDC_1PPS_MONITOR_SET_STD,m_set_std);
	DDX_Control(pDX,IDC_1PPS_MONITOR_NOW_STD,m_now_std);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMonitor_1PPS, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CMonitor_1PPS 訊息處理常式

void CMonitor_1PPS::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	OnOK();
}

BOOL CMonitor_1PPS::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_lbl_set_std.ShowWindow(0);
	m_lbl_now_std.ShowWindow(0);
	m_set_std.ShowWindow(0);
	m_now_std.ShowWindow(0);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CMonitor_1PPS::Show1PPSTiming(int mode,int surveylen,float quan_err)
{
	CString temp;

	if (mode==0)
		m_mode.SetWindowText("PVT Mode");
	else if(mode==1)
		m_mode.SetWindowText("Survey Mode");
	else if(mode ==2)
		m_mode.SetWindowText("Static Mode");

	temp.Format("%d",surveylen);
	m_length.SetWindowText(temp);

	temp.Format("%.1f",quan_err);
	m_error.SetWindowText(temp);
}

void CMonitor_1PPS::Show1PPSTiming(int mode,int surveylen,float quan_err,int set_std,int now_std)
{
	CString temp;

	if (mode==0)
		m_mode.SetWindowText("PVT Mode");
	else if(mode==1)
		m_mode.SetWindowText("Survey Mode");
	else if(mode ==2)
		m_mode.SetWindowText("Static Mode");

	temp.Format("%d",surveylen);
	m_length.SetWindowText(temp);

	temp.Format("%.1f",quan_err);
	m_error.SetWindowText(temp);

	temp.Format("%d",set_std);
	m_set_std.SetWindowText(temp);

	temp.Format("%d",now_std);
	m_now_std.SetWindowText(temp);
}


void CMonitor_1PPS::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	CDialog::OnCancel();

	CGPSDlg::gpsDlg->close_minitor_1pps_window();
}

void CMonitor_1PPS::OnBnClickedCancel()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	OnCancel();
}
