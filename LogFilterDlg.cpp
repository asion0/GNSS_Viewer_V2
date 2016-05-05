// LogFilterDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "LogFilterDlg.h"
#include "GPSDlg.h"


// CLogFilterDlg 對話方塊

IMPLEMENT_DYNAMIC(CLogFilterDlg, CDialog)
CLogFilterDlg::CLogFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogFilterDlg::IDD, pParent)
{
	//memset(m_mic,0,sizeof(m_mic));
}

CLogFilterDlg::~CLogFilterDlg()
{
}

void CLogFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LOGENABLE, m_logenable);
	DDX_Control(pDX, IDC_FIFOMODE, m_fifomode);

	DDX_Text(pDX,IDC_EMINT,CGPSDlg::gpsDlg->m_logFlashInfo.min_time);
	DDX_Text(pDX,IDC_EMAXT,CGPSDlg::gpsDlg->m_logFlashInfo.max_time);
	DDX_Text(pDX,IDC_EMIND,CGPSDlg::gpsDlg->m_logFlashInfo.min_distance);
	DDX_Text(pDX,IDC_EMAXD,CGPSDlg::gpsDlg->m_logFlashInfo.max_distance);
	DDX_Text(pDX,IDC_EMINV,CGPSDlg::gpsDlg->m_logFlashInfo.min_speed);
	DDX_Text(pDX,IDC_EMAXV,CGPSDlg::gpsDlg->m_logFlashInfo.max_speed);

	DDX_Control(pDX,IDC_EMIC,m_mic);
	DDX_Control(pDX,IDC_LBL_MIC,m_lbl_mic);

	DDV_MinMaxUInt(pDX,CGPSDlg::gpsDlg->m_logFlashInfo.min_time,0,0xffffffff);
	DDV_MinMaxUInt(pDX,CGPSDlg::gpsDlg->m_logFlashInfo.max_time,0,0xffffffff);
	DDV_MinMaxUInt(pDX,CGPSDlg::gpsDlg->m_logFlashInfo.min_distance,0,0xffffffff);
	DDV_MinMaxUInt(pDX,CGPSDlg::gpsDlg->m_logFlashInfo.max_distance,0,0xffffffff);
	DDV_MinMaxUInt(pDX,CGPSDlg::gpsDlg->m_logFlashInfo.min_speed,0,0xffffffff);
	DDV_MinMaxUInt(pDX,CGPSDlg::gpsDlg->m_logFlashInfo.max_speed,0,0xffffffff);

	DDX_Control(pDX,IDC_EMINT,m_mint);
	DDX_Control(pDX,IDC_EMAXT,m_maxt);
	DDX_Control(pDX,IDC_EMIND,m_mind);
	DDX_Control(pDX,IDC_EMAXD,m_maxd);
	DDX_Control(pDX,IDC_EMINV,m_minv);
	DDX_Control(pDX,IDC_EMAXV,m_maxv);

	DDX_Control(pDX, IDC_LOGENABLE, m_logenable);
}


BEGIN_MESSAGE_MAP(CLogFilterDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CLogFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_logenable.SetCurSel(CGPSDlg::gpsDlg->m_logFlashInfo.datalog_enable);
	if(_V8_SUPPORT)
	{
		m_fifomode.SetCurSel(CGPSDlg::gpsDlg->m_logFlashInfo.fifo_mode);
		m_fifomode.ShowWindow(SW_SHOW);
	}

	m_mic.ShowWindow(0);
	m_lbl_mic.ShowWindow(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CLogFilterDlg::OnBnClickedOk()
{
	int len = 9;

	if ( m_mint.GetWindowTextLength() > len || m_maxt.GetWindowTextLength() > len ||
		m_mind.GetWindowTextLength() > len || m_maxd.GetWindowTextLength() > len ||
		m_minv.GetWindowTextLength() > len || m_maxv.GetWindowTextLength() > len )
	{
		AfxMessageBox("max input length = 9");
		return;
	}

	CGPSDlg::gpsDlg->m_logFlashInfo.datalog_enable = m_logenable.GetCurSel();
	if(_V8_SUPPORT)
	{
		CGPSDlg::gpsDlg->m_logFlashInfo.fifo_mode = m_fifomode.GetCurSel();
	}
	OnOK();
}
