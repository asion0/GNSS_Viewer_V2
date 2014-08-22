// CfgMsg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "CfgMsg.h"
#include "GPSDlg.h"

UINT CigMsgThread(LPVOID pParam)
{	

	U08 messages[10];  
    int i;	
	memset(messages, 0, 10);
	messages[0]  = (U08)0xa0;
	messages[1]  = (U08)0xa1;
	messages[2]  = 0;
	messages[3]  = 3;
	messages[4]  = msgid;//0x9; //msgid
    messages[5]  = type;
	messages[6]  = attribute;	
	U08 checksum = 0;
	for(i=0;i<(int)messages[3];i++)
		checksum^=messages[i+4];		
	messages[7] = checksum;
	messages[8] = (U08)0x0d;
	messages[9] = (U08)0x0a;
	//	for(int i=0;i<10;i++)	_cprintf("%x ",messages[i]);	
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 10, "Configure Message Successful...");

	/*
	CGPSDlg::gpsDlg->SendToTarget(messages, 10,"Configure Message Successful...");	
	CGPSDlg::gpsDlg->SetMode(); 
	CGPSDlg::gpsDlg->CreateGPSThread();
	*/
//	AfxEndThread(0);
	return 0;

}

// CCfgMsg 對話方塊

IMPLEMENT_DYNAMIC(CCfgMsg, CDialog)
CCfgMsg::CCfgMsg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfgMsg::IDD, pParent)
{
}

CCfgMsg::~CCfgMsg()
{
}

void CCfgMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_type);
	DDX_Control(pDX, IDC_COMBO2, m_attribute);
}


BEGIN_MESSAGE_MAP(CCfgMsg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CCfgMsg 訊息處理常式

BOOL CCfgMsg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_type.AddString("No output");
	m_type.AddString("NMEA Message");
	m_type.AddString("Binary Message");
#if PACIFIC
	m_type.AddString("Binary Message(Pacific Telescope defined)");
#endif
	m_attribute.AddString("Update to SRAM");
	m_attribute.AddString("Update to SRAM+FLASH");

	m_type.SetCurSel(1);
	m_attribute.SetCurSel(0);


	msgid = msg_id;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CCfgMsg::OnBnClickedOk()
{
	type = m_type.GetCurSel();
	switch(type)
	{
	case 0:
		CGPSDlg::gpsDlg->SetMsgType(CGPSDlg::Nooutput_Mode);
		break;
	case 1:
		CGPSDlg::gpsDlg->SetMsgType(CGPSDlg::NMEA_Mode);
		break;
	case 2:
		CGPSDlg::gpsDlg->SetMsgType(CGPSDlg::Binary_Mode); 
		break;
	case 3:
		CGPSDlg::gpsDlg->SetMsgType(CGPSDlg::Binary_Pacific);
	default:
		break;
	}
	attribute = m_attribute.GetCurSel();
	AfxBeginThread(CigMsgThread,0);
	OnOK();
}
