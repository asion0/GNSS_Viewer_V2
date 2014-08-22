// SoftImDwDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "InputPasswordDlg.h"
#include "resource.h"

// CInputPassword 對話方塊

IMPLEMENT_DYNAMIC(CInputPassword, CDialog)
CInputPassword::CInputPassword(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PASSWD, pParent)
{
}

CInputPassword::~CInputPassword()
{
}

void CInputPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_BUTTON1, m_close);
}


BEGIN_MESSAGE_MAP(CInputPassword, CDialog)
END_MESSAGE_MAP()


// CInputPassword 訊息處理常式

BOOL CInputPassword::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT1)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CInputPassword::OnCancel()
{	
	CDialog::OnCancel();
}
void CInputPassword::OnOK()
{	
	GetDlgItem(IDC_EDIT1)->GetWindowText(password);
	CDialog::OnOK();
}