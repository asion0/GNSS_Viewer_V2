// Con_register.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con_register.h"


// CCon_register 對話方塊

IMPLEMENT_DYNAMIC(CCon_register, CDialog)
CCon_register::CCon_register(CWnd* pParent /*=NULL*/)
	: CDialog(CCon_register::IDD, pParent)
{
}

CCon_register::~CCon_register()
{
}

void CCon_register::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_REG_ADDR,m_txt_addr);
	DDX_Control(pDX,IDC_REG_DATA,m_txt_data);

	DDX_Text(pDX,IDC_REG_ADDR,m_addr);
	DDX_Text(pDX,IDC_REG_DATA,m_data);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCon_register, CDialog)
	ON_EN_UPDATE(IDC_REG_ADDR, OnEnUpdateRegAddr)
END_MESSAGE_MAP()


// CCon_register 訊息處理常式

BOOL CCon_register::OnInitDialog()
{
	CString tmp;

	CDialog::OnInitDialog();

	m_txt_addr.SetLimitText(8);
	m_txt_data.SetLimitText(8);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CCon_register::OnEnUpdateRegAddr()
{
	// TODO:  如果這是 RICHEDIT 控制項，控制項將不會
	// 傳送此告知，除非您覆寫 CDialog::OnInitDialog()
	// 要傳送 EM_SETEVENTMASK 訊息到控制項的函式
	// 將具有 ENM_UPDATE 旗標 ORed 加入 lParam 遮罩。

	// TODO:  在此加入控制項告知處理常式程式碼

	//CString tmp;
	//m_txt_addr.GetWindowText(tmp);
	//if (tmp.GetLength() > 8)
	//	tmp = tmp.Left(8);
	//m_txt_addr.SetWindowText(tmp);
	//m_txt_addr.SetLimitText
}	
