// Login.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Login.h"


// CLogin 對話方塊

IMPLEMENT_DYNAMIC(CLogin, CDialog)
CLogin::CLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CLogin::IDD, pParent)
{
}

CLogin::~CLogin()
{
}

void CLogin::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX,IDC_LOGIN_PASSWORD,password);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogin, CDialog)
END_MESSAGE_MAP()


// CLogin 訊息處理常式
