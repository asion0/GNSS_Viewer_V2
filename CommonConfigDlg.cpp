#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"
#include "CommonConfigDlg.h"

// CCommonConfigDlg 對話方塊
BinaryCommand configCmd;
CString configPrompt;

UINT AFX_CDECL ConfigThread(LPVOID param)
{
	bool restoreConnect = (((int)(param))==0);
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), configPrompt, restoreConnect);
	return 0;
}

IMPLEMENT_DYNAMIC(CCommonConfigDlg, CDialog)

CCommonConfigDlg::CCommonConfigDlg(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)

{
	pCancelBtn = new CButton();
	pAcceptBtn = new CButton();
}

CCommonConfigDlg::~CCommonConfigDlg()
{
	delete pCancelBtn;
	delete pAcceptBtn;
}

BEGIN_MESSAGE_MAP(CCommonConfigDlg, CDialog)
END_MESSAGE_MAP()

BOOL CCommonConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON;
	CRect	rcClient, rcCtrl;
	GetClientRect(&rcClient);

	rcCtrl.SetRect(rcClient.right - 112, rcClient.bottom - 36, rcClient.right - 16, rcClient.bottom - 8);
	pCancelBtn->Create(_T("Cancel"), dwStyle, rcCtrl, this, IDCANCEL);

	rcCtrl.SetRect(rcClient.right - 224, rcClient.bottom - 36, rcClient.right - 128, rcClient.bottom - 8);
	pAcceptBtn->Create(_T("Accept"), dwStyle, rcCtrl, this, IDOK);

	return TRUE;  // return TRUE unless you set the focus to a control
}

// CConfigDGPS 對話方塊
IMPLEMENT_DYNAMIC(CConfigDGPS, CCommonConfigDlg)

CConfigDGPS::CConfigDGPS(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_DGPS, pParent)
{
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigDGPS, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigDGPS::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigDGPS 訊息處理常式
BOOL CConfigDGPS::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	GetDlgItem(IDC_OVERDUE_SEC)->SetWindowText("30");
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigDGPS::OnBnClickedOk()
{
	CString txt;
	GetDlgItem(IDC_OVERDUE_SEC)->GetWindowText(txt);
	m_nOverdueSeconds = atoi(txt);
	if(m_nOverdueSeconds < 0 || m_nOverdueSeconds > 65535)
	{
		AfxMessageBox("Invalid value!");
		return;
	}

	m_bEnable = ((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}
	
void CConfigDGPS::DoCommand()
{
	BinaryData cmd(6);
	*cmd.GetBuffer(0) = 0x69;
	*cmd.GetBuffer(1) = 0x01;
	*cmd.GetBuffer(2) = (U08)m_bEnable;
	*cmd.GetBuffer(3) = HIBYTE(m_nOverdueSeconds);
	*cmd.GetBuffer(4) = LOBYTE(m_nOverdueSeconds);
	*cmd.GetBuffer(5) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure DGPS Successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigSmoothMode 對話方塊
IMPLEMENT_DYNAMIC(CConfigSmoothMode, CCommonConfigDlg)

CConfigSmoothMode::CConfigSmoothMode(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_SMOOTH_MODE, pParent)
{
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigSmoothMode, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigSmoothMode::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigDGPS 訊息處理常式
BOOL CConfigSmoothMode::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}	

void CConfigSmoothMode::OnBnClickedOk()
{
	m_bEnable = ((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigSmoothMode::DoCommand()
{
	BinaryData cmd(5);
	*cmd.GetBuffer(0) = 0x69;
	*cmd.GetBuffer(1) = 0x03;
	*cmd.GetBuffer(2) = (U08)m_bEnable;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure carrier smooth mode successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigTimeStamping 對話方塊
IMPLEMENT_DYNAMIC(CConfigTimeStamping, CCommonConfigDlg)

CConfigTimeStamping::CConfigTimeStamping(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_TIME_STAMPING, pParent)
{
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigTimeStamping, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigTimeStamping::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigDGPS 訊息處理常式
BOOL CConfigTimeStamping::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_TIGGER_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}	

void CConfigTimeStamping::OnBnClickedOk()
{
	m_bEnable = ((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();
	m_bTiggerMode = ((CComboBox*)GetDlgItem(IDC_TIGGER_MODE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigTimeStamping::DoCommand()
{
	BinaryData cmd(5);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x1D;
	*cmd.GetBuffer(2) = (U08)m_bEnable;
	*cmd.GetBuffer(3) = (U08)m_bTiggerMode;
	*cmd.GetBuffer(4) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure time stamping successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigSBAS 對話方塊
IMPLEMENT_DYNAMIC(CConfigSBAS, CCommonConfigDlg)

CConfigSBAS::CConfigSBAS(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_SBAS, pParent)
{
	m_bEnable = FALSE;
	m_bRanging = FALSE;
	m_bCorrection = FALSE;
	m_nUraMask = 0;
	m_nTrackingChannel = 0;
	m_bWAAS = FALSE;
	m_bEGNOS = FALSE;
	m_bMSAS = FALSE;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigSBAS, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigSBAS::OnBnClickedOk)
	ON_BN_CLICKED(IDC_ENABLE_WAAS, OnBnClickedEnableWaas)
	ON_BN_CLICKED(IDC_ENABLE_EGNOS, OnBnClickedEnableEgnos)
	ON_BN_CLICKED(IDC_ENABLE_MSAS, OnBnClickedEnableMasa)
	ON_BN_CLICKED(IDC_ENABLE_ALL, OnBnClickedEnableAll)
END_MESSAGE_MAP()

// CConfigSBAS 訊息處理常式
BOOL CConfigSBAS::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	GetDlgItem(IDC_NUMBER_CHANNEL)->SetWindowText("0");
	GetDlgItem(IDC_URAMASK)->SetWindowText("8");
	((CComboBox*)GetDlgItem(IDC_ENABLE_NAV))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigSBAS::OnBnClickedOk()
{
	CString txt;
	GetDlgItem(IDC_URAMASK)->GetWindowText(txt);
	m_nUraMask = atoi(txt);
	if(m_nUraMask<0 || m_nUraMask>15)
	{
		AfxMessageBox("URA Mask must be between 0 and 15!");
		return;
	}

	GetDlgItem(IDC_NUMBER_CHANNEL)->GetWindowText(txt);
	m_nTrackingChannel = atoi(txt);
	if(m_nTrackingChannel<0 || m_nTrackingChannel>3)
	{
		AfxMessageBox("Number of tracking channels must be between 0 and 3!");
		return;
	}

	m_bEnable = ((CButton*)GetDlgItem(IDC_ENABLE_SBAS))->GetCheck();
	m_bRanging = ((CComboBox*)GetDlgItem(IDC_ENABLE_NAV))->GetCurSel();
	m_bCorrection = ((CButton*)GetDlgItem(IDC_ENABLE_CORRECTION))->GetCheck();
	m_bWAAS = ((CButton*)GetDlgItem(IDC_ENABLE_WAAS))->GetCheck();
	m_bEGNOS = ((CButton*)GetDlgItem(IDC_ENABLE_EGNOS))->GetCheck();
	m_bMSAS = ((CButton*)GetDlgItem(IDC_ENABLE_MSAS))->GetCheck();
	m_bAll = ((CButton*)GetDlgItem(IDC_ENABLE_ALL))->GetCheck();

	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigSBAS::OnBnClickedEnableWaas()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_WAAS))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(FALSE);
	}
}

void CConfigSBAS::OnBnClickedEnableEgnos()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_EGNOS))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(FALSE);
	}
}

void CConfigSBAS::OnBnClickedEnableMasa()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_MSAS))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(FALSE);
	}
}

void CConfigSBAS::OnBnClickedEnableAll()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_ALL))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_WAAS))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_ENABLE_EGNOS))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_ENABLE_MSAS))->SetCheck(FALSE);
	}
}

void CConfigSBAS::DoCommand()
{
	BinaryData cmd(9);
	*cmd.GetBuffer(0) = 0x62;
	*cmd.GetBuffer(1) = 0x01;
	*cmd.GetBuffer(2) = (U08)m_bEnable;
	*cmd.GetBuffer(3) = (U08)m_bRanging;
	*cmd.GetBuffer(4) = (U08)m_nUraMask;
	*cmd.GetBuffer(5) = (U08)m_bCorrection;
	*cmd.GetBuffer(6) = (U08)m_nTrackingChannel;
	*cmd.GetBuffer(7) = (U08)(m_bWAAS | (m_bEGNOS << 1) | (m_bMSAS << 2) | (m_bAll << 7));
	*cmd.GetBuffer(8) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure SBAS successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigSAEE 對話方塊
IMPLEMENT_DYNAMIC(CConfigSAEE, CCommonConfigDlg)

CConfigSAEE::CConfigSAEE(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_SAGPS, pParent)
{
	m_nEnable = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigSAEE, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigSAEE::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigSAEE 訊息處理常式
BOOL CConfigSAEE::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	GetDlgItem(IDC_WARNING)->SetWindowText("Warning :\r\nPlease disable multi-hz position update rate before enable SAEE.");
	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigSAEE::OnBnClickedOk()
{
	m_nEnable = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigSAEE::DoCommand()
{
#if (CHECK_SAEE_MULTIHZ_ON)
	if(m_nEnable != 2)
	{	//Enable SAEE must checke position update rate first.
		U08 data = 0;
		if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QueryPositionRate(CGPSDlg::Return, &data))
		{
			if(data > 1)
			{	//Now is in multi-hz position update rate.
				CGPSDlg::gpsDlg->add_msgtolist("Configure SAEE Cancel...");	
				::AfxMessageBox("Please disable multi-hz position update rate before enable SAEE.");

				CGPSDlg::gpsDlg->SetMode();  
				CGPSDlg::gpsDlg->CreateGPSThread();
				return;
			}
		}
	}
#endif
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x63;
	*cmd.GetBuffer(1) = 0x01;
	*cmd.GetBuffer(2) = (U08)m_nEnable;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure SAEE successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigQZSS 對話方塊
IMPLEMENT_DYNAMIC(CConfigQZSS, CCommonConfigDlg)

CConfigQZSS::CConfigQZSS(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_QZSS, pParent)
{
	m_bEnable = FALSE;
	m_nTrackingChannel = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigQZSS, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigQZSS::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigQZSS 訊息處理常式
BOOL CConfigQZSS::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	GetDlgItem(IDC_NUMBER_CHANNEL)->SetWindowText("1");
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigQZSS::OnBnClickedOk()
{
	CString txt;
	GetDlgItem(IDC_NUMBER_CHANNEL)->GetWindowText(txt);
	m_nTrackingChannel = atoi(txt);
	if(m_nTrackingChannel < 0 || m_nTrackingChannel > 3)
	{
		AfxMessageBox("Number of tracking channels must be between 0 to 3!");
		return;
	}

	m_bEnable = ((CButton*)GetDlgItem(IDC_ENABLE_QZSS))->GetCheck();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigQZSS::DoCommand()
{
	BinaryData cmd(5);
	*cmd.GetBuffer(0) = 0x62;
	*cmd.GetBuffer(1) = 0x03;
	*cmd.GetBuffer(2) = (U08)m_bEnable;
	*cmd.GetBuffer(3) = (U08)m_nTrackingChannel;
	*cmd.GetBuffer(4) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure QZSS Successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigInterferenceDetectControl 對話方塊
IMPLEMENT_DYNAMIC(CConfigInterferenceDetectControl, CCommonConfigDlg)

CConfigInterferenceDetectControl::CConfigInterferenceDetectControl(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_INF_DTE_CTRL, pParent)
{
	m_nMode = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigInterferenceDetectControl, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigInterferenceDetectControl::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigInterferenceDetectControl 訊息處理常式
BOOL CConfigInterferenceDetectControl::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigInterferenceDetectControl::OnBnClickedOk()
{
	m_nMode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigInterferenceDetectControl::DoCommand()
{
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x06;
	*cmd.GetBuffer(2) = (U08)m_nMode;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure InterferenceDetectControl successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigNMEABinaryOutputDestination 對話方塊
IMPLEMENT_DYNAMIC(CConfigNMEABinaryOutputDestination, CCommonConfigDlg)

CConfigNMEABinaryOutputDestination::CConfigNMEABinaryOutputDestination(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_NMBI_OUT_DES, pParent)
{
	m_nMode = 0;
	m_nAttribute = 0;
}


BEGIN_MESSAGE_MAP(CConfigNMEABinaryOutputDestination, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigNMEABinaryOutputDestination::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigNMEABinaryOutputDestination 訊息處理常式
BOOL CConfigNMEABinaryOutputDestination::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CButton*)GetDlgItem(IDC_UART))->SetCheck(1);
	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigNMEABinaryOutputDestination::OnBnClickedOk()
{
	m_nMode = 0;
	if(((CButton*)GetDlgItem(IDC_UART))->GetCheck())
	{
		m_nMode |= 0x01;
	}
	int nSlaveSel = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	if(nSlaveSel==1)
	{
		m_nMode |= 0x02;
	}
	else if(nSlaveSel==2)
	{
		m_nMode |= 0x04;
	}
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigNMEABinaryOutputDestination::DoCommand()
{
	BinaryData cmd(5);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x04;
	*cmd.GetBuffer(2) = 0x00;
	*cmd.GetBuffer(3) = (U08)m_nMode;
	*cmd.GetBuffer(4) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure NMEABinaryOutputDestination successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigParameterSearchEngineNumber 對話方塊
IMPLEMENT_DYNAMIC(CConfigParameterSearchEngineNumber, CCommonConfigDlg)

CConfigParameterSearchEngineNumber::CConfigParameterSearchEngineNumber(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_PARAM_SEARCH_ENG_NUM, pParent)
{
	m_nMode = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigParameterSearchEngineNumber, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigParameterSearchEngineNumber::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_MODE, &CConfigParameterSearchEngineNumber::OnCbnSelchangeMode)
END_MESSAGE_MAP()

// CConfigParameterSearchEngineNumber 訊息處理常式
BOOL CConfigParameterSearchEngineNumber::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
	OnCbnSelchangeMode();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigParameterSearchEngineNumber::OnBnClickedOk()
{
	m_nMode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigParameterSearchEngineNumber::OnCbnSelchangeMode()
{
	int nSel = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	const char* szTable[] = { 
		"0 : ROM version decide by HW power-on latch.\r\n     FLASH version : by SW define.",
		"1 : Low",
		"2 : Middle",
		"3 : High",
		"4 : Full"};
	GetDlgItem(IDC_DESCRIPTION)->SetWindowText(szTable[nSel]);
}

void CConfigParameterSearchEngineNumber::DoCommand()
{
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x0A;
	*cmd.GetBuffer(2) = (U08)m_nMode;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure ParameterSearchEngineNumber successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigPositionFixNavigationMask 對話方塊
IMPLEMENT_DYNAMIC(CConfigPositionFixNavigationMask, CCommonConfigDlg)

CConfigPositionFixNavigationMask::CConfigPositionFixNavigationMask(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_POS_FIX_NAV_MASK, pParent)
{
	m_nMask1 = 0;
	m_nMask2 = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigPositionFixNavigationMask, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigPositionFixNavigationMask::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigPositionFixNavigationMask 訊息處理常式
BOOL CConfigPositionFixNavigationMask::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MASK1))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_MASK2))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigPositionFixNavigationMask::OnBnClickedOk()
{
	m_nMask1 = ((CComboBox*)GetDlgItem(IDC_MASK1))->GetCurSel();
	m_nMask2 = ((CComboBox*)GetDlgItem(IDC_MASK2))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigPositionFixNavigationMask::DoCommand()
{
	BinaryData cmd(5);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x11;
	*cmd.GetBuffer(2) = (U08)m_nMask1;
	*cmd.GetBuffer(3) = (U08)m_nMask2;
	*cmd.GetBuffer(4) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure PositionFixNavigationMask successful...";
    AfxBeginThread(ConfigThread, 0);
}

// ConfigRefTimeToGpsTimeDlg 對話方塊
IMPLEMENT_DYNAMIC(ConfigRefTimeToGpsTimeDlg, CCommonConfigDlg)

ConfigRefTimeToGpsTimeDlg::ConfigRefTimeToGpsTimeDlg(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_REF_TIME_TO_GPS_TIME, pParent)
{

}

BEGIN_MESSAGE_MAP(ConfigRefTimeToGpsTimeDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &ConfigRefTimeToGpsTimeDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigRefTimeToGpsTimeDlg 訊息處理常式
BOOL ConfigRefTimeToGpsTimeDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	CComboBox* monthCombo = (CComboBox*)GetDlgItem(IDC_MONTH);
	CComboBox* dayCombo = (CComboBox*)GetDlgItem(IDC_DAY);

	((CButton*)GetDlgItem(IDC_ENABLE))->SetCheck(0);
	GetDlgItem(IDC_YEAR)->SetWindowText("2015");
	monthCombo->SetCurSel(0);
	dayCombo->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void ConfigRefTimeToGpsTimeDlg::OnBnClickedOk()
{
	CString strValue;
	m_isEnable = ((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();
	GetDlgItem(IDC_YEAR)->GetWindowText(strValue);
	m_nYear = atoi(strValue);
	GetDlgItem(IDC_MONTH)->GetWindowText(strValue);
	m_nMonth = atoi(strValue);
	GetDlgItem(IDC_DAY)->GetWindowText(strValue);
	m_nDay = atoi(strValue);
	m_nAttribute = 0;
	
	CCommonConfigDlg::OnOK();
}

void ConfigRefTimeToGpsTimeDlg::DoCommand()
{
	BinaryData cmd(8);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x15;
	*cmd.GetBuffer(2) = (U08)m_isEnable;
	*cmd.GetBuffer(3) = (U08)HIBYTE(m_nYear);
	*cmd.GetBuffer(4) = (U08)LOBYTE(m_nYear);
	*cmd.GetBuffer(5) = (U08)m_nMonth;
	*cmd.GetBuffer(6) = (U08)m_nDay;

	configCmd.SetData(cmd);
	configPrompt = "Configure RefTimeSyncToGpsTime successful...";
    AfxBeginThread(ConfigThread, 0);
}

// ConfigGnssConstellationTypeDlg 對話方塊
IMPLEMENT_DYNAMIC(ConfigGnssConstellationTypeDlg, CCommonConfigDlg)

ConfigGnssConstellationTypeDlg::ConfigGnssConstellationTypeDlg(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_GNSS_CTLN_TPE, pParent)
{

}

BEGIN_MESSAGE_MAP(ConfigGnssConstellationTypeDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &ConfigGnssConstellationTypeDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigRefTimeToGpsTimeDlg 訊息處理常式
BOOL ConfigGnssConstellationTypeDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CButton*)GetDlgItem(IDC_GPS))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_GLONASS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_GALILEO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BEIDOU))->SetCheck(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void ConfigGnssConstellationTypeDlg::OnBnClickedOk()
{
	m_mode = 0;
	if(((CButton*)GetDlgItem(IDC_GPS))->GetCheck())
	{
		m_mode |= 0x1;
	}
	if(((CButton*)GetDlgItem(IDC_GLONASS))->GetCheck())
	{
		m_mode |= 0x2;
	}
	if(((CButton*)GetDlgItem(IDC_GALILEO))->GetCheck())
	{
		m_mode |= 0x4;
	}
	if(((CButton*)GetDlgItem(IDC_BEIDOU))->GetCheck())
	{
		m_mode |= 0x8;
	}
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void ConfigGnssConstellationTypeDlg::DoCommand()
{
	BinaryData cmd(5);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x19;
	*cmd.GetBuffer(2) = (U08)HIBYTE(m_mode);
	*cmd.GetBuffer(3) = (U08)LOBYTE(m_mode);
	*cmd.GetBuffer(4) = (U08)m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure GnssNavSol Successful...";
    AfxBeginThread(ConfigThread, 0);
}

// ConfigBinaryMeasurementDataOutDlg 對話方塊
IMPLEMENT_DYNAMIC(ConfigBinaryMeasurementDataOutDlg, CCommonConfigDlg)

ConfigBinaryMeasurementDataOutDlg::ConfigBinaryMeasurementDataOutDlg(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_BIN_MEA_DAT_OUT, pParent)
{

}

BEGIN_MESSAGE_MAP(ConfigBinaryMeasurementDataOutDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &ConfigBinaryMeasurementDataOutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigRefTimeToGpsTimeDlg 訊息處理常式
BOOL ConfigBinaryMeasurementDataOutDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	((CComboBox*)GetDlgItem(IDC_OUTPUT_RATE))->SetCurSel(0);

	((CButton*)GetDlgItem(IDC_MEAS_TIME))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RAW_MEAS))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_SV_CH))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RCV_STATE))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_GPS))->SetCheck(1);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void ConfigBinaryMeasurementDataOutDlg::OnBnClickedOk()
{
	m_rate = ((CComboBox*)GetDlgItem(IDC_OUTPUT_RATE))->GetCurSel();
	m_measTime = ((CButton*)GetDlgItem(IDC_MEAS_TIME))->GetCheck();
	m_rawMeas = ((CButton*)GetDlgItem(IDC_RAW_MEAS))->GetCheck();
	m_svChStatus = ((CButton*)GetDlgItem(IDC_SV_CH))->GetCheck();
	m_rcvChStatus = ((CButton*)GetDlgItem(IDC_RCV_STATE))->GetCheck();

	m_subFrame = 0;
	if(((CButton*)GetDlgItem(IDC_GPS))->GetCheck())
	{
		m_subFrame |= 0x1;
	}
	if(((CButton*)GetDlgItem(IDC_GLONASS))->GetCheck())
	{
		m_subFrame |= 0x2;
	}
	if(((CButton*)GetDlgItem(IDC_GALILEO))->GetCheck())
	{
		m_subFrame |= 0x4;
	}
	if(((CButton*)GetDlgItem(IDC_BEIDOU))->GetCheck())
	{
		m_subFrame |= 0x8;
	}
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void ConfigBinaryMeasurementDataOutDlg::DoCommand()
{
	BinaryData cmd(8);
	*cmd.GetBuffer(0) = 0x1E;
	*cmd.GetBuffer(1) = (U08)m_rate;
	*cmd.GetBuffer(2) = (U08)m_measTime;
	*cmd.GetBuffer(3) = (U08)m_rawMeas;
	*cmd.GetBuffer(4) = (U08)m_svChStatus;
	*cmd.GetBuffer(5) = (U08)m_rcvChStatus;
	*cmd.GetBuffer(6) = (U08)m_subFrame;
	*cmd.GetBuffer(7) = (U08)m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure BinaryMeasurementDataOut successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigLeapSeconds 對話方塊
IMPLEMENT_DYNAMIC(CConfigLeapSeconds, CCommonConfigDlg)

CConfigLeapSeconds::CConfigLeapSeconds(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_LEAP_SECONDS, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigLeapSeconds, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigLeapSeconds::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigRefTimeToGpsTimeDlg 訊息處理常式
BOOL CConfigLeapSeconds::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	GetDlgItem(IDC_LEAP_SECONDS)->SetWindowText("0");
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigLeapSeconds::OnBnClickedOk()
{
	CString txt;
	GetDlgItem(IDC_LEAP_SECONDS)->GetWindowText(txt);
	m_nLeapSeconds = atoi(txt);
	if(m_nLeapSeconds < 0 || m_nLeapSeconds > 255)
	{
		AfxMessageBox("Invalid value!");
		return;
	}
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigLeapSeconds::DoCommand()
{
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x1F;
	*cmd.GetBuffer(2) = (U08)m_nLeapSeconds;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure GPS/UTC leap seconds successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigPowerMode 對話方塊
IMPLEMENT_DYNAMIC(CConfigPowerMode, CCommonConfigDlg)

CConfigPowerMode::CConfigPowerMode(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_POWER_MODE, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigPowerMode, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigPowerMode::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigRefTimeToGpsTimeDlg 訊息處理常式
BOOL CConfigPowerMode::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	((CComboBox*)GetDlgItem(IDC_POWER_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigPowerMode::OnBnClickedOk()
{
	CString txt;
	m_nPowerMode = ((CComboBox*)GetDlgItem(IDC_POWER_MODE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigPowerMode::DoCommand()
{
	BinaryData cmd(3);
	*cmd.GetBuffer(0) = 0x0C;
	*cmd.GetBuffer(1) = (U08)m_nPowerMode;
	*cmd.GetBuffer(2) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure PowerMode Successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigParamSearchEngineSleepCRiteria 對話方塊
IMPLEMENT_DYNAMIC(CConfigParamSearchEngineSleepCRiteria, CCommonConfigDlg)

CConfigParamSearchEngineSleepCRiteria::CConfigParamSearchEngineSleepCRiteria(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_PARAM_SRCH_ENG_SLP_CRT, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigParamSearchEngineSleepCRiteria, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigParamSearchEngineSleepCRiteria::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigParamSearchEngineSleepCRiteria 訊息處理常式
BOOL CConfigParamSearchEngineSleepCRiteria::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	GetDlgItem(IDC_TRACKED_NUM)->SetWindowText("10");
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigParamSearchEngineSleepCRiteria::OnBnClickedOk()
{
	CString txt;
	GetDlgItem(IDC_TRACKED_NUM)->GetWindowText(txt);
	m_nTrackedNumber = atoi(txt);
	if(m_nTrackedNumber!=0 && (m_nTrackedNumber < 6 || m_nTrackedNumber > 16))
	{
		AfxMessageBox("Invalid value!");
		return;
	}
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigParamSearchEngineSleepCRiteria::DoCommand()
{
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x25;
	*cmd.GetBuffer(2) = (U08)m_nTrackedNumber;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure ParamSearchEngineSleepCRiteria successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigDatumIndex 對話方塊
IMPLEMENT_DYNAMIC(CConfigDatumIndex, CCommonConfigDlg)

CConfigDatumIndex::CConfigDatumIndex(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_DATUM_INDEX, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigDatumIndex, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigDatumIndex::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigParamSearchEngineSleepCRiteria 訊息處理常式
BOOL CConfigDatumIndex::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	CComboBox* cbDatum = (CComboBox*)GetDlgItem(IDC_DATUM_LIST);

	cbDatum->ResetContent();
	for(int i=0; i<DatumListSize; ++i)
	{
		if(DatumList[i][0]==NULL)
			break;
		cbDatum->AddString(DatumList[i]);
	}
	cbDatum->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigDatumIndex::OnBnClickedOk()
{
	m_nDatumIndex = ((CComboBox*)GetDlgItem(IDC_DATUM_LIST))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigDatumIndex::DoCommand()
{
	BinaryData cmd(5);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x27;
	*cmd.GetBuffer(2) = (U08)HIBYTE(m_nDatumIndex);
	*cmd.GetBuffer(3) = (U08)LOBYTE(m_nDatumIndex);
	*cmd.GetBuffer(4) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure DatumIndex successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CSUP800EraseUserDataDlg 對話方塊
IMPLEMENT_DYNAMIC(CSUP800EraseUserDataDlg, CCommonConfigDlg)

CSUP800EraseUserDataDlg::CSUP800EraseUserDataDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_SUP800_ERASE_DATA, pParent)
{

}

BEGIN_MESSAGE_MAP(CSUP800EraseUserDataDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDC_ERASE, &CSUP800EraseUserDataDlg::OnBnClickedErase)
END_MESSAGE_MAP()

// CSUP800EraseUserDataDlg 訊息處理常式
BOOL CSUP800EraseUserDataDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	CString str;
	for(int i=0; i<32; ++i)
	{
		str.Format("%d", i);
		((CComboBox*)GetDlgItem(IDC_SECTOR))->AddString(str);
	}
	((CComboBox*)GetDlgItem(IDC_SECTOR))->SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSUP800EraseUserDataDlg::OnBnClickedErase()
{
	m_nSector = ((CComboBox*)GetDlgItem(IDC_SECTOR))->GetCurSel();

	OnOK();
}

void CSUP800EraseUserDataDlg::DoCommand()
{
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x7A;
	*cmd.GetBuffer(1) = 0x09;
	*cmd.GetBuffer(2) = 0x01;
	*cmd.GetBuffer(3) = m_nSector;

	configCmd.SetData(cmd);
	configPrompt = "SUP800 erase user data successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CSUP800WriteUserDataDlg 對話方塊
IMPLEMENT_DYNAMIC(CSUP800WriteUserDataDlg, CCommonConfigDlg)

CSUP800WriteUserDataDlg::CSUP800WriteUserDataDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_SUP800_WRITE_DATA, pParent)
{

}

BEGIN_MESSAGE_MAP(CSUP800WriteUserDataDlg, CCommonConfigDlg)
	ON_EN_CHANGE(IDC_DATA, &CSUP800WriteUserDataDlg::OnEnChangeInput)
	ON_BN_CLICKED(IDC_WRITE, &CSUP800WriteUserDataDlg::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_LOAD, &CSUP800WriteUserDataDlg::OnBnClickedLoad)
END_MESSAGE_MAP()

// CSUP800WriteUserDataDlg 訊息處理常式
BOOL CSUP800WriteUserDataDlg::OnInitDialog()
{
	//CCommonConfigDlg::OnInitDialog();
	CDialog::OnInitDialog();

	CString str;
	for(int i=0; i<32; ++i)
	{
		str.Format("%d", i);
		((CComboBox*)GetDlgItem(IDC_SECTOR))->AddString(str);
	}
	((CComboBox*)GetDlgItem(IDC_SECTOR))->SetCurSel(0);
	GetDlgItem(IDC_OFFSET)->SetWindowText("0");
	GetDlgItem(IDC_DATA)->SetFont(&CGPSDlg::messageFont, TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSUP800WriteUserDataDlg::OnBnClickedWrite()
{
	CString txt;

	m_nSector = ((CComboBox*)GetDlgItem(IDC_SECTOR))->GetCurSel();
	GetDlgItem(IDC_OFFSET)->GetWindowText(txt);
	m_nOffset = atoi(txt);

	GetDlgItem(IDC_DATA)->GetWindowText(txt);
	if(!Utility::ConvertHexToBinary(txt, m_binData))
	{

	}
	
	OnOK();
}

void CSUP800WriteUserDataDlg::DoCommand()
{
	BinaryData cmd(m_binData.Size() + 8);
	*cmd.GetBuffer(0) = 0x7A;
	*cmd.GetBuffer(1) = 0x09;
	*cmd.GetBuffer(2) = 0x02;
	*cmd.GetBuffer(3) = m_nSector;
	*cmd.GetBuffer(4) = HIBYTE(m_nOffset);
	*cmd.GetBuffer(5) = LOBYTE(m_nOffset);
	*cmd.GetBuffer(6) = HIBYTE(m_binData.Size());
	*cmd.GetBuffer(7) = LOBYTE(m_binData.Size());
	memcpy(cmd.GetBuffer(8), m_binData.Ptr(), m_binData.Size());

	configCmd.SetData(cmd);
	configPrompt = "SUP800 write user data successful...";
    AfxBeginThread(ConfigThread, 0);
}

void CSUP800WriteUserDataDlg::OnEnChangeInput()
{
	// TODO:  如果這是 RICHEDIT 控制項，控制項將不會
	// 傳送此告知，除非您覆寫 CDialog::OnInitDialog()
	// 函式和呼叫 CRichEditCtrl().SetEventMask()
	// 讓具有 ENM_CHANGE 旗標 ORed 加入遮罩。
 	CString strInput;
	GetDlgItem(IDC_DATA)->GetWindowText(strInput);
	CWnd* pBtn = GetDlgItem(IDC_WRITE);

	//GetDlgItem(IDC_CHECKSUM)->SetWindowText(strInput + "_");
	BinaryData binData;
	if(!Utility::ConvertHexToBinary(strInput, binData))
	{
		GetDlgItem(IDC_DATA_SIZE)->SetWindowText("Invalidate Format!");
		pBtn->EnableWindow(FALSE);
		return;
	}

	CString strOutput;
	strOutput.Format("Size : %d", binData.Size());
	GetDlgItem(IDC_DATA_SIZE)->SetWindowText(strOutput);

	if(binData.Size()>0 && binData.Size()<=256)
	{
		pBtn->EnableWindow(TRUE);
	}
	else
	{
		pBtn->EnableWindow(FALSE);
	}
}

void CSUP800WriteUserDataDlg::OnBnClickedLoad()
{
	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("data.bin");

	CFileDialog dlgFile(TRUE, NULL, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("ALL Files (*.*)|*.*||"), this);
	
	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();

	if(nResult != IDOK)
	{
		return;
	}

	BinaryData binData;
	int len = binData.ReadFromFile(fileName);
	if(len == 0)
	{
		::AfxMessageBox("Load file fail!");
		return;
	}

	CString strHex;
	len = (binData.Size()>256) ? 256 : binData.Size();
	bool b = Utility::ConvertBinaryToHex(binData, strHex, 0, len, 16);

	GetDlgItem(IDC_DATA)->SetWindowText(strHex);
	if(binData.Size()>256)
	{
		::AfxMessageBox("File size is more than 256 bytes, only use 256 bytes.");
	}
	OnEnChangeInput();
}

// CSUP800ReadUserDataDlg 對話方塊
IMPLEMENT_DYNAMIC(CSUP800ReadUserDataDlg, CCommonConfigDlg)

CSUP800ReadUserDataDlg::CSUP800ReadUserDataDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_SUP800_READ_DATA, pParent)
{

}

BEGIN_MESSAGE_MAP(CSUP800ReadUserDataDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDC_READ, &CSUP800ReadUserDataDlg::OnBnClickedRead)
	ON_BN_CLICKED(IDC_SAVE, &CSUP800ReadUserDataDlg::OnBnClickedSave)
END_MESSAGE_MAP()

// CSUP800ReadUserDataDlg 訊息處理常式
BOOL CSUP800ReadUserDataDlg::OnInitDialog()
{
	//CCommonConfigDlg::OnInitDialog();
	CDialog::OnInitDialog();

	CString str;
	for(int i=0; i<32; ++i)
	{
		str.Format("%d", i);
		((CComboBox*)GetDlgItem(IDC_SECTOR))->AddString(str);
	}
	((CComboBox*)GetDlgItem(IDC_SECTOR))->SetCurSel(0);
	GetDlgItem(IDC_OFFSET)->SetWindowText("0");
	GetDlgItem(IDC_DATA_SIZE)->SetWindowText("1");
	GetDlgItem(IDC_DATA)->SetFont(&CGPSDlg::messageFont, TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSUP800ReadUserDataDlg::OnBnClickedRead()
{
	CString txt;
	m_nSector = ((CComboBox*)GetDlgItem(IDC_SECTOR))->GetCurSel();
	GetDlgItem(IDC_OFFSET)->GetWindowText(txt);
	m_nOffset = atoi(txt);	
	GetDlgItem(IDC_DATA_SIZE)->GetWindowText(txt);
	m_nDataSize = atoi(txt);	
	
	if( (m_nOffset<4096 && m_nOffset>=0) && 
		(m_nDataSize>0 && m_nDataSize<=256) )
	{
		DoCommand();
	}
	else
	{
		AfxMessageBox("Invalid parameter!");
	}
	//OnOK();
}

void CSUP800ReadUserDataDlg::OnBnClickedSave()
{
	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("data.bin");

	CFileDialog dlgFile(FALSE, NULL, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("ALL Files (*.*)|*.*||"), this);
	
	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();

	if(nResult != IDOK)
	{
		return;
	}

	CFile f;
	if(!f.Open(fileName, CFile::modeWrite | CFile::modeCreate))
	{
		::AfxMessageBox("Save user data fail!");
		return;
	}
	
	CString txt;
	GetDlgItem(IDC_DATA)->GetWindowText(txt);
	BinaryData binData;
	if(!txt.IsEmpty() && !Utility::ConvertHexToBinary(txt, binData))
	{
		::AfxMessageBox("Invalidate Format!");
		return;
	}
	f.Write(binData.Ptr(), binData.Size());
	f.Close();
}

void CSUP800ReadUserDataDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(8);
	*cmd.GetBuffer(0) = 0x7A;
	*cmd.GetBuffer(1) = 0x09;
	*cmd.GetBuffer(2) = 0x03;
	*cmd.GetBuffer(3) = m_nSector;
	*cmd.GetBuffer(4) = HIBYTE(m_nOffset);
	*cmd.GetBuffer(5) = LOBYTE(m_nOffset);
	*cmd.GetBuffer(6) = HIBYTE(m_nDataSize);
	*cmd.GetBuffer(7) = LOBYTE(m_nDataSize);

	configCmd.SetData(cmd);
	configPrompt = "SUP800 read user data successful...";

	//bool restoreConnect = (((int)(param))==0);
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), configPrompt, false);
	BinaryData ackCmd;
	ackCmd.Alloc(1024);
	CGPSDlg::CmdErrorCode er = CGPSDlg::gpsDlg->GetBinaryResponse(&ackCmd, 0x7A, 0x09, 2000, true, true);

	if(er != CGPSDlg::Ack)
	{
		::AfxMessageBox("Read user data fail!");
		return;
	}

	CString strHex;
	bool b = Utility::ConvertBinaryToHex(ackCmd, strHex, 7, m_nDataSize, 16);
	GetDlgItem(IDC_DATA)->SetWindowText(strHex);
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

// CConfigureSignalDisturbanceStatusDlg 對話方塊
IMPLEMENT_DYNAMIC(CConfigureSignalDisturbanceStatusDlg, CCommonConfigDlg)

CConfigureSignalDisturbanceStatusDlg::CConfigureSignalDisturbanceStatusDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_SIG_DISTURB_DLG, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigureSignalDisturbanceStatusDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigureSignalDisturbanceStatusDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigureSignalDisturbanceStatusDlg 訊息處理常式
BOOL CConfigureSignalDisturbanceStatusDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	((CComboBox*)GetDlgItem(IDC_OPERATION))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigureSignalDisturbanceStatusDlg::OnBnClickedOk()
{
	m_nOperationType = ((CComboBox*)GetDlgItem(IDC_OPERATION))->GetCurSel();

	OnOK();
}

void CConfigureSignalDisturbanceStatusDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(3);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x2A;
	*cmd.GetBuffer(2) = m_nOperationType;

	configCmd.SetData(cmd);
	configPrompt = "ConfigureSignalDisturbanceStatus successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigureGpsUtcLeapSecondsInUtcDlg 對話方塊
IMPLEMENT_DYNAMIC(CConfigureGpsUtcLeapSecondsInUtcDlg, CCommonConfigDlg)

CConfigureGpsUtcLeapSecondsInUtcDlg::CConfigureGpsUtcLeapSecondsInUtcDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_GPS_LEAP_IN_UTC_DLG, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigureGpsUtcLeapSecondsInUtcDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigureGpsUtcLeapSecondsInUtcDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigureSignalDisturbanceStatusDlg 訊息處理常式
BOOL CConfigureGpsUtcLeapSecondsInUtcDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	GetDlgItem(IDC_YEAR)->SetWindowText("2015");
	((CComboBox*)GetDlgItem(IDC_MONTH))->SetCurSel(0);
	GetDlgItem(IDC_LEAP_SEC)->SetWindowText("17");
	((CComboBox*)GetDlgItem(IDC_INS_SEC))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigureGpsUtcLeapSecondsInUtcDlg::OnBnClickedOk()
{
	CString txt;
	GetDlgItem(IDC_YEAR)->GetWindowText(txt);
	m_nYear = atoi(txt);

	GetDlgItem(IDC_MONTH)->GetWindowText(txt);
	m_nMonth = atoi(txt);

	GetDlgItem(IDC_LEAP_SEC)->GetWindowText(txt);
	m_nLeapSeconds = atoi(txt);

	GetDlgItem(IDC_INS_SEC)->GetWindowText(txt);
	m_nInsertSecond = atoi(txt);

	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigureGpsUtcLeapSecondsInUtcDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(8);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x2D;
	*cmd.GetBuffer(2) = HIBYTE(m_nYear);
	*cmd.GetBuffer(3) = LOBYTE(m_nYear);
	*cmd.GetBuffer(4) = m_nMonth;
	*cmd.GetBuffer(5) = m_nLeapSeconds;
	*cmd.GetBuffer(6) = m_nInsertSecond;
	*cmd.GetBuffer(7) = m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "ConfigureGpsUtcLeapSecondsInUtc successful...";
    AfxBeginThread(ConfigThread, 0);
}

// CConfigNoisePowerControlDlg 對話方塊
IMPLEMENT_DYNAMIC(CConfigNoisePowerControlDlg, CCommonConfigDlg)

CConfigNoisePowerControlDlg::CConfigNoisePowerControlDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_NOISE_PWR_CTRL, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigNoisePowerControlDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigNoisePowerControlDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_MODE, &CConfigNoisePowerControlDlg::OnCbnSelchangeMode)
	ON_CBN_SELCHANGE(IDC_DEFAULT, &CConfigNoisePowerControlDlg::OnCbnSelchangeMode)
END_MESSAGE_MAP()

// CConfigureSignalDisturbanceStatusDlg 訊息處理常式
BOOL CConfigNoisePowerControlDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_DEFAULT))->SetCurSel(0);
	GetDlgItem(IDC_VALUE)->SetWindowText("2500000");
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
	OnCbnSelchangeMode();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigNoisePowerControlDlg::OnBnClickedOk()
{
	m_nMode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_nDefault = ((CComboBox*)GetDlgItem(IDC_DEFAULT))->GetCurSel();

	CString txt;
	GetDlgItem(IDC_VALUE)->GetWindowText(txt);
	m_nValue = atoi(txt);

	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigNoisePowerControlDlg::OnCbnSelchangeMode()
{
	m_nMode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_nDefault = ((CComboBox*)GetDlgItem(IDC_DEFAULT))->GetCurSel();
	if(m_nMode == 1)
	{
		GetDlgItem(IDC_DEFAULT)->EnableWindow(FALSE);
		GetDlgItem(IDC_VALUE)->EnableWindow(FALSE);
	}
	else if(m_nMode == 0 && m_nDefault == 1)
	{
		GetDlgItem(IDC_DEFAULT)->EnableWindow(TRUE);
		GetDlgItem(IDC_VALUE)->EnableWindow(TRUE);
	}
	else if(m_nMode == 0 && m_nDefault == 0)
	{
		GetDlgItem(IDC_DEFAULT)->EnableWindow(TRUE);
		GetDlgItem(IDC_VALUE)->EnableWindow(FALSE);
	}
}

void CConfigNoisePowerControlDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(9);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x08;
	*cmd.GetBuffer(2) = m_nMode;
	*cmd.GetBuffer(3) = m_nDefault;
	*cmd.GetBuffer(4) = HIBYTE(HIWORD(m_nValue));
	*cmd.GetBuffer(5) = LOBYTE(HIWORD(m_nValue));
	*cmd.GetBuffer(6) = HIBYTE(LOWORD(m_nValue));
	*cmd.GetBuffer(7) = LOBYTE(LOWORD(m_nValue));
	*cmd.GetBuffer(8) = m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "ConfigureNoisePower successful...";
    AfxBeginThread(ConfigThread, 0);
}

// ConfigPowerSavingParametersRomDlg 對話方塊
IMPLEMENT_DYNAMIC(ConfigPowerSavingParametersRomDlg, CCommonConfigDlg)

ConfigPowerSavingParametersRomDlg::ConfigPowerSavingParametersRomDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_PWR_SVNG_PRM_ROM, pParent)
{
	m_param[0] = 2;
	m_param[1] = 9;
	m_param[2] = 0;
	m_param[3] = 4;
	m_param[4] = 90;
	m_param[5] = 20;
	m_param[6] = 600;
	m_param[7] = 180;
	m_param[8] = 15;
	m_bRomMode = false;
}

BEGIN_MESSAGE_MAP(ConfigPowerSavingParametersRomDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &ConfigPowerSavingParametersRomDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigPowerSavingParametersRomDlg 訊息處理常式
void ConfigPowerSavingParametersRomDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_POWERSAVING_1, m_param[0]);
	DDX_Text(pDX, IDC_POWERSAVING_2, m_param[1]);
	DDX_Text(pDX, IDC_POWERSAVING_3, m_param[2]);
	DDX_Text(pDX, IDC_POWERSAVING_4, m_param[3]);
	DDX_Text(pDX, IDC_POWERSAVING_5, m_param[4]);
	DDX_Text(pDX, IDC_POWERSAVING_6, m_param[5]);
	DDX_Text(pDX, IDC_POWERSAVING_7, m_param[6]);
	DDX_Text(pDX, IDC_POWERSAVING_8, m_param[7]);
	DDX_Text(pDX, IDC_POWERSAVING_9, m_param[8]);

	CDialog::DoDataExchange(pDX);
}

BOOL ConfigPowerSavingParametersRomDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	if(m_bRomMode)
	{
		GetDlgItem(IDC_POWERSAVING_8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_POWERSAVING_9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_9)->ShowWindow(SW_HIDE);
	}
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void ConfigPowerSavingParametersRomDlg::OnBnClickedOk()
{
	UpdateData();
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void ConfigPowerSavingParametersRomDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(21);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x0C;
	*cmd.GetBuffer(2) = m_param[0]>>8 & 0xFF;
	*cmd.GetBuffer(3) = m_param[0] & 0xFF;
	*cmd.GetBuffer(4) = m_param[1]>>8 & 0xFF;
	*cmd.GetBuffer(5) = m_param[1] & 0xFF;
	*cmd.GetBuffer(6) = m_param[2]>>8 & 0xFF;
	*cmd.GetBuffer(7) = m_param[2] & 0xFF;
	*cmd.GetBuffer(8) = m_param[3]>>8 & 0xFF;
	*cmd.GetBuffer(9) = m_param[3] & 0xFF;
	*cmd.GetBuffer(10) = m_param[4]>>8 & 0xFF;
	*cmd.GetBuffer(11) = m_param[4] & 0xFF;
	*cmd.GetBuffer(12) = m_param[5]>>8 & 0xFF;
	*cmd.GetBuffer(13) = m_param[5] & 0xFF;
	*cmd.GetBuffer(14) = m_param[6]>>8 & 0xFF;
	*cmd.GetBuffer(15) = m_param[6] & 0xFF;
	*cmd.GetBuffer(16) = m_param[7]>>8 & 0xFF;
	*cmd.GetBuffer(17) = m_param[7] & 0xFF;
	*cmd.GetBuffer(18) = m_param[8]>>8 & 0xFF;
	*cmd.GetBuffer(19) = m_param[8] & 0xFF;
	*cmd.GetBuffer(20) = m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "ConfigPowerSavingParametersRom successful...";
    AfxBeginThread(ConfigThread, 0);
}

