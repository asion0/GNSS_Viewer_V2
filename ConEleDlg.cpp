// ConEleDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConEleDlg.h"
#include "GPSDlg.h"

CConEleDlg* pCEDlg;
U08 ele ;

UINT CigEleThread(LPVOID pParam)
{	

	U08 messages[9];  
    int i;	    
	memset(messages, 0, 9);
	messages[0]  = (U08)0xa0;
	messages[1]  = (U08)0xa1;
	messages[2]  = 0;
	messages[3]  = 2;
	messages[4]  = 0x2b; //msgid			
	messages[5]  = ele;
	U08 checksum = 0;
	for(i=0;i<(int)messages[3];i++)
		checksum^=messages[i+4];		
	messages[6] = checksum;
	messages[7] = (U08)0x0d;
	messages[8] = (U08)0x0a;
	//for(int i=0;i<9;i++)	_cprintf("%x ",messages[i]);	
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 9, "Configure Elevation Successful...");
/*
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(messages, 9,"Configure Elevation Successful...");		
	CGPSDlg::gpsDlg->SetMode(); 
	CGPSDlg::gpsDlg->CreateGPSThread();
//	AfxEndThread(0);
*/

	return 0;
}


// CConEleDlg 對話方塊

IMPLEMENT_DYNAMIC(CConEleDlg, CDialog)
CConEleDlg::CConEleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConEleDlg::IDD, pParent)
{
}

CConEleDlg::~CConEleDlg()
{
}

void CConEleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_eele);
	DDX_Control(pDX, IDC_SPIN1, m_spele);
	DDX_Control(pDX, IDC_SLIDER1, m_slele);
}


BEGIN_MESSAGE_MAP(CConEleDlg, CDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CConEleDlg 訊息處理常式

BOOL CConEleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	pCEDlg=this;
	m_slele.SetRange(0,89);
	m_slele.SetTicFreq(10);
	m_spele.SetRange(0,89);
	m_spele.SetPos(0);	
	m_eele.SetWindowText("0");

	// TODO:  在此加入額外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CConEleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( pScrollBar )
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;		
		char text[20];
		sprintf_s(text, sizeof(text),"%d",pSlider->GetPos());
		int pos = pSlider->GetPos();		
		if(pSlider->m_hWnd==m_slele.m_hWnd)
		{
			m_eele.SetWindowText(text);m_spele.SetPos(pos);
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CConEleDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( pScrollBar )
	{
		CSpinButtonCtrl* pSlider = (CSpinButtonCtrl*)pScrollBar;			
		char text[20];
		sprintf_s(text, sizeof(text),"%d",nPos);//pSlider->GetPos());		
		if(pSlider->m_hWnd==m_spele.m_hWnd)
		{
			m_eele.SetWindowText(text);m_slele.SetPos(nPos);
		}
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CConEleDlg::OnBnClickedOk()
{
	ele = m_slele.GetPos();
	AfxBeginThread(CigEleThread,0);
	OnOK();
}
