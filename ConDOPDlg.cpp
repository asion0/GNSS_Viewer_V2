// ConDOPDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConDOPDlg.h"
#include "GPSDlg.h"

CConDOPDlg* pCDDlg;
U08  dopsel;
U08  gdop;
U08  pdop;
U08  hdop;

UINT CigDopThread(LPVOID pParam)
{
   
	U08 messages[12];  
    int i;		     
	memset(messages, 0, 12);
	messages[0]  = (U08)0xa0;
	messages[1]  = (U08)0xa1;
	messages[2]  = 0;
	messages[3]  = 5;
	messages[4]  = 0x2a; //msgid		
	messages[5]  = dopsel;	
	messages[6]  = gdop;
	messages[7]  = pdop;
	messages[8]  = hdop;		
	U08 checksum = 0;
	for(i=0;i<(int)messages[3];i++)	checksum^=messages[i+4];		
	messages[9]  = checksum;
	messages[10] = (U08)0x0d;
	messages[11] = (U08)0x0a;
	//for(int i=0;i<12;i++)	_cprintf("%x ",messages[i]);	
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 12, "Configure DOP Successful...");
/*
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(messages, 12,"Configure DOP Successful...");		
	CGPSDlg::gpsDlg->SetMode(); 
	CGPSDlg::gpsDlg->CreateGPSThread();
//	AfxEndThread(0);*/

	return 0;
}

// CConDOPDlg 對話方塊

IMPLEMENT_DYNAMIC(CConDOPDlg, CDialog)
CConDOPDlg::CConDOPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConDOPDlg::IDD, pParent)
{
}

CConDOPDlg::~CConDOPDlg()
{
}

void CConDOPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_dopsel);
	DDX_Control(pDX, IDC_EDITGDOP, m_egdop);
	DDX_Control(pDX, IDC_EDIT2, m_epdop);
	DDX_Control(pDX, IDC_EDITHDOP, m_ehdop);
	DDX_Control(pDX, IDC_SPINGDOP, m_spgdop);
	DDX_Control(pDX, IDC_SPINPDOP, m_sppdop);
	DDX_Control(pDX, IDC_SPIN3, m_sphdop);
	DDX_Control(pDX, IDC_SLIDER1, m_slgdop);
	DDX_Control(pDX, IDC_SLIDER2, m_slpdop);
	DDX_Control(pDX, IDC_SLIDER3, m_slhdop);
}


BEGIN_MESSAGE_MAP(CConDOPDlg, CDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CConDOPDlg 訊息處理常式

void CConDOPDlg::PreInitDialog()
{
	

	CDialog::PreInitDialog();
}

void CConDOPDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( pScrollBar )
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;		
	
		char text[20];
		sprintf_s(text, sizeof(text),"%d",pSlider->GetPos());
		int pos = pSlider->GetPos();
		
		if(pSlider->m_hWnd==m_slgdop.m_hWnd)
		{
			m_egdop.SetWindowText(text);m_spgdop.SetPos(pos);
		}
		else if(pSlider->m_hWnd==m_slpdop.m_hWnd)
		{
			m_epdop.SetWindowText(text);m_sppdop.SetPos(pos);
		}
		else if(pSlider->m_hWnd==m_slhdop.m_hWnd)
		{
			m_ehdop.SetWindowText(text);m_sphdop.SetPos(pos);
		}
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CConDOPDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( pScrollBar )
	{
		CSpinButtonCtrl* pSlider = (CSpinButtonCtrl*)pScrollBar;		
	
		char text[20];
		sprintf_s(text, sizeof(text), "%d",nPos);//pSlider->GetPos());	
	
		if(pSlider->m_hWnd==m_spgdop.m_hWnd)
		{
			m_egdop.SetWindowText(text);m_slgdop.SetPos(nPos);
		}else if(pSlider->m_hWnd==m_sppdop.m_hWnd)
		{
			m_epdop.SetWindowText(text);	m_slpdop.SetPos(nPos);
		}else if(pSlider->m_hWnd==m_sphdop.m_hWnd)
		{
			m_ehdop.SetWindowText(text);m_slhdop.SetPos(nPos);
		}
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CConDOPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	pCDDlg=this;
	m_slgdop.SetRange(0,255);
	m_slpdop.SetRange(0,255);
	m_slhdop.SetRange(0,255);	

	m_slgdop.SetTicFreq(15);
	m_slpdop.SetTicFreq(15);
	m_slhdop.SetTicFreq(15);	

	m_spgdop.SetRange(0,255);
	m_sppdop.SetRange(0,255);
	m_sphdop.SetRange(0,255);

	m_spgdop.SetPos(0);
	m_sppdop.SetPos(0);
	m_sphdop.SetPos(0);
	
	m_egdop.SetWindowText("0");
	m_epdop.SetWindowText("0");
	m_ehdop.SetWindowText("0");

	m_dopsel.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CConDOPDlg::OnBnClickedOk()
{
	dopsel = m_dopsel.GetCurSel();	
	gdop   = m_slgdop.GetPos();
	pdop   = m_slpdop.GetPos();
	hdop   = m_slhdop.GetPos();	
	AfxBeginThread(CigDopThread,0);	
	OnOK();
}
