// ConMultiMode.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "ConMultiMode.h"

// CConMultiMode 
IMPLEMENT_DYNAMIC(CConMultiMode, CDialog)
CConMultiMode::CConMultiMode(CWnd* pParent /*=NULL*/)
	: CDialog(CConMultiMode::IDD, pParent)
{
	mode = 0;
	attribute = 0;
}

CConMultiMode::~CConMultiMode()
{
}

void CConMultiMode::DoDataExchange(CDataExchange* pDX)
{
//	DDX_CBIndex(pDX,IDC_MULTI_MODE,mode);
	DDX_CBIndex(pDX,IDC_MULTIMODE_ATTR,attribute);

	DDX_Control(pDX,IDC_MULTI_MODE, m_mode);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConMultiMode, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CUSTOM, &CConMultiMode::OnBnClickedCustom)
	ON_BN_CLICKED(IDC_BUILD_IN, &CConMultiMode::OnBnClickedBuildIn)
END_MESSAGE_MAP()


// CConMultiMode 

void CConMultiMode::OnBnClickedOk()
{

	if(((CButton*)GetDlgItem(IDC_BUILD_IN))->GetCheck())
	{
		mode = m_mode.GetCurSel();
    CString txt;
    m_mode.GetWindowText(txt);
    if(txt == "Quadcopter")
    {
      mode = 7;
    }
	}
	else	
	{
		CString s;
		GetDlgItem(IDC_CUSTOM_NAV)->GetWindowText(s);
		mode = atoi(s);
	}
	OnOK();
}

BOOL CConMultiMode::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_mode.Clear();
	((CButton*)GetDlgItem(IDC_BUILD_IN))->SetCheck(1);
	OnBnClickedBuildIn();
	if(!IS_DEBUG)
	{
		GetDlgItem(IDC_BUILD_IN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CUSTOM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CUSTOM_NAV)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CUSTOM_T)->ShowWindow(SW_HIDE);
	}

	m_mode.AddString("Auto");
	m_mode.AddString("Pedestrian");
	m_mode.AddString("Car");
	m_mode.AddString("Marine");
	m_mode.AddString("Ballon");
	m_mode.AddString("Airborne");
  //20160801 Remove this item, requast from Andrew and Terrance
	//m_mode.AddString("Surveying and mapping");
  //20160808 Add new mode but skip index 6, requast from Terrance
	m_mode.AddString("Quadcopter");
	GetDlgItem(IDC_CUSTOM_NAV)->SetWindowText("0");

	m_mode.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConMultiMode::OnBnClickedCustom()
{
	GetDlgItem(IDC_MULTI_MODE)->EnableWindow(FALSE);

	GetDlgItem(IDC_CUSTOM_NAV)->EnableWindow(TRUE);
	GetDlgItem(IDC_CUSTOM_T)->EnableWindow(TRUE);
}

void CConMultiMode::OnBnClickedBuildIn()
{
	GetDlgItem(IDC_MULTI_MODE)->EnableWindow(TRUE);

	GetDlgItem(IDC_CUSTOM_NAV)->EnableWindow(FALSE);
	GetDlgItem(IDC_CUSTOM_T)->EnableWindow(FALSE);
}
