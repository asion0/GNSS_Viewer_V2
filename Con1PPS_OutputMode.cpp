// Con1PPS_OutputMode.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con1PPS_OutputMode.h"


// CCon1PPS_OutputMode 對話方塊

IMPLEMENT_DYNAMIC(CCon1PPS_OutputMode, CDialog)

CCon1PPS_OutputMode::CCon1PPS_OutputMode(CWnd* pParent /*=NULL*/)
	: CDialog(CCon1PPS_OutputMode::IDD, pParent)
{
	mode = attr = 0;
	align_to =0;
}

CCon1PPS_OutputMode::~CCon1PPS_OutputMode()
{
}

void CCon1PPS_OutputMode::DoDataExchange(CDataExchange* pDX)
{

	//DDX_CBIndex(pDX,IDC_1PPS_MODE_ATTR,attr);
	DDX_Control(pDX,IDC_RADIO_PPS_NO_OUTPUT,m_no_output);
	DDX_Control(pDX,IDC_RADIO_PPS_OUTPUT_HAVE_GPS_TIME,m_output_gps_time);
	DDX_Control(pDX,IDC_RADIO_PPS_OUTPUT_ALIGN,m_output_align);
	DDX_Control(pDX,IDC_RADIO_PPS_OUTPUT_HAVE_GPS_TIME,m_output_gps_time);
	DDX_Control(pDX,IDC_RADIO_PPS_ALIGN_TO_GPS,m_align_to_GPS);
	DDX_Control(pDX,IDC_RADIO_PPS_ALIGN_TO_UTC,m_align_to_UTC);
	DDX_Control(pDX, IDC_1PPS_MODE_ATTR, m_attr);
	CDialog::DoDataExchange(pDX);
	
	
}


BEGIN_MESSAGE_MAP(CCon1PPS_OutputMode, CDialog)
	ON_BN_CLICKED(IDC_RADIO_PPS_NO_OUTPUT, &CCon1PPS_OutputMode::OnBnClickedRadioPpsNoOutput)
	//ON_BN_CLICKED(IDC_RADIO_PPS_OUTPUT_ALWAYS, &CCon1PPS_OutputMode::OnBnClickedRadioPpsOutputAlways)
	ON_BN_CLICKED(IDC_RADIO_PPS_OUTPUT_HAVE_GPS_TIME, &CCon1PPS_OutputMode::OnBnClickedRadioPpsOutputHaveGpsTime)
	ON_BN_CLICKED(IDC_RADIO_PPS_OUTPUT_ALIGN, &CCon1PPS_OutputMode::OnBnClickedRadioPpsOutputAlign)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_PPS_ALIGN_TO_GPS, &CCon1PPS_OutputMode::OnBnClickedRadioPpsAlignToGps)
	ON_BN_CLICKED(IDC_RADIO_PPS_ALIGN_TO_UTC, &CCon1PPS_OutputMode::OnBnClickedRadioPpsAlignToUtc)
END_MESSAGE_MAP()


BOOL CCon1PPS_OutputMode::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  在此加入額外的初始化
//GetDlgItem(IDC_RADIO_PPS_NO_OUTPUT)->SetCheck();
	m_attr.SetCurSel(0);
	m_no_output.SetCheck(0);
	m_output_gps_time.SetCheck(1);
	m_output_align.SetCheck(0);
	m_align_to_GPS.SetCheck(1);
	m_align_to_UTC.SetCheck(0);
	m_output_align.ShowWindow((TIMING_OUTPUT_ALIGN) ? SW_SHOW : SW_HIDE);




	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

// CCon1PPS_OutputMode 訊息處理常式

void CCon1PPS_OutputMode::OnBnClickedRadioPpsNoOutput()
{
	m_no_output.SetCheck(1);
	m_output_gps_time.SetCheck(0);
	m_output_align.SetCheck(0);
}


void CCon1PPS_OutputMode::OnBnClickedRadioPpsOutputHaveGpsTime()
{
	m_no_output.SetCheck(0);
	m_output_gps_time.SetCheck(1);
	m_output_align.SetCheck(0);
}

void CCon1PPS_OutputMode::OnBnClickedRadioPpsOutputAlign()
{
	m_no_output.SetCheck(0);
	m_output_gps_time.SetCheck(0);
	m_output_align.SetCheck(1);
}

void CCon1PPS_OutputMode::OnBnClickedOk()
{
	CString tmp;
	

	if (m_no_output.GetCheck())
	{
		mode = 0;
	}else if (m_output_gps_time.GetCheck())
	{
		mode = 1;
	}else if (m_output_align.GetCheck())
	{
		mode = 2;
	}else
		mode = 0;

	if (m_align_to_GPS.GetCheck())
	{
		align_to = 0;
	}else if (m_align_to_UTC.GetCheck())
	{
		align_to = 1;
	}else
		align_to = 0;

	attr = m_attr.GetCurSel();

	OnOK();
}

void CCon1PPS_OutputMode::OnBnClickedRadioPpsAlignToGps()
{
	m_align_to_GPS.SetCheck(1);
	m_align_to_UTC.SetCheck(0);
}

void CCon1PPS_OutputMode::OnBnClickedRadioPpsAlignToUtc()
{
	m_align_to_GPS.SetCheck(0);
	m_align_to_UTC.SetCheck(1);
}
