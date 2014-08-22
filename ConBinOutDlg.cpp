// ConBinOutDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConBinOutDlg.h"
#include "GPSDlg.h"

CConBinOutDlg* pCBODlg;
bool nmea,sat,pvt,ecef;
UINT CigBOThread(LPVOID pParam)
{

	U08 messages[10];  
    int i;	
	memset(messages, 0, 10);
	messages[0]  = (U08)0xa0;
	messages[1]  = (U08)0xa1;
	messages[2]  = 0;
	messages[3]  = 3;
	messages[4]  = 0x2c; //msgid
    messages[5]  = 0x0;
	messages[6]  = 0x0;		
	if(nmea)messages[6] |= (1<<3);
	if(sat) messages[6] |= (1<<2);
	if(pvt) messages[6] |= (1<<1);	
	if(ecef)messages[6] |= (1<<0);	
	U08 checksum = 0;
	for(i=0;i<(int)messages[3];i++)
		checksum^=messages[i+4];		
	messages[7] = checksum;
	messages[8] = (U08)0x0d;
	messages[9] = (U08)0x0a;
	//	for(int i=0;i<10;i++)	_cprintf("%x ",messages[i]);		
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 10, "Configure Binary Successful...");
/*
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(messages, 10,"Configure Binary Successful...");	
	CGPSDlg::gpsDlg->SetMode(); 
	CGPSDlg::gpsDlg->CreateGPSThread();
//	AfxEndThread(0);
*/
	return 0;

}

// CConBinOutDlg 對話方塊

IMPLEMENT_DYNAMIC(CConBinOutDlg, CDialog)
CConBinOutDlg::CConBinOutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConBinOutDlg::IDD, pParent)
{
}

CConBinOutDlg::~CConBinOutDlg()
{
}

void CConBinOutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_ecef);
	DDX_Control(pDX, IDC_CHECK2, m_pvt);
	DDX_Control(pDX, IDC_CHECK3, m_sat);
	DDX_Control(pDX, IDC_CHECK4, m_meas);
}


BEGIN_MESSAGE_MAP(CConBinOutDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CConBinOutDlg 訊息處理常式

BOOL CConBinOutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	pCBODlg=this;
	m_ecef.SetCheck(BST_UNCHECKED);  
	m_pvt.SetCheck(BST_CHECKED);   
	m_sat.SetCheck(BST_CHECKED);   
	m_meas.SetCheck(BST_CHECKED); 
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CConBinOutDlg::OnBnClickedOk()
{
	nmea = 0!=m_meas.GetCheck();
	sat  = 0!=m_sat.GetCheck();
	pvt  = 0!=m_pvt.GetCheck();
	ecef = 0!=m_ecef.GetCheck();
	AfxBeginThread(CigBOThread, 0);
	OnOK();
}
