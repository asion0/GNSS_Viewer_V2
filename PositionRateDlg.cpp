// PositionRateDlg.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "PositionRateDlg.h"


// CPositionRateDlg 

IMPLEMENT_DYNAMIC(CPositionRateDlg, CDialog)
CPositionRateDlg::CPositionRateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPositionRateDlg::IDD, pParent)
{
}

CPositionRateDlg::~CPositionRateDlg()
{
}

void CPositionRateDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_COMBO1,cbo_rate);
	DDX_Control(pDX,IDC_PR_ATTR,cbo_attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPositionRateDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CPositionRateDlg 

void CPositionRateDlg::OnBnClickedOk()
{
	CString tmp;
	
	cbo_rate.GetLBText(cbo_rate.GetCurSel(),tmp);
	rate = atoi(tmp);
	
	attr = cbo_attr.GetCurSel();

	OnOK();
}

BOOL CPositionRateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	cbo_rate.SetCurSel(0);
	cbo_attr.SetCurSel(0);

	GetDlgItem(IDC_WARNING)->SetWindowText("Warning :\r\nPlease disable SAEE before using multi-hz position update rate.");
	return TRUE;  // return TRUE unless you set the focus to a control
}
