// Agps_config.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Agps_config.h"


// CAgps_config 對話方塊

IMPLEMENT_DYNAMIC(CAgps_config, CDialog)
CAgps_config::CAgps_config(CWnd* pParent /*=NULL*/)
	: CDialog(CAgps_config::IDD, pParent)
{
	enable = false ;
	attribute = 0;
}

CAgps_config::~CAgps_config()
{
}

void CAgps_config::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_COMBO_STATUS,cbo_status);
	DDX_CBIndex(pDX,IDC_AGPS_ATTR,attribute);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAgps_config, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CAgps_config 訊息處理常式

void CAgps_config::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	enable = cbo_status.GetCurSel();
	OnOK();
}

BOOL CAgps_config::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	cbo_status.SetCurSel(enable);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CAgps_config::Set_Enable(U08 m_enable)
{
	enable = m_enable;
	//cbo_status.SetCurSel(enable);
}
