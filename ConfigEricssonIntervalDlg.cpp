// ConfigNmeaIntervalDlg.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"
#include "ConfigEricssonIntervalDlg.h"

ConfigEricssonIntervalDlg* g_pConfigNmeaIntervalDlg = NULL;

static UINT SliderTable[] = { 
	IDC_SLIDER1, IDC_SLIDER2, IDC_SLIDER3,
	IDC_SLIDER4  };

static UINT SpinTable[] = { 
	IDC_SPIN1, IDC_SPIN2, IDC_SPIN3,
	IDC_SPIN4  };

static UINT EditTable[] = { 
	IDC_EDIT1, IDC_EDIT2, IDC_EDIT3,
	IDC_EDIT4  };

static const char * InitialTable[] = {
	"1", "1", "60", "60"};


static const int tableSize = sizeof(SliderTable)/sizeof(SliderTable[0]);
static int ConfigEricssonIntervalParam[tableSize];

// ConfigEricssonIntervalDlg 
UINT ConfigEricssonIntervalThread(LPVOID pParam)
{
	U08 messages[15] = {0};   
	messages[0]  = (U08)0xa0;
	messages[1]  = (U08)0xa1;
	messages[2]  = 0;
	messages[3]  = 8;
	messages[4]  = 0x7A; //msgid		
	messages[5]  = 0x04;	
	messages[6]  = 0x01;
	messages[7]  = g_pConfigNmeaIntervalDlg->GetParam(0);
	messages[8]  = g_pConfigNmeaIntervalDlg->GetParam(1);
	messages[9]  = g_pConfigNmeaIntervalDlg->GetParam(2);
	messages[10] = g_pConfigNmeaIntervalDlg->GetParam(3);
	messages[11] = g_pConfigNmeaIntervalDlg->GetAttribute();

	U08 checksum = 0;
	for(int i=0; i<(int)messages[3]; i++)
	{
		checksum^=messages[i+4];
	}

	messages[12] = checksum;
	messages[13] = (U08)0x0d;
	messages[14] = (U08)0x0a;
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 15, "Configure Ericsson Sentence successfully");

	return 0;
}

IMPLEMENT_DYNAMIC(ConfigEricssonIntervalDlg, CDialog)
ConfigEricssonIntervalDlg::ConfigEricssonIntervalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConfigEricssonIntervalDlg::IDD, pParent)
{

}

ConfigEricssonIntervalDlg::~ConfigEricssonIntervalDlg()
{

}

void ConfigEricssonIntervalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BINARY_ATTRI, m_attr);
}


BEGIN_MESSAGE_MAP(ConfigEricssonIntervalDlg, CDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void ConfigEricssonIntervalDlg::AdjustValue(int nPos, CScrollBar* pScrollBar)
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

// ConfigEricssonIntervalDlg 
void ConfigEricssonIntervalDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void ConfigEricssonIntervalDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

BOOL ConfigEricssonIntervalDlg::OnInitDialog()
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
}

void ConfigEricssonIntervalDlg::OnBnClickedOk()
{
	CString strNum;
	for(int i=0; i<tableSize; ++i)
	{
		((CEdit*)GetDlgItem(EditTable[i]))->GetWindowText(strNum);
		ConfigEricssonIntervalParam[i] = atoi(strNum);
	}
	m_attrib = (U08)m_attr.GetCurSel();
	AfxBeginThread(ConfigEricssonIntervalThread, 0);

	OnOK();
}

U08 ConfigEricssonIntervalDlg::GetParam(int index)
{
	return ConfigEricssonIntervalParam[index];
}

///////////////////////////////////////////////////////////////////////////

// ConfigEricssonIntervalDlg 
UINT ConfigSerialNumberThread(LPVOID pParam)
{
	ConfigSerialNumberDlg* p = (ConfigSerialNumberDlg*)pParam;
	p->SendCommand();
	return 0;
}

IMPLEMENT_DYNAMIC(ConfigSerialNumberDlg, CDialog)
ConfigSerialNumberDlg::ConfigSerialNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConfigSerialNumberDlg::IDD, pParent)
{

}

ConfigSerialNumberDlg::~ConfigSerialNumberDlg()
{

}

void ConfigSerialNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ConfigSerialNumberDlg, CDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_SN, &ConfigSerialNumberDlg::OnEnChangeSn)
END_MESSAGE_MAP()

BOOL ConfigSerialNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_SN)->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void ConfigSerialNumberDlg::OnBnClickedOk()
{
	GetDlgItem(IDC_SN)->GetWindowText(strSn);
	if(strSn.GetLength() > 20)
	{
		::AfxMessageBox("The serial number length limit is 20 characters!");
		return;
	}
	AfxBeginThread(ConfigSerialNumberThread, this);
	OnOK();
}

void ConfigSerialNumberDlg::SendCommand()
{
	BinaryCommand cmd(24);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x05);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, (U08)strSn.GetLength());
	cmd.SetString(5, (LPCSTR)strSn, strSn.GetLength());

	CGPSDlg::gpsDlg->ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), 
		"Configure Serial Number successfully");
}
void ConfigSerialNumberDlg::OnEnChangeSn()
{
}
