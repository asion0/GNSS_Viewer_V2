// Con1PPS_DOP.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con1PPS_DOP.h"


// CCon1PPS_DOP 對話方塊

IMPLEMENT_DYNAMIC(CCon1PPS_DOP, CDialog)
CCon1PPS_DOP::CCon1PPS_DOP(CWnd* pParent /*=NULL*/)
	: CDialog(CCon1PPS_DOP::IDD, pParent)
{
	dop_sel ;
	pdop = hdop = gdop = 10;
}

CCon1PPS_DOP::~CCon1PPS_DOP()
{
}

void CCon1PPS_DOP::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_RADIO_DOP_AUTO,m_dop_auto);
	DDX_Control(pDX,IDC_RADIO_DOP_PDOP,m_dop_pdop);
	DDX_Control(pDX,IDC_RADIO_DOP_HDOP,m_dop_hdop);
	DDX_Control(pDX,IDC_RADIO_DOP_GDOP,m_dop_gdop);
	DDX_Control(pDX,IDC_RADIO_DOP_DIABLE,m_dop_disable);

	DDX_Control(pDX,IDC_EDIT_DOP_PDOP,m_pdop);
	DDX_Control(pDX,IDC_EDIT_DOP_HDOP,m_hdop);
	DDX_Control(pDX,IDC_EDIT_DOP_GDOP,m_gdop);

	DDX_Control(pDX,IDC_1PPS_DOP_ATTRI,m_attr);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCon1PPS_DOP, CDialog)
	ON_BN_CLICKED(IDC_RADIO_DOP_AUTO, OnBnClickedRadioDopAuto)
	ON_BN_CLICKED(IDC_RADIO_DOP_PDOP, OnBnClickedRadioDopPdop)
	ON_BN_CLICKED(IDC_RADIO_DOP_HDOP, OnBnClickedRadioDopHdop)
	ON_BN_CLICKED(IDC_RADIO_DOP_GDOP, OnBnClickedRadioDopGdop)
	ON_BN_CLICKED(IDC_RADIO_DOP_DIABLE, OnBnClickedRadioDopDiable)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CCon1PPS_DOP 訊息處理常式

void CCon1PPS_DOP::OnBnClickedRadioDopAuto()
{
	m_pdop.EnableWindow(1);
	m_hdop.EnableWindow(1);
	m_gdop.EnableWindow(0);
}

void CCon1PPS_DOP::OnBnClickedRadioDopPdop()
{
	m_pdop.EnableWindow(1);
	m_hdop.EnableWindow(0);
	m_gdop.EnableWindow(0);
}

void CCon1PPS_DOP::OnBnClickedRadioDopHdop()
{
	m_pdop.EnableWindow(0);
	m_hdop.EnableWindow(1);
	m_gdop.EnableWindow(0);
}

void CCon1PPS_DOP::OnBnClickedRadioDopGdop()
{
	m_pdop.EnableWindow(0);
	m_hdop.EnableWindow(0);
	m_gdop.EnableWindow(1);
}

void CCon1PPS_DOP::OnBnClickedRadioDopDiable()
{
	m_pdop.EnableWindow(0);
	m_hdop.EnableWindow(0);
	m_gdop.EnableWindow(0);
}

BOOL CCon1PPS_DOP::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	m_attr.SetCurSel(0);
	m_dop_auto.SetCheck(1);
	m_dop_pdop.SetCheck(0);
	m_dop_hdop.SetCheck(0);
	m_dop_gdop.SetCheck(0);
	m_dop_disable.SetCheck(0);

	OnBnClickedRadioDopAuto();

	m_pdop.SetWindowText("10");
	m_hdop.SetWindowText("10");
	m_gdop.SetWindowText("10");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}



void CCon1PPS_DOP::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CString tmp;
	

	if (m_dop_auto.GetCheck())
	{
		dop_sel = 1;
		m_pdop.GetWindowText(tmp);
		pdop = (float)atof(tmp);

		m_hdop.GetWindowText(tmp);
		hdop = (float)atof(tmp);
	}
	else if (m_dop_pdop.GetCheck())
	{
		dop_sel = 2;
		m_pdop.GetWindowText(tmp);
		pdop = (float)atof(tmp);
	}
	else if (m_dop_hdop.GetCheck())
	{
		dop_sel = 3;
		m_hdop.GetWindowText(tmp);
		hdop = (float)atof(tmp);
	}
	else if (m_dop_gdop.GetCheck())
	{
		dop_sel = 4;
		m_gdop.GetWindowText(tmp);
		gdop = (float)atof(tmp);
	}
	else
	{
		dop_sel = 0;
	}

	attr = m_attr.GetCurSel();

	OnOK();
}
