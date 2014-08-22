// ConNMEADlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"
#include "ConNMEADlg.h"
#include "commctrl.h"

CConNMEADlg* pCNDlg;


// CConNMEADlg 對話方塊

UINT CigNMEAtThread(LPVOID pParam)
{

	U08 messages[16];   
    int i;	      
	memset(messages, 0, 16);
	messages[0]  = (U08)0xa0;
	messages[1]  = (U08)0xa1;
	messages[2]  = 0;
	messages[3]  = 9;
	messages[4]  = 8; //msgid		
	messages[5]  = slgga;	
	messages[6]  = slgsa;
	messages[7]  = slgsv;
	messages[8]  = slgll;
	messages[9]  = slrmc;
	messages[10] = slvtg;
	messages[11] = slzda;
	messages[12] = attribute;

	U08 checksum = 0;
	for(i=0;i<(int)messages[3];i++)
	{checksum^=messages[i+4];}
	messages[13] = checksum;
	messages[14] = (U08)0x0d;
	messages[15] = (U08)0x0a;
	//	for(int i=0;i<15;i++)	_cprintf("%x ",messages[i]);	
	CGPSDlg::gpsDlg->ClearQue();
	int ack = CGPSDlg::gpsDlg->SendToTarget(messages, 16,"Configure NMEA Successful...");
	if (ack == 1)
	{
		if (attribute == 1) Sleep(1000);
		memset(messages, 0, 16);
		messages[0]  = (U08)0xa0;
		messages[1]  = (U08)0xa1;
		messages[2]  = 0;
		messages[3]  = 3;
		messages[4]  = 0x4A; //msgid		
		messages[5]  = slgns;	
		messages[6] = attribute;

		U08 checksum = 0;
		for(i=0;i<(int)messages[3];i++)
		{checksum^=messages[i+4];}
		messages[7] = checksum;
		messages[8] = (U08)0x0d;
		messages[9] = (U08)0x0a;

		CGPSDlg::gpsDlg->ClearQue();
		int ack = CGPSDlg::gpsDlg->SendToTarget(messages, 10,"Configure GNS Successful...");
	}
	CGPSDlg::gpsDlg->SetMode(); 
	CGPSDlg::gpsDlg->CreateGPSThread();
//	AfxEndThread(0);
	return 0;
}

IMPLEMENT_DYNAMIC(CConNMEADlg, CDialog)
CConNMEADlg::CConNMEADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConNMEADlg::IDD, pParent)
{
}

CConNMEADlg::~CConNMEADlg()
{
}

void CConNMEADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_slgga);
	DDX_Control(pDX, IDC_SLIDER2, m_slgsa);
	DDX_Control(pDX, IDC_SLIDER3, m_slgsv);
	DDX_Control(pDX, IDC_SLIDER4, m_slgll);
	DDX_Control(pDX, IDC_SLIDER5, m_slrmc);
	DDX_Control(pDX, IDC_SLIDER6, m_slvtg);
	DDX_Control(pDX, IDC_SLIDER7, m_slzda);
	DDX_Control(pDX, IDC_SLIDER8, m_slgns);
	DDX_Control(pDX, IDC_EDIT1, m_gga);
	DDX_Control(pDX, IDC_EDIT2, m_gsa);
	DDX_Control(pDX, IDC_EDIT8, m_gsv);
	DDX_Control(pDX, IDC_EDIT4, m_gll);
	DDX_Control(pDX, IDC_EDIT5, m_rmc);
	DDX_Control(pDX, IDC_EDIT6, m_vtg);
	DDX_Control(pDX, IDC_EDIT7, m_zda);
	DDX_Control(pDX, IDC_EDIT10, m_gns);
	DDX_Control(pDX, IDC_SPIN2, m_spingga);
	DDX_Control(pDX, IDC_SPIN3, m_spingsa);
	DDX_Control(pDX, IDC_SPIN4, m_spingsv);
	DDX_Control(pDX, IDC_SPIN5, m_spingll);
	DDX_Control(pDX, IDC_SPIN6, m_spinrmc);
	DDX_Control(pDX, IDC_SPIN7, m_spinvtg);
	DDX_Control(pDX, IDC_SPIN8, m_spinzda);
	DDX_Control(pDX, IDC_SPIN9, m_spingns);
	DDX_Control(pDX, IDC_CFGNMEA_ATT, m_attribute);


	DDX_Control(pDX, IDC_NMEA_ZDA, m_nmea1_zda);
	DDX_Control(pDX, IDC_NMEA2_ZDA, m_nmea2_zda);
	DDX_Control(pDX, IDC_NMEA3_ZDA, m_nmea3_zda);
}


BEGIN_MESSAGE_MAP(CConNMEADlg, CDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CConNMEADlg 訊息處理常式

void CConNMEADlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( pScrollBar )
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;		
	
		char text[20];
		sprintf_s(text, sizeof(text),"%d",pSlider->GetPos());
		int pos = pSlider->GetPos();
		
		if(pSlider->m_hWnd==m_slgll.m_hWnd)
		{
			m_gll.SetWindowText(text);m_spingll.SetPos(pos);
				
		}else if(pSlider->m_hWnd==m_slgga.m_hWnd)
		{			
		    m_gga.SetWindowText(text);m_spingga.SetPos(pos);

		}else if(pSlider->m_hWnd==m_slgsa.m_hWnd)
		{			
		    m_gsa.SetWindowText(text);m_spingsa.SetPos(pos);
		}else if(pSlider->m_hWnd==m_slgsv.m_hWnd)
		{			
		    m_gsv.SetWindowText(text);	m_spingsv.SetPos(pos);
		}else if(pSlider->m_hWnd==m_slrmc.m_hWnd)
		{			
		    m_rmc.SetWindowText(text);m_spinrmc.SetPos(pos);
		}else if(pSlider->m_hWnd==m_slvtg.m_hWnd)
		{			
		    m_vtg.SetWindowText(text);m_spinvtg.SetPos(pos);
		}else if(pSlider->m_hWnd==m_slzda.m_hWnd)
		{			
		    m_zda.SetWindowText(text);m_spinzda.SetPos(pos);
		}else if(pSlider->m_hWnd==m_slgns.m_hWnd)
		{			
			m_gns.SetWindowText(text);m_spingns.SetPos(pos);
		}
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CConNMEADlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	pCNDlg=this;
	m_slgga.SetRange(0,255);
	m_slgsa.SetRange(0,255);
	m_slgsv.SetRange(0,255);
	m_slgll.SetRange(0,255);
	m_slrmc.SetRange(0,255);
	m_slvtg.SetRange(0,255);
	m_slzda.SetRange(0,255);
	m_slgns.SetRange(0,255);

	m_slgga.SetTicFreq(15);
	m_slgsa.SetTicFreq(15);
	m_slgsv.SetTicFreq(15);
	m_slgll.SetTicFreq(15);
	m_slrmc.SetTicFreq(15);
	m_slvtg.SetTicFreq(15);
	m_slzda.SetTicFreq(15);
	m_slgns.SetTicFreq(15);

	m_spingga.SetRange(0,255);
	m_spingsa.SetRange(0,255);
	m_spingsv.SetRange(0,255);
	m_spingll.SetRange(0,255);
	m_spinrmc.SetRange(0,255);
	m_spinvtg.SetRange(0,255);
	m_spinzda.SetRange(0,255);
	m_spingns.SetRange(0,255);

	m_spingga.SetPos(1);
	m_spingsa.SetPos(1);
	m_spingsv.SetPos(1);
	m_spingll.SetPos(0);
	m_spinrmc.SetPos(1);
	m_spinvtg.SetPos(0);
	m_spinzda.SetPos(0);
	m_spingns.SetPos(0);

	m_slgga.SetPos(1);
	m_slgsa.SetPos(1);
	m_slgsv.SetPos(1);
	m_slgll.SetPos(0);
	m_slrmc.SetPos(1);
	m_slvtg.SetPos(0);
	m_slzda.SetPos(0);
	m_slgns.SetPos(0);
	
	m_gga.SetWindowText("1");
	m_gsa.SetWindowText("1");
	m_gsv.SetWindowText("1");
	m_gll.SetWindowText("0");
	m_rmc.SetWindowText("1");
	m_vtg.SetWindowText("0");
	m_zda.SetWindowText("0");
	m_gns.SetWindowText("0");

	m_attribute.SetCurSel(0);

#ifdef EVERMORE
	m_spinzda.ShowWindow(0);
	m_slzda.ShowWindow(0);
	m_zda.ShowWindow(0);
	m_nmea1_zda.ShowWindow(0);
	m_nmea2_zda.ShowWindow(0);
	m_nmea3_zda.ShowWindow(0);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CConNMEADlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( pScrollBar )
	{
		CSpinButtonCtrl* pSlider = (CSpinButtonCtrl*)pScrollBar;		
	
		char text[20];
		sprintf_s(text, sizeof(text),"%d",nPos);//pSlider->GetPos());	
	
		if(pSlider->m_hWnd==m_spingga.m_hWnd)
		{
			m_gga.SetWindowText(text);m_slgga.SetPos(nPos);
		}else if(pSlider->m_hWnd==m_spingsa.m_hWnd)
		{
			m_gsa.SetWindowText(text);	m_slgsa.SetPos(nPos);
		}else if(pSlider->m_hWnd==m_spingsv.m_hWnd)
		{
			m_gsv.SetWindowText(text);m_slgsv.SetPos(nPos);
		}else if(pSlider->m_hWnd==m_spingll.m_hWnd)
		{
			m_gll.SetWindowText(text);m_slgll.SetPos(nPos);
		}else if(pSlider->m_hWnd==m_spinrmc.m_hWnd)
		{
			m_rmc.SetWindowText(text);m_slrmc.SetPos(nPos);
		}else if(pSlider->m_hWnd==m_spinvtg.m_hWnd)
		{
			m_vtg.SetWindowText(text);m_slvtg.SetPos(nPos);
		}else if(pSlider->m_hWnd==m_spinzda.m_hWnd)
		{
			m_zda.SetWindowText(text);m_slzda.SetPos(nPos);
		}else if(pSlider->m_hWnd==m_spingns.m_hWnd)
		{
			m_gns.SetWindowText(text);m_slgns.SetPos(nPos);
		}
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CConNMEADlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CConNMEADlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CConNMEADlg::OnBnClickedOk()
{
	slgga = m_slgga.GetPos();	
	slgsa = m_slgsa.GetPos();
	slgsv = m_slgsv.GetPos();
	slgll = m_slgll.GetPos();
	slrmc = m_slrmc.GetPos();
	slvtg = m_slvtg.GetPos();
	slzda = m_slzda.GetPos();
	slgns = m_slgns.GetPos();
	attribute = m_attribute.GetCurSel();
	AfxBeginThread(CigNMEAtThread,0);
	OnOK();
}
