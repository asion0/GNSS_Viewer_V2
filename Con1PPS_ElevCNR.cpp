// Con1PPS_ElevCNR.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con1PPS_ElevCNR.h"

// CCon1PPS_ElevCNR 對話方塊

IMPLEMENT_DYNAMIC(CCon1PPS_ElevCNR, CDialog)
CCon1PPS_ElevCNR::CCon1PPS_ElevCNR(CWnd* pParent /*=NULL*/)
	: CDialog(CCon1PPS_ElevCNR::IDD, pParent)
{
	elev = 5;
	cnr = 0;
}

CCon1PPS_ElevCNR::~CCon1PPS_ElevCNR()
{
}

void CCon1PPS_ElevCNR::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_RADIO_ELEV_AND_CNR,m_bboth);
	DDX_Control(pDX,IDC_RADIO_ELEV,m_belev);
	DDX_Control(pDX,IDC_RADIO_CNR,m_bcnr);
	DDX_Control(pDX,IDC_RADIO_ELEV_DIABLE,m_disable);

	DDX_Control(pDX,IDC_EDIT_ELEV,m_elev);
	DDX_Control(pDX,IDC_EDIT_CNR,m_cnr);

	DDX_Control(pDX,IDC_1PPS_ELEV_ATTRI,m_attr);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCon1PPS_ElevCNR, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_ELEV_AND_CNR, OnBnClickedRadioElevAndCnr)
	ON_BN_CLICKED(IDC_RADIO_ELEV, OnBnClickedRadioElev)
	ON_BN_CLICKED(IDC_RADIO_CNR, OnBnClickedRadioCnr)
	ON_BN_CLICKED(IDC_RADIO_ELEV_DIABLE, OnBnClickedRadioElevDiable)
END_MESSAGE_MAP()


// CCon1PPS_ElevCNR 訊息處理常式

void CCon1PPS_ElevCNR::OnBnClickedOk()
{
	CString tmp;


	if (m_bboth.GetCheck())
	{
		sel = 1;
		m_elev.GetWindowText(tmp);
		elev = atoi(tmp);

		m_cnr.GetWindowText(tmp);
		cnr = atoi(tmp);
	}else if (m_belev.GetCheck())
	{
		sel = 2;
		m_elev.GetWindowText(tmp);
		elev = atoi(tmp);
	}else if (m_bcnr.GetCheck())
	{
		sel = 3;
		m_cnr.GetWindowText(tmp);
		cnr = atoi(tmp);
	}else
		sel = 0;

	attr = m_attr.GetCurSel();

	OnOK();
}

BOOL CCon1PPS_ElevCNR::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_attr.SetCurSel(0);
	m_bboth.SetCheck(1);
	m_belev.SetCheck(0);
	m_bcnr.SetCheck(0);
	m_disable.SetCheck(0);


	OnBnClickedRadioElevAndCnr();

	m_elev.SetWindowText("5");
	m_cnr.SetWindowText("0");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CCon1PPS_ElevCNR::OnBnClickedRadioElevAndCnr()
{
	m_elev.EnableWindow(1);
	m_cnr.EnableWindow(1);
}

void CCon1PPS_ElevCNR::OnBnClickedRadioElev()
{
	m_elev.EnableWindow(1);
	m_cnr.EnableWindow(0);
}

void CCon1PPS_ElevCNR::OnBnClickedRadioCnr()
{
	m_elev.EnableWindow(0);
	m_cnr.EnableWindow(1);
}

void CCon1PPS_ElevCNR::OnBnClickedRadioElevDiable()
{
	m_elev.EnableWindow(0);
	m_cnr.EnableWindow(0);
}
