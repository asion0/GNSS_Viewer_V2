// SetFilterDlg.cpp
//

#include "stdafx.h"
#include "GPS.h"
#include "SetFilterDlg.h"
#include "CompressDlg.h"

extern CCompressDlg *pComDlg;

// CSetFilterDlg

IMPLEMENT_DYNAMIC(CSetFilterDlg, CDialog)
CSetFilterDlg::CSetFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetFilterDlg::IDD, pParent)
{
}

CSetFilterDlg::~CSetFilterDlg()
{
}

void CSetFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tmin);
	DDX_Control(pDX, IDC_EDIT2, m_tmax);
	DDX_Control(pDX, IDC_EDIT3, m_dmin);
	DDX_Control(pDX, IDC_EDIT4, m_dmax);
	DDX_Control(pDX, IDC_EDIT5, m_vmin);
	DDX_Control(pDX, IDC_EDIT6, m_vmax);
}


BEGIN_MESSAGE_MAP(CSetFilterDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSetFilterDlg 

void CSetFilterDlg::OnBnClickedOk()
{
	CString  buf;
	CString& temp =buf;
	char temp1[50];

	m_tmin.GetWindowText(temp);
	sprintf_s(temp1, sizeof(temp1), "%s", temp);

	pComDlg->LogFlashInfo.min_time     = atoi( temp1 );
	
	m_tmax.GetWindowText(temp);
	sprintf_s(temp1, sizeof(temp1), "%s", temp);
	pComDlg->LogFlashInfo.max_time     = atoi( temp1 );
	
	m_dmin.GetWindowText(temp);
	sprintf_s(temp1, sizeof(temp1), "%s", temp);
	pComDlg->LogFlashInfo.min_distance = atoi( temp1 );
	
	m_dmax.GetWindowText(temp);
	sprintf_s(temp1, sizeof(temp1), "%s", temp);
	pComDlg->LogFlashInfo.max_distance = atoi( temp1 );

	m_vmin.GetWindowText(temp);
	sprintf_s(temp1, sizeof(temp1), "%s", temp);
	pComDlg->LogFlashInfo.min_speed    = atoi( temp1 );
	
	m_vmax.GetWindowText(temp);
	sprintf_s(temp1, sizeof(temp1), "%s", temp);
	pComDlg->LogFlashInfo.max_speed    = atoi( temp1 );
	OnOK();
}

BOOL CSetFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char temp[100];
	sprintf_s(temp, sizeof(temp), "%d",	pComDlg->LogFlashInfo.min_time);
	m_tmin.SetWindowText( temp );
	
	sprintf_s(temp, sizeof(temp), "%d",	pComDlg->LogFlashInfo.max_time);
	m_tmax.SetWindowText( temp );
	
	sprintf_s(temp, sizeof(temp), "%d",	pComDlg->LogFlashInfo.min_distance);
	m_dmin.SetWindowText( temp );
	
	sprintf_s(temp, sizeof(temp), "%d",	pComDlg->LogFlashInfo.max_distance);
	m_dmax.SetWindowText( temp );
	
	sprintf_s(temp, sizeof(temp), "%d",	pComDlg->LogFlashInfo.min_speed);
 	m_vmin.SetWindowText( temp );
	
	sprintf_s(temp, sizeof(temp), "%d",	pComDlg->LogFlashInfo.max_speed);
	m_vmax.SetWindowText( temp );

	return TRUE;  // return TRUE unless you set the focus to a control
}
