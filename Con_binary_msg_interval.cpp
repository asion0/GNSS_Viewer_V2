// Con_binary_msg_interval.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"
#include "Con_binary_msg_interval.h"


U08 interval;
U08 interval_type;

UINT Config_binary_Thread(LPVOID pParam)
{

	U08 messages[10];   
	int i;	      
	memset(messages, 0, 10);
	messages[0]  = (U08)0xa0;
	messages[1]  = (U08)0xa1;
	messages[2]  = 0;
	messages[3]  = 3;
	messages[4]  = 0xf; //msgid		
	messages[5]  = interval;	
	messages[6]  = interval_type;

	U08 checksum = 0;
	for(i=0;i<(int)messages[3];i++)
	{checksum^=messages[i+4];}
	messages[7] = checksum;
	messages[8] = (U08)0x0d;
	messages[9] = (U08)0x0a;
	//	for(int i=0;i<15;i++)	_cprintf("%x ",messages[i]);
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 16, "Configure Binary Message Interval Successful...");
/*
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(messages, 16, "Configure Binary Message Interval Successful...");		
	CGPSDlg::gpsDlg->SetMode(); 
	CGPSDlg::gpsDlg->CreateGPSThread();
//	AfxEndThread(0);
*/
	return 0;
}

// CCon_binary_msg_interval 對話方塊

IMPLEMENT_DYNAMIC(CCon_binary_msg_interval, CDialog)
CCon_binary_msg_interval::CCon_binary_msg_interval(CWnd* pParent /*=NULL*/)
	: CDialog(CCon_binary_msg_interval::IDD, pParent)
{
}

CCon_binary_msg_interval::~CCon_binary_msg_interval()
{
}

void CCon_binary_msg_interval::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_SLIDER1,m_sliderctrl);
	DDX_Control(pDX,IDC_EDIT1,m_val);
	DDX_Control(pDX,IDC_SPIN2,m_spin);
	DDX_Control(pDX,IDC_CFGNMEA_ATT,m_cbo);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCon_binary_msg_interval, CDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CCon_binary_msg_interval 訊息處理常式

void CCon_binary_msg_interval::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;	
	
	if(pSlider->m_hWnd == m_sliderctrl.m_hWnd)
	{
		char text[20];
		sprintf_s(text, sizeof(text),"%d",m_sliderctrl.GetPos());
		m_val.SetWindowText(text);
		m_spin.SetPos(m_sliderctrl.GetPos());
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
void CCon_binary_msg_interval::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (pScrollBar)
	{


		CSpinButtonCtrl* pSlider = (CSpinButtonCtrl*)pScrollBar;	

		if(pSlider->m_hWnd == m_spin.m_hWnd)
		{
			char text[20];
			sprintf_s(text, sizeof(text),"%d",nPos);
			m_val.SetWindowText(text);
			m_sliderctrl.SetPos(nPos);
		}
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CCon_binary_msg_interval::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	m_sliderctrl.SetRange(0,255);
	m_sliderctrl.SetPos(1);
	m_spin.SetRange(0,255);
	m_spin.SetPos(1);
	m_sliderctrl.SetTicFreq(15);

	m_val.SetWindowText("1");

	m_cbo.AddString("Update to SRAM");
	m_cbo.AddString("Update to SRAM+FLASH");
	m_cbo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}



void CCon_binary_msg_interval::OnBnClickedOk()
{
	interval = m_sliderctrl.GetPos();	
	interval_type = m_cbo.GetCurSel();
	AfxBeginThread(Config_binary_Thread,0);
	OnOK();
}
