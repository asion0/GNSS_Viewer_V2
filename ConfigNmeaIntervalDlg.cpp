// ConfigNmeaIntervalDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"
#include "ConfigNmeaIntervalDlg.h"

CConfigNmeaIntervalDlg* g_pConfigNmeaIntervalDlg = NULL;
#if (CUSTOMER_ID == SWID)	//SWID customize
static UINT SliderTable[] = { 
	IDC_SLIDER1, IDC_SLIDER2, IDC_SLIDER3,
	IDC_SLIDER4, IDC_SLIDER5  };

static UINT SpinTable[] = { 
	IDC_SPIN1, IDC_SPIN2, IDC_SPIN3,
	IDC_SPIN4, IDC_SPIN5  };

static UINT EditTable[] = { 
	IDC_EDIT1, IDC_EDIT2, IDC_EDIT3,
	IDC_EDIT4, IDC_EDIT5  };

static const char * InitialTable[] = {
	"60", "53", "59",
	"1", "49" };

#elif (CUSTOMER_ID==Ericsson)	//Ericsson
static UINT SliderTable[] = { 
	IDC_SLIDER1, IDC_SLIDER2, IDC_SLIDER3,
	IDC_SLIDER4, IDC_SLIDER5, IDC_SLIDER6, 
	IDC_SLIDER7, IDC_SLIDER8, IDC_SLIDER9, 
	IDC_SLIDER10, IDC_SLIDER11, IDC_SLIDER12 };

static UINT SpinTable[] = { 
	IDC_SPIN1, IDC_SPIN2, IDC_SPIN3,
	IDC_SPIN4, IDC_SPIN5, IDC_SPIN6, 
	IDC_SPIN7, IDC_SPIN8, IDC_SPIN9, 
	IDC_SPIN10, IDC_SPIN11, IDC_SPIN12 };

static UINT EditTable[] = { 
	IDC_EDIT1, IDC_EDIT2, IDC_EDIT3,
	IDC_EDIT4, IDC_EDIT5, IDC_EDIT6, 
	IDC_EDIT7, IDC_EDIT8, IDC_EDIT9, 
	IDC_EDIT10, IDC_EDIT11, IDC_EDIT12 };

static const char * InitialTable[] = {
	"60", "60", "60",
	"0", "0", "0",
	"0", "0", "0",
	"0", "0", "0" };

#else
static UINT SliderTable[] = { 
	IDC_SLIDER1, IDC_SLIDER2, IDC_SLIDER3,
	IDC_SLIDER4, IDC_SLIDER5, IDC_SLIDER6, 
	IDC_SLIDER7, IDC_SLIDER8, IDC_SLIDER9, 
	IDC_SLIDER10, IDC_SLIDER11, IDC_SLIDER12 };

static UINT SpinTable[] = { 
	IDC_SPIN1, IDC_SPIN2, IDC_SPIN3,
	IDC_SPIN4, IDC_SPIN5, IDC_SPIN6, 
	IDC_SPIN7, IDC_SPIN8, IDC_SPIN9, 
	IDC_SPIN10, IDC_SPIN11, IDC_SPIN12 };

static UINT EditTable[] = { 
	IDC_EDIT1, IDC_EDIT2, IDC_EDIT3,
	IDC_EDIT4, IDC_EDIT5, IDC_EDIT6, 
	IDC_EDIT7, IDC_EDIT8, IDC_EDIT9, 
	IDC_EDIT10, IDC_EDIT11, IDC_EDIT12 };

static const char * InitialTable[] = {
	"1", "1", "1",
	"0", "1", "1",
	"0", "0", "0",
	"0", "0", "0" };
#endif

static const int tableSize = sizeof(SliderTable)/sizeof(SliderTable[0]);
static int ConfigNmeaIntervalParam[tableSize];

// CConfigNmeaIntervalDlg 對話方塊
UINT ConfigNmeaIntervalThread(LPVOID pParam)
{
	U08 messages[22] = {0};   
	messages[0]  = (U08)0xa0;
	messages[1]  = (U08)0xa1;
	messages[2]  = 0;
	messages[3]  = 15;
	messages[4]  = 0x64; //msgid		
	messages[5]  = 0x02;	
	messages[6]  = g_pConfigNmeaIntervalDlg->GetParam(0);
	messages[7]  = g_pConfigNmeaIntervalDlg->GetParam(1);
	messages[8]  = g_pConfigNmeaIntervalDlg->GetParam(2);
	messages[9]  = g_pConfigNmeaIntervalDlg->GetParam(3);
	messages[10] = g_pConfigNmeaIntervalDlg->GetParam(4);
	messages[11] = g_pConfigNmeaIntervalDlg->GetParam(5);
	messages[12] = g_pConfigNmeaIntervalDlg->GetParam(6);
	messages[13] = g_pConfigNmeaIntervalDlg->GetParam(7);
	messages[14] = g_pConfigNmeaIntervalDlg->GetParam(8);
	messages[15] = g_pConfigNmeaIntervalDlg->GetParam(9);
	messages[16] = g_pConfigNmeaIntervalDlg->GetParam(10);
	messages[17] = g_pConfigNmeaIntervalDlg->GetParam(11);
	messages[18] = g_pConfigNmeaIntervalDlg->GetAttribute();

	U08 checksum = 0;
	for(int i=0;i<(int)messages[3];i++)
	{
		checksum^=messages[i+4];
	}

	messages[19] = checksum;
	messages[20] = (U08)0x0d;
	messages[21] = (U08)0x0a;
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 22, "Configure NMEA Successful...");

	return 0;
}

// CConfigNmeaIntervalDlg 對話方塊
UINT ConfigNmeaInterval2Thread(LPVOID pParam)
{
	U08 messages[16] = {0};   
	messages[0]  = (U08)0xa0;
	messages[1]  = (U08)0xa1;
	messages[2]  = 0;
	messages[3]  = 9;
	messages[4]  = 0x7A; //msgid		
	messages[5]  = 0x01;	
	messages[6]  = 0x01;
	messages[7]  = g_pConfigNmeaIntervalDlg->GetParam(0);
	messages[8]  = g_pConfigNmeaIntervalDlg->GetParam(1);
	messages[9]  = g_pConfigNmeaIntervalDlg->GetParam(2);
	messages[10] = g_pConfigNmeaIntervalDlg->GetParam(3);
	messages[11] = g_pConfigNmeaIntervalDlg->GetParam(4);
	messages[12] = g_pConfigNmeaIntervalDlg->GetAttribute();

	U08 checksum = 0;
	for(int i=0;i<(int)messages[3];i++)
	{
		checksum^=messages[i+4];
	}

	messages[13] = checksum;
	messages[14] = (U08)0x0d;
	messages[15] = (U08)0x0a;
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 16, "Configure NMEA Successful...");

	return 0;
}

IMPLEMENT_DYNAMIC(CConfigNmeaIntervalDlg, CDialog)
CConfigNmeaIntervalDlg::CConfigNmeaIntervalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigNmeaIntervalDlg::IDD, pParent)
{

}

CConfigNmeaIntervalDlg::~CConfigNmeaIntervalDlg()
{

}

void CConfigNmeaIntervalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BINARY_ATTRI, m_attr);
}


BEGIN_MESSAGE_MAP(CConfigNmeaIntervalDlg, CDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CConfigNmeaIntervalDlg::AdjustValue(int nPos, CScrollBar* pScrollBar)
{
	if(nPos==-1)
	{
		nPos = ((CSliderCtrl*)pScrollBar)->GetPos();
	}

	CString strNum;
	strNum.Format("%d", nPos);

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER1:
		GetDlgItem(IDC_EDIT1)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetPos(nPos);
		break;
	case IDC_SLIDER2:
		GetDlgItem(IDC_EDIT2)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->SetPos(nPos);
		break;
	case IDC_SLIDER3:
		GetDlgItem(IDC_EDIT3)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN3))->SetPos(nPos);
		break;
	case IDC_SLIDER4:
		GetDlgItem(IDC_EDIT4)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN4))->SetPos(nPos);
		break;
	case IDC_SLIDER5:
		GetDlgItem(IDC_EDIT5)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN5))->SetPos(nPos);
		break;
	case IDC_SLIDER6:
		GetDlgItem(IDC_EDIT6)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN6))->SetPos(nPos);
		break;
	case IDC_SLIDER7:
		GetDlgItem(IDC_EDIT7)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN7))->SetPos(nPos);
		break;
	case IDC_SLIDER8:
		GetDlgItem(IDC_EDIT8)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN8))->SetPos(nPos);
		break;
	case IDC_SLIDER9:
		GetDlgItem(IDC_EDIT9)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN9))->SetPos(nPos);
		break;
	case IDC_SLIDER10:
		GetDlgItem(IDC_EDIT10)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN10))->SetPos(nPos);
		break;
	case IDC_SLIDER11:
		GetDlgItem(IDC_EDIT11)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN11))->SetPos(nPos);
		break;
	case IDC_SLIDER12:
		GetDlgItem(IDC_EDIT12)->SetWindowText(strNum);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN12))->SetPos(nPos);
		break;


	case IDC_SPIN1:
		GetDlgItem(IDC_EDIT1)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->SetPos(nPos);
		break;
	case IDC_SPIN2:
		GetDlgItem(IDC_EDIT2)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER2))->SetPos(nPos);
		break;
	case IDC_SPIN3:
		GetDlgItem(IDC_EDIT3)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER3))->SetPos(nPos);
		break;
	case IDC_SPIN4:
		GetDlgItem(IDC_EDIT4)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER4))->SetPos(nPos);
		break;
	case IDC_SPIN5:
		GetDlgItem(IDC_EDIT5)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER5))->SetPos(nPos);
		break;
	case IDC_SPIN6:
		GetDlgItem(IDC_EDIT6)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER6))->SetPos(nPos);
		break;
	case IDC_SPIN7:
		GetDlgItem(IDC_EDIT7)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER7))->SetPos(nPos);
		break;
	case IDC_SPIN8:
		GetDlgItem(IDC_EDIT8)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER8))->SetPos(nPos);
		break;
	case IDC_SPIN9:
		GetDlgItem(IDC_EDIT9)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER9))->SetPos(nPos);
		break;
	case IDC_SPIN10:
		GetDlgItem(IDC_EDIT10)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER10))->SetPos(nPos);
		break;
	case IDC_SPIN11:
		GetDlgItem(IDC_EDIT11)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER11))->SetPos(nPos);
		break;
	case IDC_SPIN12:
		GetDlgItem(IDC_EDIT12)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER12))->SetPos(nPos);
		break;
	default:
		ASSERT(FALSE);
	}

}

// CConfigNmeaIntervalDlg 訊息處理常式
void CConfigNmeaIntervalDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{   
	if(NULL==pScrollBar)
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	if(SB_ENDSCROLL!=nSBCode)
	{
		AdjustValue(-1, pScrollBar);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CConfigNmeaIntervalDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(NULL==pScrollBar)
	{
		CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	if(SB_ENDSCROLL!=nSBCode)
	{
		AdjustValue(nPos, pScrollBar);
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CConfigNmeaIntervalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_pConfigNmeaIntervalDlg = this;

	for(int i=0; i<tableSize; ++i)
	{
		((CSliderCtrl*)GetDlgItem(SliderTable[i]))->SetRange(0, 255);
		((CSliderCtrl*)GetDlgItem(SliderTable[i]))->SetTicFreq(15);
		((CSliderCtrl*)GetDlgItem(SliderTable[i]))->SetPos(atoi(InitialTable[i]));

		((CSpinButtonCtrl*)GetDlgItem(SpinTable[i]))->SetRange(0, 255);
		((CSpinButtonCtrl*)GetDlgItem(SpinTable[i]))->SetPos(atoi(InitialTable[i]));

		((CEdit*)GetDlgItem(EditTable[i]))->SetWindowText(InitialTable[i]);
	}
	m_attr.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CConfigNmeaIntervalDlg::OnBnClickedOk()
{
	CString strNum;
	for(int i=0; i<tableSize; ++i)
	{
		((CEdit*)GetDlgItem(EditTable[i]))->GetWindowText(strNum);
		ConfigNmeaIntervalParam[i] = atoi(strNum);
	}
	m_attrib = (U08)m_attr.GetCurSel();

#if (CUSTOMER_ID==SWID)	//SWID customize
	AfxBeginThread(ConfigNmeaInterval2Thread, 0);
#else
	AfxBeginThread(ConfigNmeaIntervalThread, 0);
#endif
	OnOK();
}

U08 CConfigNmeaIntervalDlg::GetParam(int index)
{
	return ConfigNmeaIntervalParam[index];
}