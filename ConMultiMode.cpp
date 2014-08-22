// ConMultiMode.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConMultiMode.h"


// CConMultiMode 對話方塊

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


// CConMultiMode 訊息處理常式

void CConMultiMode::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	
	if(((CButton*)GetDlgItem(IDC_BUILD_IN))->GetCheck())
	{
		mode = m_mode.GetCurSel();
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

	// TODO:  在此加入額外的初始化
	m_mode.Clear();
	((CButton*)GetDlgItem(IDC_BUILD_IN))->SetCheck(1);
	OnBnClickedBuildIn();
	if(!IS_DEBUG)
	{
		GetDlgItem(IDC_BUILD_IN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CUSTOM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CUSTOM_NAV)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CUSTOM_T)->ShowWindow(SW_HIDE);

/*		CRect rc1, rc2;
		GetDlgItem(IDC_MULTI_MODE)->GetClientRect(rc1);
		GetDlgItem(IDC_BUILD_IN)->GetClientRect(rc2);
		rc1.top = rc2.top;
		GetDlgItem(IDC_MULTI_MODE)->MoveWindow(rc1);	*/	
	}

	m_mode.AddString("Auto");
	m_mode.AddString("Pedestrian");
	m_mode.AddString("Car");
	m_mode.AddString("Marine");
	m_mode.AddString("Ballon");
	m_mode.AddString("Airborne");
	m_mode.AddString("Surveying and mapping");
	GetDlgItem(IDC_CUSTOM_NAV)->SetWindowText("0");

	m_mode.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
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
