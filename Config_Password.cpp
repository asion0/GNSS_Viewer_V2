// Config_Password.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Config_Password.h"


// CConfig_Password 對話方塊

IMPLEMENT_DYNAMIC(CConfig_Password, CDialog)
CConfig_Password::CConfig_Password(CWnd* pParent /*=NULL*/)
	: CDialog(CConfig_Password::IDD, pParent)
{
	memset(old_password, 0, sizeof(old_password));
	memset(new_password, 0, sizeof(new_password));
}

CConfig_Password::~CConfig_Password()
{
}

void CConfig_Password::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_OLD_PASS, m_old);
	DDX_Control(pDX, IDC_NEW_PASS, m_new);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfig_Password, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CConfig_Password 訊息處理常式

void CConfig_Password::OnBnClickedOk()
{
	CString tmp;

	m_old.GetWindowText(tmp);
	if (tmp.GetLength() > 8 )
	{
		AfxMessageBox("Password size must <= 8...");
		return ;
	}

	m_new.GetWindowText(tmp);
	if (tmp.GetLength() > 8 )
	{
		AfxMessageBox("Password size must <= 8...");
		return ;
	}
	
	m_old.GetWindowText(old_password, 9);
	m_new.GetWindowText(new_password, 9);
	// TODO: 在此加入控制項告知處理常式程式碼
	OnOK();
}
