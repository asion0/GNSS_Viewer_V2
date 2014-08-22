// Con1PPS_Timing.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con1PPS_Timing.h"


// CCon1PPS_Timing 對話方塊

IMPLEMENT_DYNAMIC(CCon1PPS_Timing, CDialog)
CCon1PPS_Timing::CCon1PPS_Timing(CWnd* pParent /*=NULL*/)
	: CDialog(CCon1PPS_Timing::IDD, pParent)
{

	memset(&_1pps_timing,0,sizeof(_1PPS_Timing_T));

	_1pps_timing.Survey_Length = 2000;
	_1pps_timing.Standard_deviation = 30;
}

CCon1PPS_Timing::~CCon1PPS_Timing()
{
}

void CCon1PPS_Timing::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_1PPS_MODE,m_combo_mode);
	DDX_Control(pDX,IDC_1PPS_SET1,m_set1);
	DDX_Control(pDX,IDC_1PPS_SET2,m_set2);
	DDX_Control(pDX,IDC_1PPS_SET3,m_set3);
	DDX_Control(pDX,IDC_STATIC_SET1,m_lbl_set1);
	DDX_Control(pDX,IDC_STATIC_SET2,m_lbl_set2);
	DDX_Control(pDX,IDC_STATIC_SET3,m_lbl_set3);

	DDX_Control(pDX,IDC_1PPS_ATTRI,m_attri);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCon1PPS_Timing, CDialog)
	ON_CBN_SELCHANGE(IDC_1PPS_MODE, OnCbnSelchange1ppsMode)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CCon1PPS_Timing 訊息處理常式

BOOL CCon1PPS_Timing::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_combo_mode.SetCurSel(0);
	OnCbnSelchange1ppsMode();
	m_attri.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CCon1PPS_Timing::OnCbnSelchange1ppsMode()
{
	int sel_index = m_combo_mode.GetCurSel();
	CString temp;

	if (sel_index==0)
	{
		m_lbl_set1.ShowWindow(0);
		m_lbl_set2.ShowWindow(0);
		m_lbl_set3.ShowWindow(0);

		m_set1.ShowWindow(0);
		m_set2.ShowWindow(0);
		m_set3.ShowWindow(0);

	}else if (sel_index==1)
	{
		m_lbl_set1.ShowWindow(1);
	if(LG || TIMING_MODE)
		m_lbl_set2.ShowWindow(1);
	else	
		m_lbl_set2.ShowWindow(0);

		m_lbl_set3.ShowWindow(0);

		m_set1.ShowWindow(1);
		if (LG || TIMING_MODE)
		{
		m_set2.ShowWindow(1);
		}
		else
		{
		m_set2.ShowWindow(0);
		}
		m_set3.ShowWindow(0);
		m_lbl_set1.SetWindowText("Survey Length");
		temp.Format("%d",_1pps_timing.Survey_Length);
		m_set1.SetWindowText(temp);
#if (LG) || (TIMING_MODE)
		m_lbl_set2.SetWindowText("Standard deviation");
		temp.Format("%d",_1pps_timing.Standard_deviation);
		m_set2.SetWindowText(temp);
        #endif
	}else if (sel_index==2)
	{
		m_lbl_set1.ShowWindow(1);
		m_lbl_set2.ShowWindow(1);
		m_lbl_set3.ShowWindow(1);
		m_set1.ShowWindow(1);
		m_set2.ShowWindow(1);
		m_set3.ShowWindow(1);

		m_lbl_set1.SetWindowText("Latitude");
		m_lbl_set2.SetWindowText("Longitude");
		m_lbl_set3.SetWindowText("Altitude");
		temp.Format("%f",_1pps_timing.latitude);
		m_set1.SetWindowText(temp);
		temp.Format("%f",_1pps_timing.longitude);
		m_set2.SetWindowText(temp);
		temp.Format("%f",_1pps_timing.altitude);
		m_set3.SetWindowText(temp);
	}
}



void CCon1PPS_Timing::OnBnClickedOk()
{
	CString temp;

	_1pps_timing.Timing_mode = m_combo_mode.GetCurSel();
	if (_1pps_timing.Timing_mode==1)
	{
		m_set1.GetWindowText(temp);
		_1pps_timing.Survey_Length = atoi(temp);
#if (LG) || (TIMING_MODE)
		m_set2.GetWindowText(temp);
		_1pps_timing.Standard_deviation = atoi(temp);
#endif
	}else if (_1pps_timing.Timing_mode==2)
	{
		m_set1.GetWindowText(temp);
		_1pps_timing.latitude = atof(temp);
		m_set2.GetWindowText(temp);
		_1pps_timing.longitude = atof(temp);
		m_set3.GetWindowText(temp);
		_1pps_timing.altitude = (F32)atof(temp);
	}
	
	_1pps_timing.attributes = m_attri.GetCurSel();

	OnOK();
}
