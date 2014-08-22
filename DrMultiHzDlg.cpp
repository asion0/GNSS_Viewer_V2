// PositionRateDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "DrMultiHzDlg.h"


// DrMultiHzDlg 對話方塊

IMPLEMENT_DYNAMIC(DrMultiHzDlg, CDialog)
DrMultiHzDlg::DrMultiHzDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CONFIG_DRUPDATERATE, pParent)
{
}

DrMultiHzDlg::~DrMultiHzDlg()
{
}

void DrMultiHzDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_COMBO1,cbo_rate);
	DDX_Control(pDX,IDC_PR_ATTR,cbo_attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DrMultiHzDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// DrMultiHzDlg 訊息處理常式

void DrMultiHzDlg::OnBnClickedOk()
{
	CString tmp;
	
	cbo_rate.GetLBText(cbo_rate.GetCurSel(),tmp);
	rate = atoi(tmp);
	
	attr = cbo_attr.GetCurSel();

	OnOK();
}

BOOL DrMultiHzDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	cbo_rate.SetCurSel(0);
	cbo_attr.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
