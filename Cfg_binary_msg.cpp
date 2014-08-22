// Cfg_binary_msg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Cfg_binary_msg.h"


// CCfg_binary_msg 對話方塊

IMPLEMENT_DYNAMIC(CCfg_binary_msg, CDialog)
CCfg_binary_msg::CCfg_binary_msg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfg_binary_msg::IDD, pParent)
{
}

CCfg_binary_msg::~CCfg_binary_msg()
{
}

void CCfg_binary_msg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_CFGBINARY_RATE, m_rate);
	DDX_Control(pDX, IDC_CFGBINARY_MT, m_mt);
	DDX_Control(pDX, IDC_CFGBINARY_RM, m_rm);
	DDX_Control(pDX, IDC_CFGBINARY_SC, m_sc);
	DDX_Control(pDX, IDC_CFGBINARY_RS, m_rs);
	DDX_Control(pDX, IDC_CFGBINARY_SUB, m_sub);
	DDX_Control(pDX, IDC_CFGBINARY_GLONASS, m_gsb);
	DDX_Control(pDX, IDC_CFGBINARY_ATT, m_attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCfg_binary_msg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CCfg_binary_msg 訊息處理常式

void CCfg_binary_msg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_rate_sel = m_rate.GetCurSel();
	m_mt_sel = m_mt.GetCurSel();
	m_rm_sel = m_rm.GetCurSel();
	m_sc_sel = m_sc.GetCurSel();
	m_rs_sel = m_rs.GetCurSel();
	m_sub_sel = m_sub.GetCurSel();
	m_gsb_sel = m_gsb.GetCurSel();
	m_attr_sel = m_attr.GetCurSel();
	OnOK();
}

void CCfg_binary_msg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CCfg_binary_msg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{


	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CCfg_binary_msg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_rate.AddString("1 Hz");
	m_rate.AddString("2 Hz");
	m_rate.AddString("4 Hz");
	m_rate.AddString("5 Hz");
	m_rate.AddString("10 Hz");
	m_rate.AddString("20 Hz");
	m_rate.SetCurSel(0);
	
	m_mt.AddString("Disable");
	m_mt.AddString("Enable");
	m_mt.SetCurSel(1);

	m_rm.AddString("Disable");
	m_rm.AddString("Enable");
	m_rm.SetCurSel(1);

	m_sc.AddString("Disable");
	m_sc.AddString("Enable");
	m_sc.SetCurSel(1);

	m_rs.AddString("Disable");
	m_rs.AddString("Enable");
	m_rs.SetCurSel(1);

	m_sub.AddString("Disable");
	m_sub.AddString("Enable");
	m_sub.SetCurSel(1);
	
	m_gsb.AddString("Disable");
	m_gsb.AddString("Enable");
	m_gsb.SetCurSel(1);

	m_attr.SetCurSel(0);

	// TODO:  在此加入額外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
