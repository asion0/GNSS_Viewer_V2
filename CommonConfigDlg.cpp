#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"
#include "CommonConfigDlg.h"

// CCommonConfigDlg 
static BinaryCommand configCmd;
static CString configPrompt;

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

	rcCtrl.SetRect(rcClient.right - 112, rcClient.bottom - 35, rcClient.right - 16, rcClient.bottom - 8);
	pCancelBtn->Create(_T("Cancel"), dwStyle, rcCtrl, this, IDCANCEL);

	rcCtrl.SetRect(rcClient.right - 224, rcClient.bottom - 35, rcClient.right - 128, rcClient.bottom - 8);
	pAcceptBtn->Create(_T("Accept"), dwStyle, rcCtrl, this, IDOK);

	return TRUE;  // return TRUE unless you set the focus to a control
}

INT_PTR CCommonConfigDlg::DoDirect(int type)
{
	return IDCANCEL;
}

// CConfigDGPS 
IMPLEMENT_DYNAMIC(CConfigDGPS, CCommonConfigDlg)

CConfigDGPS::CConfigDGPS(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_DGPS, pParent)
{
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigDGPS, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigDGPS::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigDGPS 
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
	configPrompt = "Configure DGPS successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigSmoothMode 
IMPLEMENT_DYNAMIC(CConfigSmoothMode, CCommonConfigDlg)

CConfigSmoothMode::CConfigSmoothMode(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_SMOOTH_MODE, pParent)
{
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigSmoothMode, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigSmoothMode::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigDGPS 
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
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x69;
	*cmd.GetBuffer(1) = 0x03;
	*cmd.GetBuffer(2) = (U08)m_bEnable;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure carrier smooth mode successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigTimeStamping 
IMPLEMENT_DYNAMIC(CConfigTimeStamping, CCommonConfigDlg)

CConfigTimeStamping::CConfigTimeStamping(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_TIME_STAMPING, pParent)
{
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigTimeStamping, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigTimeStamping::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigDGPS 
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
	configPrompt = "Configure time stamping successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigSBAS 
IMPLEMENT_DYNAMIC(CConfigSBAS, CCommonConfigDlg)

CConfigSBAS::CConfigSBAS(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_SBAS, pParent)
{
	m_bEnable = FALSE;
	m_bRanging = FALSE;
	m_bCorrection = FALSE;
	m_nUraMask = 0;
	m_nTrackingChannel = 0;
	m_bWAAS = FALSE;
	m_bEGNOS = FALSE;
	m_bMSAS = FALSE;
	m_bGAGAN = FALSE;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigSBAS, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigSBAS::OnBnClickedOk)
	ON_BN_CLICKED(IDC_ENABLE_WAAS, OnBnClickedEnableWaas)
	ON_BN_CLICKED(IDC_ENABLE_EGNOS, OnBnClickedEnableEgnos)
	ON_BN_CLICKED(IDC_ENABLE_MSAS, OnBnClickedEnableMsas)
	ON_BN_CLICKED(IDC_ENABLE_GAGAN, OnBnClickedEnableGagan)
	ON_BN_CLICKED(IDC_ENABLE_ALL, OnBnClickedEnableAll)
END_MESSAGE_MAP()

// CConfigSBAS 
BOOL CConfigSBAS::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

  BinaryData ackCmd;
  CString txt, title;
  this->GetWindowText(title);
	if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QuerySbas(CGPSDlg::Return, &ackCmd))
	{
    title += " (Query success)";
    ((CButton*)GetDlgItem(IDC_ENABLE_SBAS))->SetCheck(ackCmd[6]);
	  ((CComboBox*)GetDlgItem(IDC_ENABLE_NAV))->SetCurSel(ackCmd[7]);
    txt.Format("%d", ackCmd[8]);
	  GetDlgItem(IDC_URAMASK)->SetWindowText(txt);
    ((CButton*)GetDlgItem(IDC_ENABLE_CORRECTION))->SetCheck(ackCmd[9]);
    txt.Format("%d", ackCmd[10]);
	  GetDlgItem(IDC_NUMBER_CHANNEL)->SetWindowText(txt);

    ((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(((ackCmd[11] & 0x80) ? TRUE : FALSE));
	  if(!(ackCmd[11] & 0x80))
	  {
      ((CButton*)GetDlgItem(IDC_ENABLE_WAAS))->SetCheck(((ackCmd[11] & 0x01) ? TRUE : FALSE));
      ((CButton*)GetDlgItem(IDC_ENABLE_EGNOS))->SetCheck(((ackCmd[11] & 0x02) ? TRUE : FALSE));
      ((CButton*)GetDlgItem(IDC_ENABLE_MSAS))->SetCheck(((ackCmd[11] & 0x04) ? TRUE : FALSE));
      ((CButton*)GetDlgItem(IDC_ENABLE_GAGAN))->SetCheck(((ackCmd[11] & 0x08) ? TRUE : FALSE));
	  }
  }
  else
  {
    title += " (Query failed)";
	  GetDlgItem(IDC_NUMBER_CHANNEL)->SetWindowText("0");
	  GetDlgItem(IDC_URAMASK)->SetWindowText("8");
	  ((CComboBox*)GetDlgItem(IDC_ENABLE_NAV))->SetCurSel(0);
  }
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
  this->SetWindowText(title);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigSBAS::OnBnClickedOk()
{
	CString txt;
	GetDlgItem(IDC_URAMASK)->GetWindowText(txt);
	m_nUraMask = atoi(txt);
	if(m_nUraMask < 0 || m_nUraMask > 15)
	{
		AfxMessageBox("URA Mask must be between 0 and 15!");
		return;
	}

	GetDlgItem(IDC_NUMBER_CHANNEL)->GetWindowText(txt);
	m_nTrackingChannel = atoi(txt);
	if(m_nTrackingChannel < 0 || m_nTrackingChannel > 3)
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
	m_bGAGAN = ((CButton*)GetDlgItem(IDC_ENABLE_GAGAN))->GetCheck();
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

void CConfigSBAS::OnBnClickedEnableMsas()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_MSAS))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(FALSE);
	}
}

void CConfigSBAS::OnBnClickedEnableGagan()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_GAGAN))->GetCheck())
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
		((CButton*)GetDlgItem(IDC_ENABLE_GAGAN))->SetCheck(FALSE);
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
	*cmd.GetBuffer(7) = (U08)(m_bWAAS | (m_bEGNOS << 1) | (m_bMSAS << 2) | (m_bGAGAN << 3) | (m_bAll << 7));
	*cmd.GetBuffer(8) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure SBAS successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigSBAS2
IMPLEMENT_DYNAMIC(CConfigSBAS2, CCommonConfigDlg)

CConfigSBAS2::CConfigSBAS2(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_SBAS2, pParent)
{
	m_bEnable = FALSE;
	m_bRanging = FALSE;
	m_bCorrection = FALSE;
	m_nUraMask = 0;
	m_nTrackingChannel = 0;
	m_bWAAS = FALSE;
	m_bEGNOS = FALSE;
	m_bMSAS = FALSE;
	m_bGAGAN = FALSE;
	m_nAttribute = 0;
  cmdMode = (NEW_SBAS2) ? ConfigSBAS2New : ConfigSBAS2Old;
}

BEGIN_MESSAGE_MAP(CConfigSBAS2, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigSBAS2::OnBnClickedOk)
	ON_BN_CLICKED(IDC_ENABLE_WAAS, OnBnClickedEnableWaas)
	ON_BN_CLICKED(IDC_ENABLE_EGNOS, OnBnClickedEnableEgnos)
	ON_BN_CLICKED(IDC_ENABLE_MSAS, OnBnClickedEnableMsas)
	ON_BN_CLICKED(IDC_ENABLE_GAGAN, OnBnClickedEnableGagan)
	ON_BN_CLICKED(IDC_ENABLE_ALL, OnBnClickedEnableAll)
	ON_BN_CLICKED(IDC_WAAS_SPEC, OnBnClickedWaasSpecify)
	ON_BN_CLICKED(IDC_EGNOS_SPEC, OnBnClickedEgnosSpecify)
	ON_BN_CLICKED(IDC_MSAS_SPEC, OnBnClickedMsasSpecify)
	ON_BN_CLICKED(IDC_GAGAN_SPEC, OnBnClickedGaganSpecify)
	ON_BN_CLICKED(IDC_APPLY_DEFAULT, &CConfigSBAS2::OnBnClickedApplyDefault)
END_MESSAGE_MAP()

// CConfigSBAS2 
BOOL CConfigSBAS2::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

  BinaryData ackCmd;
  CString txt, title;
  this->GetWindowText(title);

#if(NEW_SBAS2)
    GetDlgItem(IDC_WAAS_SPEC)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EGNOS_SPEC)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_MSAS_SPEC)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GAGAN_SPEC)->ShowWindow(SW_HIDE);
#endif

  CGPSDlg::CmdErrorCode err = CGPSDlg::gpsDlg->QuerySbas2(CGPSDlg::Return, &ackCmd);

	if(CGPSDlg::Ack == err)
	{
    U16 cmdLen = ConvertLeonU16(ackCmd.Ptr(2));
    cmdMode = (cmdLen == 24) ? ConfigSBAS2New : ConfigSBAS2Old;
    if(cmdMode == ConfigSBAS2Old)
    {
      GetDlgItem(IDC_WAAS_SPEC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_EGNOS_SPEC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_MSAS_SPEC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_GAGAN_SPEC)->ShowWindow(SW_SHOW);
    }

    int idx = 5;
    title += " (Query success)";
    ((CButton*)GetDlgItem(IDC_ENABLE_SBAS))->SetCheck(ackCmd[++idx]);
	  ((CComboBox*)GetDlgItem(IDC_ENABLE_NAV))->SetCurSel(ackCmd[++idx]);
    txt.Format("%d", ackCmd[++idx]);
	  GetDlgItem(IDC_URAMASK)->SetWindowText(txt);
    ((CButton*)GetDlgItem(IDC_ENABLE_CORRECTION))->SetCheck(ackCmd[++idx]);
    txt.Format("%d", ackCmd[++idx]);
	  GetDlgItem(IDC_NUMBER_CHANNEL)->SetWindowText(txt);
    U08 flag = ackCmd[++idx];   //11
    ((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(((flag & 0x80) ? TRUE : FALSE));
	  if(!(flag & 0x80))
	  {
      ((CButton*)GetDlgItem(IDC_ENABLE_WAAS))->SetCheck(((flag & 0x01) ? TRUE : FALSE));
      ((CButton*)GetDlgItem(IDC_ENABLE_EGNOS))->SetCheck(((flag & 0x02) ? TRUE : FALSE));
      ((CButton*)GetDlgItem(IDC_ENABLE_MSAS))->SetCheck(((flag & 0x04) ? TRUE : FALSE));
      ((CButton*)GetDlgItem(IDC_ENABLE_GAGAN))->SetCheck(((flag & 0x08) ? TRUE : FALSE));
	  }

    if(cmdMode == ConfigSBAS2Old)
    {
      U08 flagUser = ackCmd[++idx]; //12
      ((CButton*)GetDlgItem(IDC_WAAS_SPEC))->SetCheck(((flagUser & 0x01) ? TRUE : FALSE));
      ((CButton*)GetDlgItem(IDC_EGNOS_SPEC))->SetCheck(((flagUser & 0x02) ? TRUE : FALSE));
      ((CButton*)GetDlgItem(IDC_MSAS_SPEC))->SetCheck(((flagUser & 0x04) ? TRUE : FALSE));
      ((CButton*)GetDlgItem(IDC_GAGAN_SPEC))->SetCheck(((flagUser & 0x08) ? TRUE : FALSE));
    }

    U08 count = ackCmd[++idx];
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U11)->SetWindowText((count > 0) ? txt : "");
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U12)->SetWindowText((count > 1) ? txt : "");
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U13)->SetWindowText((count > 2) ? txt : "");

    count = ackCmd[++idx];
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U21)->SetWindowText((count > 0) ? txt : "");
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U22)->SetWindowText((count > 1) ? txt : "");
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U23)->SetWindowText((count> 2) ? txt : "");

    count = ackCmd[++idx];
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U31)->SetWindowText((count > 0) ? txt : "");
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U32)->SetWindowText((count > 1) ? txt : "");
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U33)->SetWindowText((count > 2) ? txt : "");

    count = ackCmd[++idx];
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U41)->SetWindowText((count > 0) ? txt : "");
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U42)->SetWindowText((count > 1) ? txt : "");
    txt.Format("%d", ackCmd[++idx]);
    GetDlgItem(IDC_U43)->SetWindowText((count > 2) ? txt : "");
  }
  else
  {
    title += " (Query failed)";
	  GetDlgItem(IDC_NUMBER_CHANNEL)->SetWindowText("0");
	  GetDlgItem(IDC_URAMASK)->SetWindowText("8");
	  ((CComboBox*)GetDlgItem(IDC_ENABLE_NAV))->SetCurSel(0);

	  GetDlgItem(IDC_U11)->SetWindowText("");
	  GetDlgItem(IDC_U12)->SetWindowText("");
	  GetDlgItem(IDC_U13)->SetWindowText("");
	  GetDlgItem(IDC_U21)->SetWindowText("");
	  GetDlgItem(IDC_U22)->SetWindowText("");
	  GetDlgItem(IDC_U23)->SetWindowText("");
	  GetDlgItem(IDC_U31)->SetWindowText("");
	  GetDlgItem(IDC_U32)->SetWindowText("");
	  GetDlgItem(IDC_U33)->SetWindowText("");
	  GetDlgItem(IDC_U41)->SetWindowText("");
	  GetDlgItem(IDC_U42)->SetWindowText("");
	  GetDlgItem(IDC_U43)->SetWindowText("");
  }

	if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QuerySbasDefault(CGPSDlg::Return, &ackCmd))
	{
    txt.Format("%d", ackCmd[7]);
    GetDlgItem(IDC_D11)->SetWindowText((ackCmd[6] > 0) ? txt : "");
    txt.Format("%d", ackCmd[8]);
    GetDlgItem(IDC_D12)->SetWindowText((ackCmd[6] > 1) ? txt : "");
    txt.Format("%d", ackCmd[9]);
    GetDlgItem(IDC_D13)->SetWindowText((ackCmd[6] > 2) ? txt : "");

    txt.Format("%d", ackCmd[11]);
    GetDlgItem(IDC_D21)->SetWindowText((ackCmd[10] > 0) ? txt : "");
    txt.Format("%d", ackCmd[12]);
    GetDlgItem(IDC_D22)->SetWindowText((ackCmd[10] > 1) ? txt : "");
    txt.Format("%d", ackCmd[13]);
    GetDlgItem(IDC_D23)->SetWindowText((ackCmd[10] > 2) ? txt : "");

    txt.Format("%d", ackCmd[15]);
    GetDlgItem(IDC_D31)->SetWindowText((ackCmd[14] > 0) ? txt : "");
    txt.Format("%d", ackCmd[16]);
    GetDlgItem(IDC_D32)->SetWindowText((ackCmd[14] > 1) ? txt : "");
    txt.Format("%d", ackCmd[17]);
    GetDlgItem(IDC_D33)->SetWindowText((ackCmd[14] > 2) ? txt : "");

    txt.Format("%d", ackCmd[19]);
    GetDlgItem(IDC_D41)->SetWindowText((ackCmd[18] > 0) ? txt : "");
    txt.Format("%d", ackCmd[20]);
    GetDlgItem(IDC_D42)->SetWindowText((ackCmd[18] > 1) ? txt : "");
    txt.Format("%d", ackCmd[21]);
    GetDlgItem(IDC_D43)->SetWindowText((ackCmd[18] > 2) ? txt : "");
  }
  else
  {
	  GetDlgItem(IDC_D11)->SetWindowText("");
	  GetDlgItem(IDC_D12)->SetWindowText("");
	  GetDlgItem(IDC_D13)->SetWindowText("");
	  GetDlgItem(IDC_D21)->SetWindowText("");
	  GetDlgItem(IDC_D22)->SetWindowText("");
	  GetDlgItem(IDC_D23)->SetWindowText("");
	  GetDlgItem(IDC_D31)->SetWindowText("");
	  GetDlgItem(IDC_D32)->SetWindowText("");
	  GetDlgItem(IDC_D33)->SetWindowText("");
	  GetDlgItem(IDC_D41)->SetWindowText("");
	  GetDlgItem(IDC_D42)->SetWindowText("");
	  GetDlgItem(IDC_D43)->SetWindowText("");
	  GetDlgItem(IDC_APPLY_DEFAULT)->EnableWindow(FALSE);
  }
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
  this->SetWindowText(title);
  UpdateStatus();
	return TRUE;  // return TRUE unless you set the focus to a control
}


int CConfigSBAS2::GetUserTaqble(U08& u1, U08& u2, U08& u3, UINT id1, UINT id2, UINT id3)
{
  CString txt;
	GetDlgItem(id1)->GetWindowText(txt);
  int i1 = atoi(txt);
	GetDlgItem(id2)->GetWindowText(txt);
  int i2 = atoi(txt);
	GetDlgItem(id3)->GetWindowText(txt);
  int i3 = atoi(txt);

  if(i1 > 255 || i1 < 0 || i2 > 255 || i2 < 0 || i3 > 255 || i3 < 0)
  {
    return -1;
  }

  if(i1 != 0 && i2 != 0 && i3 != 0)
  {
    u1 = i1; u2 = i2; u3 = i3;
    return 3;
  }

  if(i1 == 0 && i2 == 0 && i3 == 0)
  {
    u1 = i1; u2 = i2; u3 = i3;
    return 0;
  }

  int n;
  if(i1 == 0)
  {
    if(i2 == 0)
    {
      i1 = i3;
      i2 = i3 = 0;
      n = 1;
    }
    else
    {
      i1 = i2;
      i2 = i3;
      i3 = 0;
      n = (i2 == 0) ? 1 : 2;
    }
  }
  else 
  {
    if(i2 == 0)
    {
      i2 = i3;
      i3 = 0;
      n = (i2 == 0) ? 1 : 2;
    }
    else
    {
      n = 2;
    }
  }
  u1 = i1; u2 = i2; u3 = i3;
  return n;
}

void CConfigSBAS2::OnBnClickedOk()
{
	CString txt;
	GetDlgItem(IDC_URAMASK)->GetWindowText(txt);
	m_nUraMask = atoi(txt);
	if(m_nUraMask < 0 || m_nUraMask > 15)
	{
		AfxMessageBox("URA Mask must be between 0 and 15!");
		return;
	}

	GetDlgItem(IDC_NUMBER_CHANNEL)->GetWindowText(txt);
	m_nTrackingChannel = atoi(txt);
	if(m_nTrackingChannel < 0 || m_nTrackingChannel > 3)
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
	m_bGAGAN = ((CButton*)GetDlgItem(IDC_ENABLE_GAGAN))->GetCheck();
	m_bAll = ((CButton*)GetDlgItem(IDC_ENABLE_ALL))->GetCheck();

  if(cmdMode == ConfigSBAS2Old)
  {
    m_bWaasSpec = ((CButton*)GetDlgItem(IDC_WAAS_SPEC))->GetCheck();
    m_bEgnosSpec = ((CButton*)GetDlgItem(IDC_EGNOS_SPEC))->GetCheck();
    m_bMsasSpec = ((CButton*)GetDlgItem(IDC_MSAS_SPEC))->GetCheck();
    m_bGaganSpec = ((CButton*)GetDlgItem(IDC_GAGAN_SPEC))->GetCheck();
  }

  int n;
  n = GetUserTaqble(m_u11, m_u12, m_u13, IDC_U11, IDC_U12, IDC_U13);
  if(n == -1)
  {
		AfxMessageBox("Invalid user subsystem mask!");
		return;
  }
  if(NEW_SBAS2 && cmdMode == ConfigSBAS2New)
  {
    if(m_bWAAS && n == 0)
    {
		  AfxMessageBox("Please set at least one user subsystem mask!");
		  return;
    }
  }

  n = GetUserTaqble(m_u21, m_u22, m_u23, IDC_U21, IDC_U22, IDC_U23);
  if(n == -1)
  {
		AfxMessageBox("Invalid user subsystem mask!");
		return;
  }
  if(NEW_SBAS2 && cmdMode == ConfigSBAS2New)
  {
    if(m_bEGNOS && n == 0)
    {
		  AfxMessageBox("Please set at least one user subsystem mask!");
		  return;
    }
  }

  n = GetUserTaqble(m_u31, m_u32, m_u33, IDC_U31, IDC_U32, IDC_U33);
  if(n == -1)
  {
		AfxMessageBox("Invalid user subsystem mask!");
		return;
  }
  if(NEW_SBAS2 && cmdMode == ConfigSBAS2New)
  {
    if(m_bMSAS && n == 0)
    {
		  AfxMessageBox("Please set at least one user subsystem mask!");
		  return;
    }
  }

  n = GetUserTaqble(m_u41, m_u42, m_u43, IDC_U41, IDC_U42, IDC_U43);
  if(n == -1)
  {
		AfxMessageBox("Invalid user subsystem mask!");
		return;
  }
  if(NEW_SBAS2 && cmdMode == ConfigSBAS2New)
  {
    if(m_bGAGAN && n == 0)
    {
		  AfxMessageBox("Please set at least one user subsystem mask!");
		  return;
    }
  }

	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();
	OnOK();
}

void CConfigSBAS2::UpdateStatus()
{
	BOOL enableWass = ((CButton*)GetDlgItem(IDC_ENABLE_WAAS))->GetCheck();
	BOOL enableEgnos = ((CButton*)GetDlgItem(IDC_ENABLE_EGNOS))->GetCheck();
	BOOL enableMsas = ((CButton*)GetDlgItem(IDC_ENABLE_MSAS))->GetCheck();
	BOOL enableGagan = ((CButton*)GetDlgItem(IDC_ENABLE_GAGAN))->GetCheck();
	BOOL enableAll = ((CButton*)GetDlgItem(IDC_ENABLE_ALL))->GetCheck();

  if(cmdMode == ConfigSBAS2Old || !NEW_SBAS2)
  {
    BOOL specWass = ((CButton*)GetDlgItem(IDC_WAAS_SPEC))->GetCheck();
	  BOOL specEgnos = ((CButton*)GetDlgItem(IDC_EGNOS_SPEC))->GetCheck();
	  BOOL specMsas = ((CButton*)GetDlgItem(IDC_MSAS_SPEC))->GetCheck();
	  BOOL specGagan = ((CButton*)GetDlgItem(IDC_GAGAN_SPEC))->GetCheck();

    ((CButton*)GetDlgItem(IDC_WAAS_SPEC))->EnableWindow(enableWass);
    ((CButton*)GetDlgItem(IDC_EGNOS_SPEC))->EnableWindow(enableEgnos);
    ((CButton*)GetDlgItem(IDC_MSAS_SPEC))->EnableWindow(enableMsas);
    ((CButton*)GetDlgItem(IDC_GAGAN_SPEC))->EnableWindow(enableGagan);

    ((CEdit*)GetDlgItem(IDC_U11))->EnableWindow(enableWass && specWass);
    ((CEdit*)GetDlgItem(IDC_U12))->EnableWindow(enableWass && specWass);
    ((CEdit*)GetDlgItem(IDC_U13))->EnableWindow(enableWass && specWass);
    ((CEdit*)GetDlgItem(IDC_U21))->EnableWindow(enableEgnos && specEgnos);
    ((CEdit*)GetDlgItem(IDC_U22))->EnableWindow(enableEgnos && specEgnos);
    ((CEdit*)GetDlgItem(IDC_U23))->EnableWindow(enableEgnos && specEgnos);
    ((CEdit*)GetDlgItem(IDC_U31))->EnableWindow(enableMsas && specMsas);
    ((CEdit*)GetDlgItem(IDC_U32))->EnableWindow(enableMsas && specMsas);
    ((CEdit*)GetDlgItem(IDC_U33))->EnableWindow(enableMsas && specMsas);
    ((CEdit*)GetDlgItem(IDC_U41))->EnableWindow(enableGagan && specGagan);
    ((CEdit*)GetDlgItem(IDC_U42))->EnableWindow(enableGagan && specGagan);
    ((CEdit*)GetDlgItem(IDC_U43))->EnableWindow(enableGagan && specGagan);
  }
  else
  {
    ((CEdit*)GetDlgItem(IDC_U11))->EnableWindow(enableWass);
    ((CEdit*)GetDlgItem(IDC_U12))->EnableWindow(enableWass);
    ((CEdit*)GetDlgItem(IDC_U13))->EnableWindow(enableWass);
    ((CEdit*)GetDlgItem(IDC_U21))->EnableWindow(enableEgnos);
    ((CEdit*)GetDlgItem(IDC_U22))->EnableWindow(enableEgnos);
    ((CEdit*)GetDlgItem(IDC_U23))->EnableWindow(enableEgnos);
    ((CEdit*)GetDlgItem(IDC_U31))->EnableWindow(enableMsas);
    ((CEdit*)GetDlgItem(IDC_U32))->EnableWindow(enableMsas);
    ((CEdit*)GetDlgItem(IDC_U33))->EnableWindow(enableMsas);
    ((CEdit*)GetDlgItem(IDC_U41))->EnableWindow(enableGagan);
    ((CEdit*)GetDlgItem(IDC_U42))->EnableWindow(enableGagan);
    ((CEdit*)GetDlgItem(IDC_U43))->EnableWindow(enableGagan);
  }
}

void CConfigSBAS2::OnBnClickedEnableWaas()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_WAAS))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(FALSE);
	}
  UpdateStatus();
}

void CConfigSBAS2::OnBnClickedEnableEgnos()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_EGNOS))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(FALSE);
	}
  UpdateStatus();
}

void CConfigSBAS2::OnBnClickedEnableMsas()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_MSAS))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(FALSE);
	}
  UpdateStatus();
}

void CConfigSBAS2::OnBnClickedEnableGagan()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_GAGAN))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ALL))->SetCheck(FALSE);
	}
  UpdateStatus();
}

void CConfigSBAS2::OnBnClickedEnableAll()
{
	if(((CButton*)GetDlgItem(IDC_ENABLE_ALL))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_ENABLE_WAAS))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_ENABLE_EGNOS))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_ENABLE_MSAS))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_ENABLE_GAGAN))->SetCheck(FALSE);
	}
  UpdateStatus();
}

void CConfigSBAS2::OnBnClickedWaasSpecify()
{
  UpdateStatus();
}

void CConfigSBAS2::OnBnClickedEgnosSpecify()
{
  UpdateStatus();
}

void CConfigSBAS2::OnBnClickedMsasSpecify()
{
  UpdateStatus();
}

void CConfigSBAS2::OnBnClickedGaganSpecify()
{
  UpdateStatus();
}

void CConfigSBAS2::OnBnClickedApplyDefault()
{
  CString strTxt;
  ((CEdit*)GetDlgItem(IDC_D11))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U11))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D12))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U12))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D13))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U13))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D21))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U21))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D22))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U22))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D23))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U23))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D31))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U31))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D32))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U32))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D33))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U33))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D41))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U41))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D42))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U42))->SetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_D43))->GetWindowText(strTxt);
  ((CEdit*)GetDlgItem(IDC_U43))->SetWindowText(strTxt);
}

void CConfigSBAS2::DoCommand()
{
  BinaryData cmd((cmdMode == ConfigSBAS2New) ? 21 : 22);

  int idx = -1;
	*cmd.GetBuffer(++idx) = 0x62;
	*cmd.GetBuffer(++idx) = 0x05;
	*cmd.GetBuffer(++idx) = (U08)m_bEnable;
	*cmd.GetBuffer(++idx) = (U08)m_bRanging;
	*cmd.GetBuffer(++idx) = (U08)m_nUraMask;
	*cmd.GetBuffer(++idx) = (U08)m_bCorrection;
	*cmd.GetBuffer(++idx) = (U08)m_nTrackingChannel;
	*cmd.GetBuffer(++idx) = (U08)(m_bWAAS | (m_bEGNOS << 1) | (m_bMSAS << 2) | (m_bGAGAN << 3) | (m_bAll << 7));
  if(cmdMode == ConfigSBAS2Old)
  {
	  *cmd.GetBuffer(++idx) = (U08)(m_bWaasSpec | (m_bEgnosSpec << 1) | (m_bMsasSpec << 2) | (m_bGaganSpec << 3));
  }
	*cmd.GetBuffer(++idx) = (U08)m_u11;
	*cmd.GetBuffer(++idx) = (U08)m_u12;
	*cmd.GetBuffer(++idx) = (U08)m_u13;
	*cmd.GetBuffer(++idx) = (U08)m_u21;
	*cmd.GetBuffer(++idx) = (U08)m_u22;
	*cmd.GetBuffer(++idx) = (U08)m_u23;
	*cmd.GetBuffer(++idx) = (U08)m_u31;
	*cmd.GetBuffer(++idx) = (U08)m_u32;
	*cmd.GetBuffer(++idx) = (U08)m_u33;
	*cmd.GetBuffer(++idx) = (U08)m_u41;
	*cmd.GetBuffer(++idx) = (U08)m_u42;
	*cmd.GetBuffer(++idx) = (U08)m_u43;
	*cmd.GetBuffer(++idx) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure SBAS advance successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigSAEE 
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

// CConfigSAEE 
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
	configPrompt = "Configure SAEE successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigQZSS 
IMPLEMENT_DYNAMIC(CConfigQZSS, CCommonConfigDlg)

CConfigQZSS::CConfigQZSS(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_QZSS, pParent)
{
	m_bEnable = FALSE;
	m_nTrackingChannel = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigQZSS, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigQZSS::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigQZSS 
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
	configPrompt = "Configure QZSS successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigInterferenceDetectControl 
IMPLEMENT_DYNAMIC(CConfigInterferenceDetectControl, CCommonConfigDlg)

CConfigInterferenceDetectControl::CConfigInterferenceDetectControl(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_INF_DTE_CTRL, pParent)
{
	m_nMode = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigInterferenceDetectControl, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigInterferenceDetectControl::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigInterferenceDetectControl 
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
	configPrompt = "Configure interference detect control successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigNMEABinaryOutputDestination 
IMPLEMENT_DYNAMIC(CConfigNMEABinaryOutputDestination, CCommonConfigDlg)

CConfigNMEABinaryOutputDestination::CConfigNMEABinaryOutputDestination(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_NMBI_OUT_DES, pParent)
{
	m_nMode = 0;
	m_nAttribute = 0;
}


BEGIN_MESSAGE_MAP(CConfigNMEABinaryOutputDestination, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigNMEABinaryOutputDestination::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigNMEABinaryOutputDestination 
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
	configPrompt = "Configure NMEA binary output destination successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigParameterSearchEngineNumber 
IMPLEMENT_DYNAMIC(CConfigParameterSearchEngineNumber, CCommonConfigDlg)

CConfigParameterSearchEngineNumber::CConfigParameterSearchEngineNumber(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_PARAM_SEARCH_ENG_NUM, pParent)
{
	m_nMode = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigParameterSearchEngineNumber, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigParameterSearchEngineNumber::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_MODE, &CConfigParameterSearchEngineNumber::OnCbnSelchangeMode)
END_MESSAGE_MAP()

// CConfigParameterSearchEngineNumber 
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
	configPrompt = "Configure parameter search engine number successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigPositionFixNavigationMask 
IMPLEMENT_DYNAMIC(CConfigPositionFixNavigationMask, CCommonConfigDlg)

CConfigPositionFixNavigationMask::CConfigPositionFixNavigationMask(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_POS_FIX_NAV_MASK, pParent)
{
	m_nMask1 = 0;
	m_nMask2 = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigPositionFixNavigationMask, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigPositionFixNavigationMask::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigPositionFixNavigationMask 
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
	configPrompt = "Configure position fix navigation mask successfully";
  AfxBeginThread(ConfigThread, 0);
}

// ConfigRefTimeToGpsTimeDlg 
IMPLEMENT_DYNAMIC(ConfigRefTimeToGpsTimeDlg, CCommonConfigDlg)

ConfigRefTimeToGpsTimeDlg::ConfigRefTimeToGpsTimeDlg(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_REFTIME2GPSTIME, pParent)
{

}

BEGIN_MESSAGE_MAP(ConfigRefTimeToGpsTimeDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &ConfigRefTimeToGpsTimeDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigRefTimeToGpsTimeDlg 
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
	configPrompt = "Configure ref. time sync to GPS time successfully";
  AfxBeginThread(ConfigThread, 0);
}

// ConfigGnssConstellationTypeDlg 
IMPLEMENT_DYNAMIC(ConfigGnssConstellationTypeDlg, CCommonConfigDlg)

ConfigGnssConstellationTypeDlg::ConfigGnssConstellationTypeDlg(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_GNSS_CTLN_TPE, pParent)
{

}

BEGIN_MESSAGE_MAP(ConfigGnssConstellationTypeDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &ConfigGnssConstellationTypeDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigRefTimeToGpsTimeDlg 
BOOL ConfigGnssConstellationTypeDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
  CString title;
  BinaryData ackCmd;
  this->GetWindowText(title);

  if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QueryGnssConstellationType(CGPSDlg::Return, &ackCmd))
  {
    U16 mode = MAKEWORD(ackCmd[7], ackCmd[6]);
    title += " (Query success)";
    ((CButton*)GetDlgItem(IDC_GPS))->SetCheck((mode & 0x0001) ? 1 : 0);
	  ((CButton*)GetDlgItem(IDC_GLONASS))->SetCheck((mode & 0x0002) ? 1 : 0);
	  ((CButton*)GetDlgItem(IDC_GALILEO))->SetCheck((mode & 0x0004) ? 1 : 0);
	  ((CButton*)GetDlgItem(IDC_BEIDOU))->SetCheck((mode & 0x0008) ? 1 : 0);
	  ((CButton*)GetDlgItem(IDC_NAVIC))->SetCheck((mode & 0x0010) ? 1 : 0);
    ((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
  }
  else
  {
    title += " (Query failed)";
	  ((CButton*)GetDlgItem(IDC_GPS))->SetCheck(1);
	  ((CButton*)GetDlgItem(IDC_GLONASS))->SetCheck(0);
	  ((CButton*)GetDlgItem(IDC_GALILEO))->SetCheck(0);
	  ((CButton*)GetDlgItem(IDC_BEIDOU))->SetCheck(0);
    ((CButton*)GetDlgItem(IDC_NAVIC))->SetCheck(0);
    ((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
  }
  this->SetWindowText(title);

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
	if(((CButton*)GetDlgItem(IDC_NAVIC))->GetCheck())
	{
		m_mode |= 0x10;
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
	configPrompt = "Configure GNSS constellation type successfully";
  AfxBeginThread(ConfigThread, 0);
}

// ConfigBinaryMeasurementDataOutDlg 
IMPLEMENT_DYNAMIC(ConfigBinaryMeasurementDataOutDlg, CCommonConfigDlg)

ConfigBinaryMeasurementDataOutDlg::ConfigBinaryMeasurementDataOutDlg(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_BIN_MEA_DAT_OUT, pParent)
{
  m_newCmd = TRUE;
}

BEGIN_MESSAGE_MAP(ConfigBinaryMeasurementDataOutDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDC_MEAS_TIME, &ConfigBinaryMeasurementDataOutDlg::OnBnClickedMeasTime)
	ON_BN_CLICKED(IDC_RAW_MEAS, &ConfigBinaryMeasurementDataOutDlg::OnBnClickedRawMeas)
	ON_BN_CLICKED(IDC_EXT_RAW_MEAS, &ConfigBinaryMeasurementDataOutDlg::OnBnClickedExtRawMeas)
	ON_BN_CLICKED(IDOK, &ConfigBinaryMeasurementDataOutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigBinaryMeasurementDataOutDlg
BOOL ConfigBinaryMeasurementDataOutDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

  BinaryData ackCmd;
	U16 cmdLen = 7;
  CString title;
  this->GetWindowText(title);
	if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QueryBinaryMeasurementDataOut(CGPSDlg::Return, &ackCmd))
	{
    title += " (Query success)";
    cmdLen = ConvertLeonU16(ackCmd.Ptr(2));
    m_newCmd = (cmdLen == 7) ? FALSE : TRUE;
	  ((CComboBox*)GetDlgItem(IDC_OUTPUT_RATE))->SetCurSel(ackCmd[5]);
	  ((CButton*)GetDlgItem(IDC_MEAS_TIME))->SetCheck(ackCmd[6]);
	  ((CButton*)GetDlgItem(IDC_RAW_MEAS))->SetCheck(ackCmd[7]);
	  ((CButton*)GetDlgItem(IDC_SV_CH))->SetCheck(ackCmd[8]);
	  ((CButton*)GetDlgItem(IDC_RCV_STATE))->SetCheck(ackCmd[9]);
    ((CButton*)GetDlgItem(IDC_EXT_RAW_MEAS))->SetCheck((m_newCmd) ? ackCmd[11] : 0);

    if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QueryConstellationCapability(CGPSDlg::Return, &ackCmd))
    {
      U16 mode = MAKEWORD(ackCmd[7], ackCmd[6]);
      ((CButton*)GetDlgItem(IDC_GPS))->SetCheck((mode & 0x0001));
      ((CButton*)GetDlgItem(IDC_GLONASS))->SetCheck((mode & 0x0002));
      ((CButton*)GetDlgItem(IDC_GALILEO))->SetCheck((mode & 0x0004));
      ((CButton*)GetDlgItem(IDC_BEIDOU))->SetCheck((mode & 0x0008));
    }
    else
    {
      ((CButton*)GetDlgItem(IDC_GPS))->SetCheck(1);
    }
	  ((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
	}
  else
  {
    title += " (Query failed)";
	  ((CComboBox*)GetDlgItem(IDC_OUTPUT_RATE))->SetCurSel(0);
	  ((CButton*)GetDlgItem(IDC_MEAS_TIME))->SetCheck(1);
	  ((CButton*)GetDlgItem(IDC_RAW_MEAS))->SetCheck(1);
	  ((CButton*)GetDlgItem(IDC_SV_CH))->SetCheck(1);
	  ((CButton*)GetDlgItem(IDC_RCV_STATE))->SetCheck(1);
    ((CButton*)GetDlgItem(IDC_EXT_RAW_MEAS))->SetCheck(0);
    ((CButton*)GetDlgItem(IDC_GPS))->SetCheck(1);
	  ((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
 }

  this->SetWindowText(title);
	UpdateStatus();
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL ConfigBinaryMeasurementDataOutDlg::CheckLegal()
{
	BOOL measTime = ((CButton*)GetDlgItem(IDC_MEAS_TIME))->GetCheck();
	BOOL rawMeas = ((CButton*)GetDlgItem(IDC_RAW_MEAS))->GetCheck();
	BOOL extRawMeas = ((CButton*)GetDlgItem(IDC_EXT_RAW_MEAS))->GetCheck();

  if((measTime || rawMeas) && extRawMeas)
  {
    return FALSE;
  }
  return TRUE;
}

void ConfigBinaryMeasurementDataOutDlg::OnBnClickedMeasTime()
{
	if(!m_newCmd)
  {
    return;
  }
	if(((CButton*)GetDlgItem(IDC_MEAS_TIME))->GetCheck() && !CheckLegal())
  {
    ::AfxMessageBox("You can not enable Meas Time or Raw Meas when Extended Raw Meas is enabled!");
    ((CButton*)GetDlgItem(IDC_MEAS_TIME))->SetCheck(FALSE);
  }
}

void ConfigBinaryMeasurementDataOutDlg::OnBnClickedRawMeas()
{
	if(!m_newCmd)
  {
    return;
  }
	if(((CButton*)GetDlgItem(IDC_RAW_MEAS))->GetCheck() && !CheckLegal())
  {
    ::AfxMessageBox("You can not enable Meas Time or Raw Meas when Extended Raw Meas is enabled!");
    ((CButton*)GetDlgItem(IDC_RAW_MEAS))->SetCheck(FALSE);
  }
}

void ConfigBinaryMeasurementDataOutDlg::OnBnClickedExtRawMeas()
{
	if(!m_newCmd)
  {
    return;
  }
	if(((CButton*)GetDlgItem(IDC_EXT_RAW_MEAS))->GetCheck() && !CheckLegal())
  {
    ::AfxMessageBox("You can not enable Meas Time or Raw Meas when Extended Raw Meas is enabled!");
    ((CButton*)GetDlgItem(IDC_EXT_RAW_MEAS))->SetCheck(FALSE);
  }
}

void ConfigBinaryMeasurementDataOutDlg::OnBnClickedOk()
{
	m_rate = ((CComboBox*)GetDlgItem(IDC_OUTPUT_RATE))->GetCurSel();
	m_measTime = ((CButton*)GetDlgItem(IDC_MEAS_TIME))->GetCheck();
	m_rawMeas = ((CButton*)GetDlgItem(IDC_RAW_MEAS))->GetCheck();
	m_svChStatus = ((CButton*)GetDlgItem(IDC_SV_CH))->GetCheck();
	m_rcvChStatus = ((CButton*)GetDlgItem(IDC_RCV_STATE))->GetCheck();
	m_extRawMeas = ((CButton*)GetDlgItem(IDC_EXT_RAW_MEAS))->GetCheck();

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

void ConfigBinaryMeasurementDataOutDlg::UpdateStatus()
{
  ((CButton*)GetDlgItem(IDC_EXT_RAW_MEAS))->EnableWindow((m_newCmd) ? TRUE : FALSE);
}

void ConfigBinaryMeasurementDataOutDlg::DoCommand()
{
	BinaryData cmd((m_newCmd) ? 9 : 8);
	*cmd.GetBuffer(0) = 0x1E;
	*cmd.GetBuffer(1) = (U08)m_rate;
	*cmd.GetBuffer(2) = (U08)m_measTime;
	*cmd.GetBuffer(3) = (U08)m_rawMeas;
	*cmd.GetBuffer(4) = (U08)m_svChStatus;
	*cmd.GetBuffer(5) = (U08)m_rcvChStatus;
	*cmd.GetBuffer(6) = (U08)m_subFrame;
	if(m_newCmd)
  {
	  *cmd.GetBuffer(7) = (U08)m_extRawMeas;
  }
	*cmd.GetBuffer((m_newCmd) ? 8 : 7) = (U08)m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure binary measurement data out successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigLeapSeconds 
IMPLEMENT_DYNAMIC(CConfigLeapSeconds, CCommonConfigDlg)

CConfigLeapSeconds::CConfigLeapSeconds(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CFG_LEAP_SECONDS, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigLeapSeconds, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigLeapSeconds::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigRefTimeToGpsTimeDlg 
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
	configPrompt = "Configure GPS/UTC leap seconds successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigPowerMode 

IMPLEMENT_DYNAMIC(CConfigPowerMode, CCommonConfigDlg)

CConfigPowerMode::CConfigPowerMode(CWnd* pParent)
: CCommonConfigDlg(IDD_CONFIG_POWER_MODE, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigPowerMode, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigPowerMode::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigRefTimeToGpsTimeDlg 
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
	configPrompt = "Configure power mode successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigParamSearchEngineSleepCriteria 
IMPLEMENT_DYNAMIC(CConfigParamSearchEngineSleepCriteria, CCommonConfigDlg)

CConfigParamSearchEngineSleepCriteria::CConfigParamSearchEngineSleepCriteria(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CFG_PRM_SRCH_ENG_SLP_CRT, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigParamSearchEngineSleepCriteria, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigParamSearchEngineSleepCriteria::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigParamSearchEngineSleepCriteria 
BOOL CConfigParamSearchEngineSleepCriteria::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	GetDlgItem(IDC_TRACKED_NUM)->SetWindowText("10");
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigParamSearchEngineSleepCriteria::OnBnClickedOk()
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

void CConfigParamSearchEngineSleepCriteria::DoCommand()
{
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x25;
	*cmd.GetBuffer(2) = (U08)m_nTrackedNumber;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure param search engine sleep criteria successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigDatumIndex 
IMPLEMENT_DYNAMIC(CConfigDatumIndex, CCommonConfigDlg)

CConfigDatumIndex::CConfigDatumIndex(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CFG_DATUM_INDEX, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigDatumIndex, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigDatumIndex::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigDatumIndex
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
	configPrompt = "Configure datum index successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CSUP800EraseUserDataDlg 
IMPLEMENT_DYNAMIC(CSUP800EraseUserDataDlg, CCommonConfigDlg)

CSUP800EraseUserDataDlg::CSUP800EraseUserDataDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_SUP800_ERASE_DATA, pParent)
{

}

BEGIN_MESSAGE_MAP(CSUP800EraseUserDataDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CSUP800EraseUserDataDlg::OnBnClickedErase)
END_MESSAGE_MAP()

// CSUP800EraseUserDataDlg 
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
	configPrompt = "SUP800 erase user data successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CSUP800WriteUserDataDlg 
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

// CSUP800WriteUserDataDlg 
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
	configPrompt = "SUP800 write user data successfully";
  AfxBeginThread(ConfigThread, 0);
}

void CSUP800WriteUserDataDlg::OnEnChangeInput()
{
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

// CSUP800ReadUserDataDlg 
IMPLEMENT_DYNAMIC(CSUP800ReadUserDataDlg, CCommonConfigDlg)

CSUP800ReadUserDataDlg::CSUP800ReadUserDataDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_SUP800_READ_DATA, pParent)
{

}

BEGIN_MESSAGE_MAP(CSUP800ReadUserDataDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDC_READ, &CSUP800ReadUserDataDlg::OnBnClickedRead)
	ON_BN_CLICKED(IDC_SAVE, &CSUP800ReadUserDataDlg::OnBnClickedSave)
END_MESSAGE_MAP()

// CSUP800ReadUserDataDlg 
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
	configPrompt = "SUP800 read user data successfully";

	//bool restoreConnect = (((int)(param))==0);
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), configPrompt, false);
	BinaryData ackCmd;
	ackCmd.Alloc(1024);
	CGPSDlg::CmdErrorCode er = CGPSDlg::gpsDlg->GetBinaryResponse(&ackCmd, 0x7A, 0x09, g_setting.defaultTimeout, true, true);

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

// CConfigureSignalDisturbanceStatusDlg 
IMPLEMENT_DYNAMIC(CConfigureSignalDisturbanceStatusDlg, CCommonConfigDlg)

CConfigureSignalDisturbanceStatusDlg::CConfigureSignalDisturbanceStatusDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_SIG_DISTURB_DLG, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigureSignalDisturbanceStatusDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigureSignalDisturbanceStatusDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigureSignalDisturbanceStatusDlg 
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
	configPrompt = "Configure signal disturbance status successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigureGpsUtcLeapSecondsInUtcDlg 
IMPLEMENT_DYNAMIC(CConfigureGpsUtcLeapSecondsInUtcDlg, CCommonConfigDlg)

CConfigureGpsUtcLeapSecondsInUtcDlg::CConfigureGpsUtcLeapSecondsInUtcDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_GPS_LEAP_IN_UTC_DLG, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigureGpsUtcLeapSecondsInUtcDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigureGpsUtcLeapSecondsInUtcDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigureGpsUtcLeapSecondsInUtcDlg 
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
	configPrompt = "Configure GPS/UTC leap Seconds in UTC successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigNoisePowerControlDlg 
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

// CConfigNoisePowerControlDlg 
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
	configPrompt = "Configure noise power successfully";
  AfxBeginThread(ConfigThread, 0);
}

// ConfigPowerSavingParametersRomDlg 
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

// ConfigPowerSavingParametersRomDlg
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
	configPrompt = "Config power saving parameters ROM successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CIqPlot 
IMPLEMENT_DYNAMIC(CIqPlot, CCommonConfigDlg)

U08 CIqPlot::m_gnssType = 1;
U16 CIqPlot::m_nmeaSvid = 1;
U08 CIqPlot::m_rate = 100;
BOOL CIqPlot::m_bEnable = TRUE;

CIqPlot::CIqPlot(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_IQ_PLOT, pParent)
{

}

BEGIN_MESSAGE_MAP(CIqPlot, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CIqPlot::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SEND, &CIqPlot::OnBnClickedSend)
END_MESSAGE_MAP()

// CIqPlot 
BOOL CIqPlot::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	CString txt;
	
	((CComboBox*)GetDlgItem(IDC_TYPE))->SetCurSel(m_gnssType - 1);
	txt.Format("%d", m_nmeaSvid);
	GetDlgItem(IDC_SVID)->SetWindowText(txt);
	txt.Format("%d", m_rate);
	GetDlgItem(IDC_RATE)->SetWindowText(txt);
	((CComboBox*)GetDlgItem(IDC_ENABLE))->SetCurSel(m_bEnable);

	//if(!Utility::IsProcessRunning(L"IQPlot.exe"))
	CString pipeName = Utility::GetNameAttachPid("SkytraqIQPlotPipe");
	if(!Utility::IsNamedPipeUsing(pipeName))
	{
		txt = Utility::GetSpecialFolder(CSIDL_APPDATA);
		txt += "\\GNSS_Viewer_V2";
		::CreateDirectory(txt, NULL);
		txt += "\\IQPlot.exe";
		Utility::CopyResToFile(txt, IDR_IQPLOT, "EXEC");
		txt += " ";
		txt += pipeName;
		Utility::ExecuteExeNoWait(txt);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CIqPlot::OnBnClickedSend()
{	
}
	
void CIqPlot::OnBnClickedOk()
{	
	CString txt;
	m_gnssType = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel() + 1;
	GetDlgItem(IDC_SVID)->GetWindowText(txt);
	m_nmeaSvid = atoi(txt);
	GetDlgItem(IDC_RATE)->GetWindowText(txt);
	m_rate = atoi(txt);
	m_bEnable = ((CComboBox*)GetDlgItem(IDC_ENABLE))->GetCurSel();

	OnOK();
}

void CIqPlot::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(7);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x7C;
	*cmd.GetBuffer(2) = m_gnssType;
	*cmd.GetBuffer(3) = (U08)HIBYTE(m_nmeaSvid);
	*cmd.GetBuffer(4) = (U08)LOBYTE(m_nmeaSvid);
	*cmd.GetBuffer(5) = m_rate;
	*cmd.GetBuffer(6) = m_bEnable;


	configCmd.SetData(cmd);
	configPrompt = "Config IQ successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigGeofencing 
IMPLEMENT_DYNAMIC(CConfigGeofencing, CCommonConfigDlg)

CConfigGeofencing::CConfigGeofencing(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_GEOFENCE, pParent)
{
	m_no = 0;
}

BEGIN_MESSAGE_MAP(CConfigGeofencing, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigGeofencing::OnBnClickedOk)
	ON_BN_CLICKED(IDC_ADD1, &CConfigGeofencing::OnBnClickedAddPoint)
	ON_BN_CLICKED(IDC_ADD2, &CConfigGeofencing::OnBnClickedAddPoints)
	ON_BN_CLICKED(IDC_CLEAR, &CConfigGeofencing::OnBnClickedClearAll)
	ON_BN_CLICKED(IDC_COPY, &CConfigGeofencing::OnBnClickedCopyResult)
END_MESSAGE_MAP()

void CConfigGeofencing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_POINTS, m_points);
}

// CConfigGeofencing
BOOL CConfigGeofencing::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

  CGPSDlg::CmdErrorCode ack = CGPSDlg::Timeout;
	CStringArray points;
  CString txt;

  CGPSDlg::gpsDlg->m_nGeofecingNo = m_no;
	ack = CGPSDlg::gpsDlg->QueryGeofenceEx(CGPSDlg::Return, &points);
  if(ack != CGPSDlg::Ack)
	{
    txt.Format("Not supported Configure geo-fencing data!");
		AfxMessageBox(txt);
    this->OnCancel();
		return TRUE;
  }

	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
	if(INVERT_LON_LAT)
	{
		GetDlgItem(IDC_COOR_TEXT1)->SetWindowText("Latitude");
		GetDlgItem(IDC_COOR_TEXT2)->SetWindowText("Longitude");
		GetDlgItem(IDC_POINTS_PMT)->SetWindowText("2. Add multiple coordinates, separated by \r\ncommas and rows(Ctrl + Enter).\r\ne.g.:\r\n24.784915663,121.008697445\r\n24.784965052,121.008810556\r\n24.784854644,121.008853770\r\n24.784811388,121.008751459");
	}
	else
	{
		GetDlgItem(IDC_COOR_TEXT1)->SetWindowText("Longitude");
		GetDlgItem(IDC_COOR_TEXT2)->SetWindowText("Latitude");
		GetDlgItem(IDC_POINTS_PMT)->SetWindowText("2. Add multiple coordinates, separated by \r\ncommas and rows(Ctrl + Enter).\r\ne.g.:\r\n121.008697445,24.784915663\r\n121.008810556,24.784965052\r\n121.008853770,24.784854644\r\n121.008751459,24.784811388");
	}

  for(int i = 0; i < points.GetCount(); ++i)
  {
    m_points.AddString(points.GetAt(i));
  }
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigGeofencing::OnBnClickedAddPoint()
{
	CString s, item;
	double d1, d2;
	GetDlgItem(IDC_LON)->GetWindowText(s);
	d1 = atof(s);
	GetDlgItem(IDC_LAT)->GetWindowText(s);
	d2 = atof(s);
	item.Format("%12.9lf,%12.9lf", d1, d2);
	AddPoint(item);
}

void CConfigGeofencing::OnBnClickedAddPoints()
{	
	CString s, item;
	double d1, d2;
	GetDlgItem(IDC_POINT_LIST)->GetWindowText(s);

	int start = 0;
	CString tok;
	do
	{
		tok = s.Tokenize(",\r\n", start);
		if(tok.IsEmpty())
		{	//break when empty line
			break;
		}
		d1 = atof(tok);
		if(start != -1)
		{
			tok = s.Tokenize(",\r\n", start);
			d2 = atof(tok);
		}
		else
		{	
			d2 = 0;
		}

		item.Format("%12.9lf,%12.9lf", d1, d2);

		if(!AddPoint(item))
			return;
	} while (start != -1);
}

bool CConfigGeofencing::AddPoint(const CString s)
{
	//const int MaxCount = (m_no==0) ? 10 : 16;
	const int MaxCount = 10;
	if(m_points.GetCount() == MaxCount)
	{
		CString msg;
		msg.Format("It can not be set more than %d points", MaxCount);
		::AfxMessageBox(msg);
		return false;
	}
	m_points.AddString(s);
	return true;
}

void CConfigGeofencing::OnBnClickedClearAll()
{
	m_points.ResetContent();
}

void CConfigGeofencing::OnBnClickedCopyResult()
{
  //IDC_POINT_LIST GetDlgItem(IDC_POINT_LIST)->GetWindowText(s);
  //IDC_POINTS m_points
  int count = m_points.GetCount();
  if(count == 0)
  {
		::AfxMessageBox("The result is empty!");
		return;  
  }
  CString txt, edTxt;
	for(int i = 0; i < count; ++i)
	{
		m_points.GetText(i, txt);
		edTxt += txt;
    if(i != count - 1)
    {
      edTxt += "\r\n";
    }
  }
  GetDlgItem(IDC_POINT_LIST)->SetWindowText(edTxt);
	m_points.ResetContent();
}

void CConfigGeofencing::OnBnClickedOk()
{	
	CString txt;
	int count = m_points.GetCount();
	if(count < 3)
	{
		txt.Format("You can not set less than 3 points.");
		::AfxMessageBox(txt);
		return;
	}

	lons.clear();
	lats.clear();
	double d;
	int sp = 0;
	for(int i = 0; i < count; ++i)
	{
		m_points.GetText(i, txt);
		sp = txt.Find(',');
		d = atof(txt.Left(sp));
		if(INVERT_LON_LAT)
		{
			lats.push_back(d);
		}
		else
		{
			lons.push_back(d);
		}

		d = atof(txt.Right(txt.GetLength() - sp - 1));
		if(INVERT_LON_LAT)
		{
			lons.push_back(d);
		}
		else
		{
			lats.push_back(d);
		}	
	}
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigGeofencing::DoCommand()
{
	CWaitCursor wait;
	U08 size = (U08)lons.size();
	U08 temp[8];
	BinaryData cmd(((m_no==0) ? 4 : 5) + 16 * size);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = (m_no==0) ? 0x2F : 0x34;
	*cmd.GetBuffer(2) = m_attribute;
	int index = 3;
	if(m_no!=0)	
	{
		*cmd.GetBuffer(3) = m_no;
		index = 4;
	}
	*cmd.GetBuffer(index) = size;

	for(U08 i = 0; i < size; ++i)
	{
		memcpy(temp, &(lats[i]), sizeof(temp));
		U08* dptr = temp + 7;
		int p = i * 16 + index + 1;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;

		memcpy(temp, &(lons[i]), sizeof(temp));
		dptr = temp + 7;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
		*cmd.GetBuffer(p++) = *dptr--;
	}

	configCmd.SetData(cmd);
	if(m_no == 0)
	{
		configPrompt = "Configure geo-fencing data successfully";
	}
	else
	{
		configPrompt.Format("Configure geo-fencing data %d successfully", m_no);
	}
  AfxBeginThread(ConfigThread, 0);
}

// CConfigRtkMode 
IMPLEMENT_DYNAMIC(CConfigRtkMode, CCommonConfigDlg)

CConfigRtkMode::CConfigRtkMode(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_RTK_MODE, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigRtkMode, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigRtkMode::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkMode 

BOOL CConfigRtkMode::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigRtkMode::OnBnClickedOk()
{	
	m_mode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

UINT AFX_CDECL ConfigRtkThread(LPVOID param)
{
	bool restoreConnect = (((int)(param))==0);
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), configPrompt, false);
	
	//RTK base mode should be in 1 Hz update rate.
	Sleep(1000);
	BinaryData cmd(3);
	*cmd.GetBuffer(0) = 0x0E;
	*cmd.GetBuffer(1) = (U08)1;
	*cmd.GetBuffer(2) = (U08)configCmd.GetData()[7];
	configCmd.SetData(cmd);
	configPrompt = "Configure 1Hz update rate successfully";
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), configPrompt, restoreConnect);

	return 0;
}

void CConfigRtkMode::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x6A;
	*cmd.GetBuffer(1) = 0x01;
	*cmd.GetBuffer(2) = (U08)m_mode;
	*cmd.GetBuffer(3) = (U08)m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure RTK mode successfully";
	if(m_mode == 1)	//Base mode
	{	//Base mode should set to 1 Hz.
		AfxBeginThread(ConfigRtkThread, 0);
	}
	else
	{
		AfxBeginThread(ConfigThread, 0);
	}
}

// CConfigRtkParameters 
IMPLEMENT_DYNAMIC(CConfigRtkParameters, CCommonConfigDlg)

CConfigRtkParameters::CConfigRtkParameters(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_RTK_PARAM, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigRtkParameters, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigRtkParameters::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkParameters 
BOOL CConfigRtkParameters::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_PARAM01))->SetWindowText("300");
	((CEdit*)GetDlgItem(IDC_PARAM01))->SetLimitText(5);
	((CEdit*)GetDlgItem(IDC_PARAM02))->SetWindowText("000F");
	((CEdit*)GetDlgItem(IDC_PARAM02))->SetLimitText(4);
	((CEdit*)GetDlgItem(IDC_PARAM03))->SetWindowText("0023");
	((CEdit*)GetDlgItem(IDC_PARAM03))->SetLimitText(4);
	((CEdit*)GetDlgItem(IDC_PARAM04))->SetWindowText("0028");
	((CEdit*)GetDlgItem(IDC_PARAM04))->SetLimitText(4);
	((CEdit*)GetDlgItem(IDC_PARAM05))->SetWindowText("00000000");
	((CEdit*)GetDlgItem(IDC_PARAM05))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_PARAM06))->SetWindowText("00000000");
	((CEdit*)GetDlgItem(IDC_PARAM06))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_PARAM07))->SetWindowText("00000000");
	((CEdit*)GetDlgItem(IDC_PARAM07))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_PARAM08))->SetWindowText("00000000");
	((CEdit*)GetDlgItem(IDC_PARAM08))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_PARAM09))->SetWindowText("00");
	((CEdit*)GetDlgItem(IDC_PARAM09))->SetLimitText(2);
	((CEdit*)GetDlgItem(IDC_PARAM10))->SetWindowText("00");
	((CEdit*)GetDlgItem(IDC_PARAM10))->SetLimitText(2);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigRtkParameters::OnBnClickedOk()
{	
	CString txt;
	((CEdit*)GetDlgItem(IDC_PARAM01))->GetWindowText(txt);
	m_param01 = (U16)atoi(txt);
	((CEdit*)GetDlgItem(IDC_PARAM02))->GetWindowText(txt);
	m_param02 = (U16)ConvertCharToU32(txt);
	((CEdit*)GetDlgItem(IDC_PARAM03))->GetWindowText(txt);
	m_param03 = (U16)ConvertCharToU32(txt);
	((CEdit*)GetDlgItem(IDC_PARAM04))->GetWindowText(txt);
	m_param04 = (U16)ConvertCharToU32(txt);
	((CEdit*)GetDlgItem(IDC_PARAM05))->GetWindowText(txt);
	m_param05 = ConvertCharToU32(txt);
	((CEdit*)GetDlgItem(IDC_PARAM06))->GetWindowText(txt);
	m_param06 = ConvertCharToU32(txt);
	((CEdit*)GetDlgItem(IDC_PARAM07))->GetWindowText(txt);
	m_param07 = ConvertCharToU32(txt);
	((CEdit*)GetDlgItem(IDC_PARAM08))->GetWindowText(txt);
	m_param08 = ConvertCharToU32(txt);
	((CEdit*)GetDlgItem(IDC_PARAM09))->GetWindowText(txt);
	m_param09 = (U08)ConvertCharToU32(txt);
	((CEdit*)GetDlgItem(IDC_PARAM10))->GetWindowText(txt);
	m_param10 = (U08)ConvertCharToU32(txt);

	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigRtkParameters::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(67);
	*cmd.GetBuffer(0) = 0x6A;
	*cmd.GetBuffer(1) = 0x03;
	*cmd.GetBuffer(2) = (U08)HIBYTE(m_param01);
	*cmd.GetBuffer(3) = (U08)LOBYTE(m_param01);

	*cmd.GetBuffer(4) = (U08)HIBYTE(m_param02);
	*cmd.GetBuffer(5) = (U08)LOBYTE(m_param02);

	*cmd.GetBuffer(6) = (U08)HIBYTE(m_param03);
	*cmd.GetBuffer(7) = (U08)LOBYTE(m_param03);

	*cmd.GetBuffer(8) = (U08)HIBYTE(m_param04);
	*cmd.GetBuffer(9) = (U08)LOBYTE(m_param04);

	*cmd.GetBuffer(10) = HIBYTE(HIWORD(m_param05));
	*cmd.GetBuffer(11) = LOBYTE(HIWORD(m_param05));
	*cmd.GetBuffer(12) = HIBYTE(LOWORD(m_param05));
	*cmd.GetBuffer(13) = LOBYTE(LOWORD(m_param05));

	*cmd.GetBuffer(14) = HIBYTE(HIWORD(m_param06));
	*cmd.GetBuffer(15) = LOBYTE(HIWORD(m_param06));
	*cmd.GetBuffer(16) = HIBYTE(LOWORD(m_param06));
	*cmd.GetBuffer(17) = LOBYTE(LOWORD(m_param06));	

	*cmd.GetBuffer(18) = HIBYTE(HIWORD(m_param07));
	*cmd.GetBuffer(19) = LOBYTE(HIWORD(m_param07));
	*cmd.GetBuffer(20) = HIBYTE(LOWORD(m_param07));
	*cmd.GetBuffer(21) = LOBYTE(LOWORD(m_param07));	

	*cmd.GetBuffer(22) = HIBYTE(HIWORD(m_param08));
	*cmd.GetBuffer(23) = LOBYTE(HIWORD(m_param08));
	*cmd.GetBuffer(24) = HIBYTE(LOWORD(m_param08));
	*cmd.GetBuffer(25) = LOBYTE(LOWORD(m_param08));

	*cmd.GetBuffer(26) = m_param09;
	*cmd.GetBuffer(27) = m_param10;

	*cmd.GetBuffer(66) = (U08)m_attribute;
	configCmd.SetData(cmd);
	configPrompt = "Configure RTK parameters successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigRtkReset 
IMPLEMENT_DYNAMIC(CConfigRtkReset, CCommonConfigDlg)

CConfigRtkReset::CConfigRtkReset(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_RTK_RESET, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigRtkReset, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigRtkReset::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkReset 
BOOL CConfigRtkReset::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	GetDlgItem(IDC_MODE)->SetWindowText("0");
  //20170421 Doesn't need UI now
  OnOK();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigRtkReset::OnBnClickedOk()
{	
	CString txt;
	GetDlgItem(IDC_MODE)->GetWindowText(txt);
	m_mode = atoi(txt);
	OnOK();
}

void CConfigRtkReset::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(2);
	*cmd.GetBuffer(0) = 0x6A;
	*cmd.GetBuffer(1) = 0x05;
	//*cmd.GetBuffer(2) = (U08)m_mode;
	configCmd.SetData(cmd);
	configPrompt = "Reset RTK engine successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigRtkMode2 
IMPLEMENT_DYNAMIC(CConfigRtkMode2, CCommonConfigDlg)

CConfigRtkMode2::CConfigRtkMode2(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_RTK_MODE2, pParent)
{
  cmdMode = CfgRtkOprtFctn;
}

BEGIN_MESSAGE_MAP(CConfigRtkMode2, CCommonConfigDlg)
	ON_CBN_SELCHANGE(IDC_MODE, OnCbnSelChangeRtkMode)
	ON_CBN_SELCHANGE(IDC_BASE_OPT_FUN, OnCbnSelChangeBaseOpt)
	ON_CBN_SELCHANGE(IDC_ROVER_OPT_FUN, OnCbnSelChangeRoverOpt)
	ON_BN_CLICKED(IDOK, &CConfigRtkMode2::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkMode 
BOOL CConfigRtkMode2::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

  BinaryData ackCmd;
  CString title;
  this->GetWindowText(title);
  if(cmdMode==CfgRtkOprtFctn)
  {
    CGPSDlg::CmdErrorCode err = CGPSDlg::gpsDlg->QueryRtkMode2(CGPSDlg::Return, &ackCmd);
	  if(CGPSDlg::Ack == err)
	  { //Load from device
      title += " (Query success)";
      U16 cmdLen = ConvertLeonU16(ackCmd.Ptr(2));
		  cmdMode = (cmdLen == 37) ? CfgRtkOprtFctnOld : CfgRtkOprtFctn;

	    ((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(ackCmd[6]);
      if(ackCmd[6] == 0)
      { //RTK Rover Mode
	      ((CComboBox*)GetDlgItem(IDC_ROVER_OPT_FUN))->SetCurSel(ackCmd[7]);
	      ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->SetCurSel(0);
	      ((CComboBox*)GetDlgItem(IDC_PKB_OPT_FUN))->SetCurSel(0);
      }
      else if(ackCmd[6] == 1)
      { //RTK Base Mode
	      ((CComboBox*)GetDlgItem(IDC_ROVER_OPT_FUN))->SetCurSel(0);
        ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->SetCurSel(ackCmd[7]);
	      ((CComboBox*)GetDlgItem(IDC_PKB_OPT_FUN))->SetCurSel(0);
      }
      else if(ackCmd[6] == 2)
      { //RTK precisely kinematic base mode
	      ((CComboBox*)GetDlgItem(IDC_ROVER_OPT_FUN))->SetCurSel(0);
        ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->SetCurSel(0);
        ((CComboBox*)GetDlgItem(IDC_PKB_OPT_FUN))->SetCurSel(ackCmd[7]);
      }

      DisplayStatic(this, IDC_MVB_EDT1, "%f", ConvertLeonFloat(ackCmd.Ptr(36)));

      DWORD svyLen = MAKELONG(MAKEWORD(ackCmd[11], ackCmd[10]), MAKEWORD(ackCmd[9], ackCmd[8]));
      DWORD stdDev = MAKELONG(MAKEWORD(ackCmd[15], ackCmd[14]), MAKEWORD(ackCmd[13], ackCmd[12]));
      if(svyLen == 0 && stdDev == 0)
      {
        DisplayStatic(this, IDC_SRV_EDT1, "%u", 2000);
        DisplayStatic(this, IDC_SRV_EDT2, "%u", 30);
      }
      else
      {
        DisplayStatic(this, IDC_SRV_EDT1, "%u", svyLen);
        DisplayStatic(this, IDC_SRV_EDT2, "%u", stdDev);
      }

      DisplayStatic(this, IDC_STT_EDT1, "%f", ConvertLeonDouble(ackCmd.Ptr(16)));
      DisplayStatic(this, IDC_STT_EDT2, "%f", ConvertLeonDouble(ackCmd.Ptr(24)));
      DisplayStatic(this, IDC_STT_EDT3, "%f", ConvertLeonFloat(ackCmd.Ptr(32)));
    } //if(CGPSDlg::Ack == err)
    else
    { //Default value
      title += " (Query failed)";
	    ((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	    ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->SetCurSel(0);
	    ((CComboBox*)GetDlgItem(IDC_ROVER_OPT_FUN))->SetCurSel(0);
	    ((CComboBox*)GetDlgItem(IDC_PKB_OPT_FUN))->SetCurSel(0);

 	    GetDlgItem(IDC_MVB_EDT1)->SetWindowText("0");
	    GetDlgItem(IDC_SRV_EDT1)->SetWindowText("2000");
	    GetDlgItem(IDC_SRV_EDT2)->SetWindowText("30");

	    GetDlgItem(IDC_STT_EDT1)->SetWindowText("");
	    GetDlgItem(IDC_STT_EDT2)->SetWindowText("");
	    GetDlgItem(IDC_STT_EDT3)->SetWindowText("");
    } //if(CGPSDlg::Ack == err) else
  } //if(cmdMode==CfgRtkOprtFctn)
  else if(cmdMode==CfgBasePosition || cmdMode==CfgTiming)
  {
    AdjustUI();
    CGPSDlg::CmdErrorCode err = (cmdMode==CfgBasePosition)
        ? CGPSDlg::gpsDlg->QueryBasePosition(CGPSDlg::Return, &ackCmd)
        : CGPSDlg::gpsDlg->QueryTiming(CGPSDlg::Return, &ackCmd);

	  if(CGPSDlg::Ack == err)
	  { //Load from device
      title += " (Query success)";
      ((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(1);
	    ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->SetCurSel(ackCmd[5]);
      
      DWORD svyLen = MAKELONG(MAKEWORD(ackCmd[9], ackCmd[8]), MAKEWORD(ackCmd[7], ackCmd[6]));
      DWORD stdDev = MAKELONG(MAKEWORD(ackCmd[13], ackCmd[12]), MAKEWORD(ackCmd[11], ackCmd[10]));
      if(svyLen == 0 && stdDev == 0)
      {
        DisplayStatic(this, IDC_SRV_EDT1, "%u", 2000);
        DisplayStatic(this, IDC_SRV_EDT2, "%u", 30);
      }
      else
      {
        DisplayStatic(this, IDC_SRV_EDT1, "%u", svyLen);
        DisplayStatic(this, IDC_SRV_EDT2, "%u", stdDev);
      }

      DisplayStatic(this, IDC_STT_EDT1, "%f", ConvertLeonDouble(ackCmd.Ptr(14)));
      DisplayStatic(this, IDC_STT_EDT2, "%f", ConvertLeonDouble(ackCmd.Ptr(22)));
      DisplayStatic(this, IDC_STT_EDT3, "%f", ConvertLeonFloat(ackCmd.Ptr(30)));
    } //if(CGPSDlg::Ack == err)
    else
    { //Default value
      title += " (Query failed)";
      ((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(1);
	    ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->SetCurSel(0);

	    GetDlgItem(IDC_SRV_EDT1)->SetWindowText("2000");
	    GetDlgItem(IDC_SRV_EDT2)->SetWindowText("30");

	    GetDlgItem(IDC_STT_EDT1)->SetWindowText("");
	    GetDlgItem(IDC_STT_EDT2)->SetWindowText("");
	    GetDlgItem(IDC_STT_EDT3)->SetWindowText("");
    } //if(CGPSDlg::Ack == err) else
  } //else if(cmdMode==CfgBasePosition)
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
  this->SetWindowText(title);
	UpdateStatus();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigRtkMode2::OnBnClickedOk()
{	
	m_rtkMode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_baseOpt = ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->GetCurSel();
	m_roverOpt = ((CComboBox*)GetDlgItem(IDC_ROVER_OPT_FUN))->GetCurSel();
	m_pkbOpt = ((CComboBox*)GetDlgItem(IDC_PKB_OPT_FUN))->GetCurSel();

	CString txt;
	((CEdit*)GetDlgItem(IDC_SRV_EDT1))->GetWindowText(txt);
	m_srvValue1 = atoi(txt);
	((CEdit*)GetDlgItem(IDC_SRV_EDT2))->GetWindowText(txt);
	m_srvValue2 = atoi(txt);

	((CEdit*)GetDlgItem(IDC_STT_EDT1))->GetWindowText(txt);
	m_sttValue1 = atof(txt);
	((CEdit*)GetDlgItem(IDC_STT_EDT2))->GetWindowText(txt);
	m_sttValue2 = atof(txt);
	((CEdit*)GetDlgItem(IDC_STT_EDT3))->GetWindowText(txt);
	m_sttValue3 = (float)atof(txt);
	if(cmdMode == CfgRtkOprtFctn)
	{
		((CEdit*)GetDlgItem(IDC_MVB_EDT1))->GetWindowText(txt);	
		m_mvbLength = (float)atof(txt);
	}
	else if(cmdMode == CfgRtkOprtFctnOld)
	{
		((CEdit*)GetDlgItem(IDC_MVB_EDT1))->ShowWindow(FALSE);	
	}
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();
	OnOK();
}

void CConfigRtkMode2::DoCommand()
{
	CWaitCursor wait;
  int cmdSize = 0;
  switch(cmdMode)
  {
  case CfgRtkOprtFctnOld:
    cmdSize = 33;
    break;
  case CfgRtkOprtFctn:
    cmdSize = 37;
    break;
  case CfgBasePosition:  
  case CfgTiming:  
    cmdSize = 31;
    break;
  }

	BinaryData cmd(cmdSize);
  if(cmdMode == CfgBasePosition || cmdMode == CfgTiming)
  {
    *cmd.GetBuffer(0) = (cmdMode == CfgBasePosition) ? 0x22 : 0x54;
	  *cmd.GetBuffer(1) = m_baseOpt;
	  //U32 Survey Length
	  *cmd.GetBuffer(2) = HIBYTE(HIWORD(m_srvValue1));
	  *cmd.GetBuffer(3) = LOBYTE(HIWORD(m_srvValue1));
	  *cmd.GetBuffer(4) = HIBYTE(LOWORD(m_srvValue1));
	  *cmd.GetBuffer(5) = LOBYTE(LOWORD(m_srvValue1));
	  //U32 Standard Deviation
	  *cmd.GetBuffer(6) = HIBYTE(HIWORD(m_srvValue2));
	  *cmd.GetBuffer(7) = LOBYTE(HIWORD(m_srvValue2));
	  *cmd.GetBuffer(8) = HIBYTE(LOWORD(m_srvValue2));
	  *cmd.GetBuffer(9) = LOBYTE(LOWORD(m_srvValue2));
	  //D64 Latitude
	  *cmd.GetBuffer(10) = *(((U08*)(&m_sttValue1)) + 7);
	  *cmd.GetBuffer(11) = *(((U08*)(&m_sttValue1)) + 6);
	  *cmd.GetBuffer(12) = *(((U08*)(&m_sttValue1)) + 5);
	  *cmd.GetBuffer(13) = *(((U08*)(&m_sttValue1)) + 4);
	  *cmd.GetBuffer(14) = *(((U08*)(&m_sttValue1)) + 3);
	  *cmd.GetBuffer(15) = *(((U08*)(&m_sttValue1)) + 2);
	  *cmd.GetBuffer(16) = *(((U08*)(&m_sttValue1)) + 1);
	  *cmd.GetBuffer(17) = *(((U08*)(&m_sttValue1)) + 0);
	  //D64 Longitude
	  *cmd.GetBuffer(18) = *(((U08*)(&m_sttValue2)) + 7);
	  *cmd.GetBuffer(19) = *(((U08*)(&m_sttValue2)) + 6);
	  *cmd.GetBuffer(20) = *(((U08*)(&m_sttValue2)) + 5);
	  *cmd.GetBuffer(21) = *(((U08*)(&m_sttValue2)) + 4);
	  *cmd.GetBuffer(22) = *(((U08*)(&m_sttValue2)) + 3);
	  *cmd.GetBuffer(23) = *(((U08*)(&m_sttValue2)) + 2);
	  *cmd.GetBuffer(24) = *(((U08*)(&m_sttValue2)) + 1);
	  *cmd.GetBuffer(25) = *(((U08*)(&m_sttValue2)) + 0);
	  //F32 Ellipsoidal Height
	  *cmd.GetBuffer(26) = *(((U08*)(&m_sttValue3)) + 3);
	  *cmd.GetBuffer(27) = *(((U08*)(&m_sttValue3)) + 2);
	  *cmd.GetBuffer(28) = *(((U08*)(&m_sttValue3)) + 1);
	  *cmd.GetBuffer(29) = *(((U08*)(&m_sttValue3)) + 0);
	  //U08 Attributes
	  *cmd.GetBuffer(30) = (U08)m_attribute;  

    configCmd.SetData(cmd);
	  configPrompt = (cmdMode == CfgBasePosition)
      ? "Configure base position successfully"
      : "Configure timing successfully";
		AfxBeginThread(ConfigThread, 0);
  } //if(if(cmdMode == CfgBasePosition)
  else
  {
	  *cmd.GetBuffer(0) = 0x6A;
	  *cmd.GetBuffer(1) = 0x06;
	  *cmd.GetBuffer(2) = (U08)m_rtkMode;
    if(m_rtkMode == 0)
    {
	    *cmd.GetBuffer(3) = m_roverOpt;
    }
    else if(m_rtkMode == 1)
    {
	    *cmd.GetBuffer(3) = m_baseOpt;
    }
    else if(m_rtkMode == 2)
    {
	    *cmd.GetBuffer(3) = m_pkbOpt;
    }
    //U32
	  *cmd.GetBuffer(4) = HIBYTE(HIWORD(m_srvValue1));
	  *cmd.GetBuffer(5) = LOBYTE(HIWORD(m_srvValue1));
	  *cmd.GetBuffer(6) = HIBYTE(LOWORD(m_srvValue1));
	  *cmd.GetBuffer(7) = LOBYTE(LOWORD(m_srvValue1));
	  //U32
	  *cmd.GetBuffer(8) = HIBYTE(HIWORD(m_srvValue2));
	  *cmd.GetBuffer(9) = LOBYTE(HIWORD(m_srvValue2));
	  *cmd.GetBuffer(10) = HIBYTE(LOWORD(m_srvValue2));
	  *cmd.GetBuffer(11) = LOBYTE(LOWORD(m_srvValue2));
	  //D64
	  *cmd.GetBuffer(12) = *(((U08*)(&m_sttValue1)) + 7);
	  *cmd.GetBuffer(13) = *(((U08*)(&m_sttValue1)) + 6);
	  *cmd.GetBuffer(14) = *(((U08*)(&m_sttValue1)) + 5);
	  *cmd.GetBuffer(15) = *(((U08*)(&m_sttValue1)) + 4);
	  *cmd.GetBuffer(16) = *(((U08*)(&m_sttValue1)) + 3);
	  *cmd.GetBuffer(17) = *(((U08*)(&m_sttValue1)) + 2);
	  *cmd.GetBuffer(18) = *(((U08*)(&m_sttValue1)) + 1);
	  *cmd.GetBuffer(19) = *(((U08*)(&m_sttValue1)) + 0);
	  //D64
	  *cmd.GetBuffer(20) = *(((U08*)(&m_sttValue2)) + 7);
	  *cmd.GetBuffer(21) = *(((U08*)(&m_sttValue2)) + 6);
	  *cmd.GetBuffer(22) = *(((U08*)(&m_sttValue2)) + 5);
	  *cmd.GetBuffer(23) = *(((U08*)(&m_sttValue2)) + 4);
	  *cmd.GetBuffer(24) = *(((U08*)(&m_sttValue2)) + 3);
	  *cmd.GetBuffer(25) = *(((U08*)(&m_sttValue2)) + 2);
	  *cmd.GetBuffer(26) = *(((U08*)(&m_sttValue2)) + 1);
	  *cmd.GetBuffer(27) = *(((U08*)(&m_sttValue2)) + 0);
	  //F32
	  *cmd.GetBuffer(28) = *(((U08*)(&m_sttValue3)) + 3);
	  *cmd.GetBuffer(29) = *(((U08*)(&m_sttValue3)) + 2);
	  *cmd.GetBuffer(30) = *(((U08*)(&m_sttValue3)) + 1);
	  *cmd.GetBuffer(31) = *(((U08*)(&m_sttValue3)) + 0);
	  if(cmdMode == CfgRtkOprtFctn)
	  {
		  //F32
		  *cmd.GetBuffer(32) = *(((U08*)(&m_mvbLength)) + 3);
		  *cmd.GetBuffer(33) = *(((U08*)(&m_mvbLength)) + 2);
		  *cmd.GetBuffer(34) = *(((U08*)(&m_mvbLength)) + 1);
		  *cmd.GetBuffer(35) = *(((U08*)(&m_mvbLength)) + 0);
	  }
	  //U08
	  *cmd.GetBuffer((cmdMode == CfgRtkOprtFctn) ? 36 : 32) = (U08)m_attribute;
    configCmd.SetData(cmd);
	  configPrompt = "Configure RTK mode and operational function successfully";
	  if(m_rtkMode == 1)	//Base mode
	  {
		  AfxBeginThread(ConfigRtkThread, 0);
	  }
	  else
	  {
		  if(m_roverOpt != 2)
		  {	//Clear PSTI032 information
			  CGPSDlg::gpsDlg->m_bClearPsti032 = TRUE;
		  }
		  AfxBeginThread(ConfigThread, 0);
	  }
  } //if(if(cmdMode == CfgBasePosition) else
}

void CConfigRtkMode2::AdjustUI()
{ 
  if(cmdMode == CfgBasePosition || cmdMode == CfgTiming)
  {
	  ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->ResetContent();
    if(cmdMode == CfgBasePosition)
    {
      SetWindowText("Configure Base Position");
      //Kinematic Mode;Survey Mode;Static Mode;
      ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->AddString("Kinematic Mode");
      ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->AddString("Survey Mode");
      ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->AddString("Static Mode");
    }
    else  //CfgTiming
    {
      SetWindowText("Configure Timing");
	    ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->ResetContent();
      //PVT;Survey;Static;
      ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->AddString("PVT");
      ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->AddString("Survey");
      ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->AddString("Static");
    }

    CRect rcTop, rcBottom;
    GetDlgItem(IDC_RTK_MODE_TXT)->GetWindowRect(&rcTop);
    GetDlgItem(IDC_MODE_TXT)->GetWindowRect(&rcBottom);
    int h = rcBottom.top - rcTop.top;
    UINT ctrlArray[] = {
      IDC_MODE_TXT, IDC_BASE_OPT_FUN,  //Base Position Mode
      IDC_SRV_SET1, IDC_SRV_EDT1, //Survey Mode Length
      IDC_SRV_SET2, IDC_SRV_EDT2, //Survey Mode Dev
      IDC_STT_SET1, IDC_STT_EDT1, //Static Mode Lat
      IDC_STT_SET2, IDC_STT_EDT2, //Static Mode Lon
      IDC_SRV_SET3, IDC_STT_EDT3, //Static Mode Height
      IDC_ATTR, IDC_ATTR_TXT,  //Attribute
      IDCANCEL, IDOK,
    };

    for(int i = 0; i < Dim(ctrlArray); ++i)
    {
      GetDlgItem(ctrlArray[i])->GetWindowRect(&rcTop);
      rcTop.OffsetRect(0, 0 - h);
      ScreenToClient(rcTop);
      GetDlgItem(ctrlArray[i])->MoveWindow(rcTop);
    }

    GetWindowRect(&rcTop);
    rcTop.bottom -= h;
    MoveWindow(rcTop);
  }
}


void CConfigRtkMode2::UpdateStatus()
{
	int rtkMode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	int baseOpt = ((CComboBox*)GetDlgItem(IDC_BASE_OPT_FUN))->GetCurSel();
	int roverOpt = ((CComboBox*)GetDlgItem(IDC_ROVER_OPT_FUN))->GetCurSel();
	int pkbOpt = ((CComboBox*)GetDlgItem(IDC_PKB_OPT_FUN))->GetCurSel();

	const char *baseDesc1 = "This function can only match below operational function in RTK base position mode:\r\n - Static Mode\r\n - Survey Mode";
  const char *baseDesc2 = "This function can only match below operational function in RTK base position mode:\r\n - Kinematic Mode\r\nor RTK precisely kinematic base mode:\r\n - Normal, Float";
	const char *roverDesc1 = "This function can only match below operational function in RTK rover mode:\r\n - Normal, Float";
	const char *roverDesc2 = "This function can only match below operational function in RTK rover mode:\r\n - Moving base\r\nor RTK precisely kinematic base mode:\r\n - Normal, Float";

	if(rtkMode == 0)	//RTK Rover mode
	{
		GetDlgItem(IDC_BASE_OPT_FUN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ROVER_OPT_FUN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_PKB_OPT_FUN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MODE_TXT)->SetWindowText("RTK Operational Function :");

		GetDlgItem(IDC_SRV_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT3)->ShowWindow(SW_HIDE);

		if(roverOpt == 2 && cmdMode == CfgRtkOprtFctn)	//Moving base
		{
			GetDlgItem(IDC_DESC)->SetWindowText(baseDesc2);
			GetDlgItem(IDC_MVB_SET)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_MVB_SET2)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_MVB_EDT1)->ShowWindow(SW_SHOW);
		}
		else// if(cmdMode == CfgRtkOprtFctnOld)
		{
			GetDlgItem(IDC_DESC)->SetWindowText(baseDesc1);
			GetDlgItem(IDC_MVB_SET)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_MVB_SET2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_MVB_EDT1)->ShowWindow(SW_HIDE);
		}
		return;
	}
	else if(rtkMode == 1) //RTK Base mode
	{
    if(cmdMode==CfgTiming)
    {
		  GetDlgItem(IDC_MODE_TXT)->SetWindowText("Configure Timing :");
    }
    else if(cmdMode==CfgBasePosition)
    {
		  GetDlgItem(IDC_MODE_TXT)->SetWindowText("Base Position Mode :");
    }
    else
    {
 		  GetDlgItem(IDC_MODE_TXT)->SetWindowText("RTK Operational Function :");
    }
		GetDlgItem(IDC_BASE_OPT_FUN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ROVER_OPT_FUN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PKB_OPT_FUN)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_MVB_SET)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MVB_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MVB_EDT1)->ShowWindow(SW_HIDE);
	}
	else if(rtkMode == 2) //RTK precisely kinematic base mode
	{
		GetDlgItem(IDC_BASE_OPT_FUN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ROVER_OPT_FUN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PKB_OPT_FUN)->ShowWindow(SW_SHOW);
 		GetDlgItem(IDC_MODE_TXT)->SetWindowText("RTK Operational Function :");
		GetDlgItem(IDC_DESC)->SetWindowText(baseDesc1);

		GetDlgItem(IDC_MVB_SET)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MVB_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MVB_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT3)->ShowWindow(SW_HIDE);
		return;
  }

  if(CfgBasePosition == cmdMode || CfgTiming == cmdMode)
  {
		  GetDlgItem(IDC_RTK_MODE_TXT)->ShowWindow(SW_HIDE);
		  GetDlgItem(IDC_DESC)->ShowWindow(SW_HIDE);
	    GetDlgItem(IDC_MODE)->ShowWindow(SW_HIDE);
	    GetDlgItem(IDC_ROVER_OPT_FUN)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_MVB_EDT1)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_DESC2)->ShowWindow(SW_HIDE);
  }

	if(baseOpt == 0)	//Kinematic 
	{
		GetDlgItem(IDC_SRV_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DESC)->SetWindowText(roverDesc2);
		return;
	}

	if(baseOpt == 1)	//Survey
	{
		GetDlgItem(IDC_SRV_SET1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SRV_SET2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SRV_EDT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SRV_EDT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DESC)->SetWindowText(roverDesc1);
		return;
	}

	if(baseOpt == 2)	//Static
	{
		GetDlgItem(IDC_SRV_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_SET2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_SET3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_EDT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_EDT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_EDT3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DESC)->SetWindowText(roverDesc1);
	}
}

void CConfigRtkMode2::OnCbnSelChangeRtkMode()
{
	UpdateStatus();
}

void CConfigRtkMode2::OnCbnSelChangeBaseOpt()
{
	UpdateStatus();
}

void CConfigRtkMode2::OnCbnSelChangeRoverOpt()
{
	UpdateStatus();
}

// CConfigMessageOut 
IMPLEMENT_DYNAMIC(CConfigMessageOut, CCommonConfigDlg)

CConfigMessageOut::CConfigMessageOut(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_MSG_OUT, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigMessageOut, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigMessageOut::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigMessageOut 
BOOL CConfigMessageOut::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
  ((CComboBox*)GetDlgItem(IDC_TYPE))->AddString("UAV Binary");
	((CComboBox*)GetDlgItem(IDC_TYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigMessageOut::OnBnClickedOk()
{	
	m_nType = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();
	OnOK();
}

void CConfigMessageOut::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(3);
	*cmd.GetBuffer(0) = 0x09;
	*cmd.GetBuffer(1) = m_nType;
	*cmd.GetBuffer(2) = m_nAttribute;
	configCmd.SetData(cmd);
	configPrompt = "Configure message type successfully";
  AfxBeginThread(ConfigThread, 0);
}

INT_PTR CConfigMessageOut::DoDirect(int type)
{
	switch(type)
	{
	case 0:
	case 1:
	case 2:
		m_nType = type;
		m_nAttribute = 0;
		break;
	default:
		return IDCANCEL;
	}
	return IDOK;
}

// CConfigSubSecRegister 
IMPLEMENT_DYNAMIC(CConfigSubSecRegister, CCommonConfigDlg)

CConfigSubSecRegister::CConfigSubSecRegister(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CFG_SUBSEC_REG, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigSubSecRegister, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigSubSecRegister::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkReset 
BOOL CConfigSubSecRegister::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	GetDlgItem(IDC_MS)->SetWindowText("0");
	GetDlgItem(IDC_NS)->SetWindowText("0");
	GetDlgItem(IDC_PLLDIV)->SetWindowText("0");
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigSubSecRegister::OnBnClickedOk()
{	
	CString txt;
	GetDlgItem(IDC_MS)->GetWindowText(txt);
	m_nMs = atoi(txt);
	if(m_nMs > 63 || m_nMs < 0)
	{
		AfxMessageBox("MS must be between 0 and 63");
		return;
	}

	GetDlgItem(IDC_NS)->GetWindowText(txt);
	m_nNs = atoi(txt);
	if(m_nNs > 63 || m_nNs < 0)
	{
		AfxMessageBox("NS must be between 0 and 63");
		return;
	}

	GetDlgItem(IDC_PLLDIV)->GetWindowText(txt);
	m_nPllDiv = atoi(txt);
	if(m_nPllDiv > 7 || m_nPllDiv < 0)
	{
		AfxMessageBox("PLL DIV must be between 0 and 7");
		return;
	}

	OnOK();
}

void CConfigSubSecRegister::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x70;
	*cmd.GetBuffer(1) = m_nMs;
	*cmd.GetBuffer(2) = m_nNs;
	*cmd.GetBuffer(3) = m_nPllDiv;
	configCmd.SetData(cmd);
	configPrompt = "Configure subsec register successfully";
  AfxBeginThread(ConfigThread, 0);
}
/*
// CConfigTiming 
IMPLEMENT_DYNAMIC(CConfigTiming, CCommonConfigDlg)

CConfigTiming::CConfigTiming(CWnd* pParent)
: CCommonConfigDlg(IDD_CFG_TIMING, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigTiming, CCommonConfigDlg)
	ON_CBN_SELCHANGE(IDC_MODE, OnCbnSelChangeMode)
	ON_BN_CLICKED(IDOK, &CConfigTiming::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkMode 
BOOL CConfigTiming::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);

	GetDlgItem(IDC_SRV_EDT1)->SetWindowText("2000");
	GetDlgItem(IDC_SRV_EDT2)->SetWindowText("30");

	GetDlgItem(IDC_STT_EDT1)->SetWindowText("");
	GetDlgItem(IDC_STT_EDT2)->SetWindowText("");
	GetDlgItem(IDC_STT_EDT3)->SetWindowText("");

	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	UpdateStatus();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigTiming::OnBnClickedOk()
{	
	m_timingMode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();

	CString txt;
	((CEdit*)GetDlgItem(IDC_SRV_EDT1))->GetWindowText(txt);
	m_srvValue1 = atoi(txt);
	((CEdit*)GetDlgItem(IDC_SRV_EDT2))->GetWindowText(txt);
	m_srvValue2 = atoi(txt);

	((CEdit*)GetDlgItem(IDC_STT_EDT1))->GetWindowText(txt);
	m_sttValue1 = atof(txt);
	((CEdit*)GetDlgItem(IDC_STT_EDT2))->GetWindowText(txt);
	m_sttValue2 = atof(txt);
	((CEdit*)GetDlgItem(IDC_STT_EDT3))->GetWindowText(txt);
	m_sttValue3 = (float)atof(txt);

	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigTiming::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(31);

	*cmd.GetBuffer(0) = 0x54;
	*cmd.GetBuffer(1) = m_timingMode;
	//U32
	*cmd.GetBuffer(2) = HIBYTE(HIWORD(m_srvValue1));
	*cmd.GetBuffer(3) = LOBYTE(HIWORD(m_srvValue1));
	*cmd.GetBuffer(4) = HIBYTE(LOWORD(m_srvValue1));
	*cmd.GetBuffer(5) = LOBYTE(LOWORD(m_srvValue1));
	//U32
	*cmd.GetBuffer(6) = HIBYTE(HIWORD(m_srvValue2));
	*cmd.GetBuffer(7) = LOBYTE(HIWORD(m_srvValue2));
	*cmd.GetBuffer(8) = HIBYTE(LOWORD(m_srvValue2));
	*cmd.GetBuffer(9) = LOBYTE(LOWORD(m_srvValue2));
	//D64
	*cmd.GetBuffer(10) = *(((U08*)(&m_sttValue1)) + 7);
	*cmd.GetBuffer(11) = *(((U08*)(&m_sttValue1)) + 6);
	*cmd.GetBuffer(12) = *(((U08*)(&m_sttValue1)) + 5);
	*cmd.GetBuffer(13) = *(((U08*)(&m_sttValue1)) + 4);
	*cmd.GetBuffer(14) = *(((U08*)(&m_sttValue1)) + 3);
	*cmd.GetBuffer(15) = *(((U08*)(&m_sttValue1)) + 2);
	*cmd.GetBuffer(16) = *(((U08*)(&m_sttValue1)) + 1);
	*cmd.GetBuffer(17) = *(((U08*)(&m_sttValue1)) + 0);
	//D64
	*cmd.GetBuffer(18) = *(((U08*)(&m_sttValue2)) + 7);
	*cmd.GetBuffer(19) = *(((U08*)(&m_sttValue2)) + 6);
	*cmd.GetBuffer(20) = *(((U08*)(&m_sttValue2)) + 5);
	*cmd.GetBuffer(21) = *(((U08*)(&m_sttValue2)) + 4);
	*cmd.GetBuffer(22) = *(((U08*)(&m_sttValue2)) + 3);
	*cmd.GetBuffer(23) = *(((U08*)(&m_sttValue2)) + 2);
	*cmd.GetBuffer(24) = *(((U08*)(&m_sttValue2)) + 1);
	*cmd.GetBuffer(25) = *(((U08*)(&m_sttValue2)) + 0);
	//F32
	*cmd.GetBuffer(26) = *(((U08*)(&m_sttValue3)) + 3);
	*cmd.GetBuffer(27) = *(((U08*)(&m_sttValue3)) + 2);
	*cmd.GetBuffer(28) = *(((U08*)(&m_sttValue3)) + 1);
	*cmd.GetBuffer(29) = *(((U08*)(&m_sttValue3)) + 0);
	//U08
	*cmd.GetBuffer(30) = (U08)m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure Timing successfully";

	AfxBeginThread(ConfigThread, 0);

}

void CConfigTiming::UpdateStatus()
{
	int baseOpt = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();

	if(baseOpt == 0)	//Kinematic 
	{
		GetDlgItem(IDC_SRV_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT3)->ShowWindow(SW_HIDE);
		return;
	}

	if(baseOpt == 1)	//Survey
	{
		GetDlgItem(IDC_SRV_SET1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SRV_SET2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SRV_EDT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SRV_EDT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_EDT3)->ShowWindow(SW_HIDE);
		return;
	}

	if(baseOpt == 2)	//Static
	{
		GetDlgItem(IDC_SRV_SET1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_SET2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRV_EDT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STT_SET1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_SET2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_SET3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_EDT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_EDT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STT_EDT3)->ShowWindow(SW_SHOW);
	}
}

void CConfigTiming::OnCbnSelChangeMode()
{
	UpdateStatus();
}
*/
// CConfigTimingCableDelay 
IMPLEMENT_DYNAMIC(CConfigTimingCableDelay, CCommonConfigDlg)

CConfigTimingCableDelay::CConfigTimingCableDelay(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CFG_TIMING_CABLE_DELAY, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigTimingCableDelay, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigTimingCableDelay::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkMode 
BOOL CConfigTimingCableDelay::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	GetDlgItem(IDC_1PPS_CABLE)->SetWindowText("0");
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigTimingCableDelay::OnBnClickedOk()
{	

	CString txt;
	((CEdit*)GetDlgItem(IDC_1PPS_CABLE))->GetWindowText(txt);
	m_delay = atoi(txt);
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigTimingCableDelay::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(6);

	*cmd.GetBuffer(0) = 0x45;
	//U32
	*cmd.GetBuffer(1) = HIBYTE(HIWORD(m_delay));
	*cmd.GetBuffer(2) = LOBYTE(HIWORD(m_delay));
	*cmd.GetBuffer(3) = HIBYTE(LOWORD(m_delay));
	*cmd.GetBuffer(4) = LOBYTE(LOWORD(m_delay));
	//U08
	*cmd.GetBuffer(5) = (U08)m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure cable delay successfully";
	AfxBeginThread(ConfigThread, 0);
}

// CConfigGpsMeasurementMode 
IMPLEMENT_DYNAMIC(CConfigGpsMeasurementMode, CCommonConfigDlg)

CConfigGpsMeasurementMode::CConfigGpsMeasurementMode(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CFG_GPS_MEAS_MODE, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigGpsMeasurementMode, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigGpsMeasurementMode::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkMode 
BOOL CConfigGpsMeasurementMode::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigGpsMeasurementMode::OnBnClickedOk()
{	
	m_mode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigGpsMeasurementMode::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(3);

	*cmd.GetBuffer(0) = 0x3E;
	*cmd.GetBuffer(1) = (U08)m_mode;
	*cmd.GetBuffer(2) = (U08)m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure GPS measurement mode successfully";
	AfxBeginThread(ConfigThread, 0);
}

// CConfigPscmDeviceAddress
IMPLEMENT_DYNAMIC(CConfigPscmDeviceAddress, CCommonConfigDlg)

CConfigPscmDeviceAddress::CConfigPscmDeviceAddress(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CFG_PSCM_DEV_ADDR, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigPscmDeviceAddress, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigPscmDeviceAddress::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigPscmDeviceAddress 
BOOL CConfigPscmDeviceAddress::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	GetDlgItem(IDC_NUM)->SetWindowText("1");
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigPscmDeviceAddress::OnBnClickedOk()
{	
	CString txt;
	GetDlgItem(IDC_NUM)->GetWindowText(txt);
	m_num = atoi(txt);
	OnOK();
}

void CConfigPscmDeviceAddress::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x7A;
	*cmd.GetBuffer(1) = 0x0A;
	*cmd.GetBuffer(2) = (U08)0x01;
	*cmd.GetBuffer(3) = (U08)m_num;
	configCmd.SetData(cmd);
	configPrompt = "Configure pscm device address successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigPscmLatLonFractionalDigits 
IMPLEMENT_DYNAMIC(CConfigPscmLatLonFractionalDigits, CCommonConfigDlg)

CConfigPscmLatLonFractionalDigits::CConfigPscmLatLonFractionalDigits(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CFG_PSCM_LAT_LON_DIGITS, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigPscmLatLonFractionalDigits, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigPscmLatLonFractionalDigits::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkReset 
BOOL CConfigPscmLatLonFractionalDigits::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	GetDlgItem(IDC_NUM)->SetWindowText("4");
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigPscmLatLonFractionalDigits::OnBnClickedOk()
{	
	CString txt;
	GetDlgItem(IDC_NUM)->GetWindowText(txt);
	m_num = atoi(txt);
	OnOK();
}

void CConfigPscmLatLonFractionalDigits::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x7A;
	*cmd.GetBuffer(1) = 0x0A;
	*cmd.GetBuffer(2) = (U08)0x03;
	*cmd.GetBuffer(3) = (U08)m_num;
	configCmd.SetData(cmd);
	configPrompt = "Configure pscm LAT/LON fractional digits successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigVeryLowSpeed 
IMPLEMENT_DYNAMIC(CConfigVeryLowSpeed, CCommonConfigDlg)

CConfigVeryLowSpeed::CConfigVeryLowSpeed(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_VERY_LOW, pParent)
{
	m_nEnable = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigVeryLowSpeed, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigVeryLowSpeed::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigVeryLowSpeed 
BOOL CConfigVeryLowSpeed::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigVeryLowSpeed::OnBnClickedOk()
{
	m_nEnable = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigVeryLowSpeed::DoCommand()
{
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x37;
	*cmd.GetBuffer(2) = (U08)m_nEnable;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure kernel very low speed successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigDofunUniqueId 
IMPLEMENT_DYNAMIC(CConfigDofunUniqueId, CCommonConfigDlg)

CConfigDofunUniqueId::CConfigDofunUniqueId(CWnd* pParent /*=NULL*/)
	: binData(UniqueIdLength), CCommonConfigDlg(IDD_CFG_DOFUN_UNIQUE_ID, pParent)
{
	m_nMode = 1;
}

BEGIN_MESSAGE_MAP(CConfigDofunUniqueId, CCommonConfigDlg)
	ON_EN_CHANGE(IDC_UNIQUE_ID, &CConfigDofunUniqueId::OnEnChangeInput)
	ON_BN_CLICKED(IDOK, &CConfigDofunUniqueId::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigVeryLowSpeed 
BOOL CConfigDofunUniqueId::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	//((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	//((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigDofunUniqueId::OnBnClickedOk()
{
	//m_nEnable = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	//m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigDofunUniqueId::OnEnChangeInput()
{
 	CString strInput;
	GetDlgItem(IDC_UNIQUE_ID)->GetWindowText(strInput);
	//BinaryData b;
	//if(!Utility::ConvertHexToBinary(strInput, b))
	//{
	//	GetDlgItem(IDC_PROMPT)->SetWindowText("Invalidate Format!");
	//	GetDlgItem(IDOK)->EnableWindow(FALSE);
	//	return;
	//}
	//if(b.Size() > UniqueIdLength)
	//{
	//	GetDlgItem(IDC_PROMPT)->SetWindowText("The device unique ID can't contain more than 16 bytes!");
	//}
	if(strInput.GetLength() != 16)
	{
		GetDlgItem(IDC_PROMPT)->SetWindowText("The device unique ID can only contain 16 bytes!");
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		return;
	}
	else
	{
		GetDlgItem(IDC_PROMPT)->SetWindowText("The correct format.");
	}

	GetDlgItem(IDOK)->EnableWindow(TRUE);
	for(int i=0; i<UniqueIdLength; ++i)
	{
		*binData.GetBuffer(i) = strInput[i];
	}
}

INT_PTR CConfigDofunUniqueId::DoDirect(int type)
{
	switch(type)
	{
	case 0:
		m_nMode = type;
		break;
	default:
		return IDCANCEL;
	}
	return IDOK;
}

void CConfigDofunUniqueId::DoCommand()
{
	if(m_nMode == 1)
	{
		BinaryData cmd(19);
		*cmd.GetBuffer(0) = 0x7A;
		*cmd.GetBuffer(1) = 0x02;
		*cmd.GetBuffer(2) = 0x03;
		for(int i=0; i<UniqueIdLength; ++i)
		{
			*cmd.GetBuffer(3 + i) = binData[i];
		}
		configCmd.SetData(cmd);
		configPrompt = "Configure device unique ID successfully";
	}
	else if(m_nMode == 0)
	{
		BinaryData cmd(3);
		*cmd.GetBuffer(0) = 0x7A;
		*cmd.GetBuffer(1) = 0x02;
		*cmd.GetBuffer(2) = 0x05;

		configCmd.SetData(cmd);
		configPrompt = "Erase device unique ID successfully";
	}
  AfxBeginThread(ConfigThread, 0);
}

// CConfigPstiInterval
IMPLEMENT_DYNAMIC(CConfigPstiInterval, CCommonConfigDlg)

CConfigPstiInterval::CConfigPstiInterval(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_PSTI_INTERVAL, pParent)
{
  m_nType = Psti;
	m_nPstiId = 0;
	m_nPstiInterval = 1;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigPstiInterval, CCommonConfigDlg)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, &CConfigPstiInterval::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigPstiInterval
BOOL CConfigPstiInterval::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

  CString txt, txt1;
  CString title;
  this->GetWindowText(title);

  if(m_nType == Psti)
  {
    title.Format("Configure PSTI%03d Interval", m_nPstiId);
  }
  else
  {
    title = "Configure PIRNSF Interval";
  }
	
  if(m_nType == Psti)
  {
    txt.Format("Set interval to 0 to disable PSTI%03d output.", m_nPstiId);
  }
  else
  {
    txt = "Set interval to 0 to disable PIRNSF output.";
  }

  CGPSDlg::CmdErrorCode ack = CGPSDlg::Timeout;
  U08 interval = 0;
  if(m_nType == Psti)
  {
    switch(m_nPstiId)
    {
    case 4:
    case 30:
    case 32:
    case 33:
      CGPSDlg::gpsDlg->m_nPstiNo = m_nPstiId;
	    ack = CGPSDlg::gpsDlg->QueryPsti(CGPSDlg::Return, &interval);
      break;
    case 63:
    case 65:
    case 67:
    case 68:
    case 70:
      CGPSDlg::gpsDlg->m_nPstiNo = m_nPstiId;
	    ack = CGPSDlg::gpsDlg->QueryPsti(CGPSDlg::Return, &interval);
      GetDlgItem(IDC_SLIDER1)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_LOWER)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_UPPER)->ShowWindow(SW_HIDE);
      txt1.Format("\r\nPSTI%03d only supports interval 0 or 1.", m_nPstiId);
      txt += txt1;
      break;
    default:
      ASSERT(FALSE);
      break;
    }
  }
  else
  {
    ack = CGPSDlg::gpsDlg->QueryNavicMessageInterval(CGPSDlg::Return, &interval);
  }
	GetDlgItem(IDC_PROMPT)->SetWindowText(txt);

  if(ack != CGPSDlg::Ack)
	{
    title += " (Query failed)";
    interval = 1;
	}
  else
  {
    title += " (Query success)";
  }
  m_nPstiInterval = interval;
  ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetRange(0, 255);
  ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->SetRange(0, 255);
  ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->SetTicFreq(15);

  txt.Format("%d", m_nPstiInterval);
  GetDlgItem(IDC_EDIT1)->SetWindowText(txt);
  ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetPos(m_nPstiInterval);
  ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->SetPos(m_nPstiInterval);
  

  ((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
	this->SetWindowText(title);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigPstiInterval::AdjustValue(int nPos, CScrollBar* pScrollBar)
{
	if(nPos == -1)
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

	case IDC_SPIN1:
		GetDlgItem(IDC_EDIT1)->SetWindowText(strNum);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->SetPos(nPos);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CConfigPstiInterval::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{   
	if(NULL==pScrollBar)
	{
		CCommonConfigDlg::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	if(SB_ENDSCROLL!=nSBCode)
	{
		AdjustValue(-1, pScrollBar);
	}
	CCommonConfigDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CConfigPstiInterval::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(NULL==pScrollBar)
	{
		CCommonConfigDlg::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	if(SB_ENDSCROLL!=nSBCode)
	{
		AdjustValue(nPos, pScrollBar);
	}
	CCommonConfigDlg::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CConfigPstiInterval::OnBnClickedOk()
{
	CString txt;
	GetDlgItem(IDC_EDIT1)->GetWindowText(txt);
	m_nPstiInterval = atoi(txt);
  m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigPstiInterval::DoCommand()
{
  if(m_nType == Psti)
  {
	  BinaryData cmd(5);
	  *cmd.GetBuffer(0) = 0x64;
	  *cmd.GetBuffer(1) = 0x21;
	  *cmd.GetBuffer(2) = (U08)m_nPstiId;
	  *cmd.GetBuffer(3) = (U08)m_nPstiInterval;
	  *cmd.GetBuffer(4) = (U08)m_nAttribute;

	  configCmd.SetData(cmd);
    configPrompt.Format("Configure PSTI%03d interval successfully", m_nPstiId);
  }
  else
  {
	  BinaryData cmd(4);
	  *cmd.GetBuffer(0) = 0x6F;
	  *cmd.GetBuffer(1) = 0x01;
	  *cmd.GetBuffer(2) = (U08)m_nPstiInterval;
	  *cmd.GetBuffer(3) = (U08)m_nAttribute;

	  configCmd.SetData(cmd);
    configPrompt.Format("Configure PIRNSF interval successfully", m_nPstiId);
  }
  AfxBeginThread(ConfigThread, 0);
}

// CRtkOnOffSv 
const UINT CRtkOnOffSv::svId[] = {
  IDC_CHECK1, IDC_CHECK2, IDC_CHECK3, IDC_CHECK4, 
  IDC_CHECK5, IDC_CHECK6, IDC_CHECK7, IDC_CHECK8, 
  IDC_CHECK9, IDC_CHECK10, IDC_CHECK11, IDC_CHECK12, 
  IDC_CHECK13, IDC_CHECK14, IDC_CHECK15, IDC_CHECK16, 
  IDC_CHECK17, IDC_CHECK18, IDC_CHECK19, IDC_CHECK20, 
  IDC_CHECK21, IDC_CHECK22, IDC_CHECK23, IDC_CHECK24, 
  IDC_CHECK25, IDC_CHECK26, IDC_CHECK27, IDC_CHECK28, 
  IDC_CHECK29, IDC_CHECK30, IDC_CHECK31, IDC_CHECK32, 
};

IMPLEMENT_DYNAMIC(CRtkOnOffSv, CCommonConfigDlg)

CRtkOnOffSv::CRtkOnOffSv(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_RTK_ONOFF_SV, pParent)
{
	m_svMode = RtkSvGps;
}

BEGIN_MESSAGE_MAP(CRtkOnOffSv, CCommonConfigDlg)
	ON_BN_CLICKED(IDC_CHECK_ALL, &CRtkOnOffSv::OnBnClickedCheckAll)
	ON_BN_CLICKED(IDC_UNCHECK_ALL, &CRtkOnOffSv::OnBnClickedUncheckAll)
	ON_BN_CLICKED(IDC_READ, &CRtkOnOffSv::OnBnClickedRead)
	ON_BN_CLICKED(IDOK, &CRtkOnOffSv::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkMode 
BOOL CRtkOnOffSv::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	UpdateStatus();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CRtkOnOffSv::OnBnClickedRead()
{	
  CGPSDlg::gpsDlg->TerminateGPSThread();
  CGPSDlg::gpsDlg->m_regAddress = m_svReadAddr;
	CGPSDlg::CmdErrorCode ack = CGPSDlg::gpsDlg->QueryRegister(CGPSDlg::Return, &m_svData);
	CGPSDlg::gpsDlg->SetMode();
	CGPSDlg::gpsDlg->CreateGPSThread();

  if(ack != CGPSDlg::Ack) 
  {
    ::AfxMessageBox("Failed to read SV status!");
    return;
  }

  for(int i = 0; i < Dim(svId); ++i)
  {
    ((CButton*)GetDlgItem(svId[i]))->SetCheck(((m_svData & (1 << i)) != 0) ? TRUE : FALSE);
  }
}

void CRtkOnOffSv::OnBnClickedCheckAll()
{	
  for(int i = 0; i < Dim(svId); ++i)
  {
    ((CButton*)GetDlgItem(svId[i]))->SetCheck(TRUE);
  }
}

void CRtkOnOffSv::OnBnClickedUncheckAll()
{	
  for(int i = 0; i < Dim(svId); ++i)
  {
    ((CButton*)GetDlgItem(svId[i]))->SetCheck(FALSE);
  }
}

void CRtkOnOffSv::OnBnClickedOk()
{	
  m_svData = 0;
  for(int i = 0; i < Dim(svId); ++i)
  {
    if(((CButton*)GetDlgItem(svId[i]))->GetCheck())
    {
      m_svData |= 1 << i;
    }
  }
  DoCommand();
}

void CRtkOnOffSv::DoCommand()
{
  CGPSDlg::gpsDlg->TerminateGPSThread();

	BinaryData cmd(9);
	*cmd.GetBuffer(0) = 0x72;
	//U32
	*cmd.GetBuffer(1) = HIBYTE(HIWORD(m_svWriteAddr));
	*cmd.GetBuffer(2) = LOBYTE(HIWORD(m_svWriteAddr));
	*cmd.GetBuffer(3) = HIBYTE(LOWORD(m_svWriteAddr));
	*cmd.GetBuffer(4) = LOBYTE(LOWORD(m_svWriteAddr));
	//U32
	*cmd.GetBuffer(5) = HIBYTE(HIWORD(m_svData));
	*cmd.GetBuffer(6) = LOBYTE(HIWORD(m_svData));
	*cmd.GetBuffer(7) = HIBYTE(LOWORD(m_svData));
	*cmd.GetBuffer(8) = LOBYTE(LOWORD(m_svData));

	configCmd.SetData(cmd);
  configPrompt.Format("Write 0X%08X to reg 0X%08X successfully", m_svData, m_svWriteAddr);
  AfxBeginThread(ConfigThread, (LPVOID)0);
}

void CRtkOnOffSv::UpdateStatus()
{
  const char* svNameGps[] = {
    "GPS SV 1", "GPS SV 2", "GPS SV 3", "GPS SV 4", 
    "GPS SV 5", "GPS SV 6", "GPS SV 7", "GPS SV 8", 
    "GPS SV 9", "GPS SV 10", "GPS SV 11", "GPS SV 12", 
    "GPS SV 13", "GPS SV 14", "GPS SV 15", "GPS SV 16", 
    "GPS SV 17", "GPS SV 18", "GPS SV 19", "GPS SV 20", 
    "GPS SV 21", "GPS SV 22", "GPS SV 24", "GPS SV 24", 
    "GPS SV 25", "GPS SV 26", "GPS SV 27", "GPS SV 28", 
    "GPS SV 29", "GPS SV 30", "GPS SV 31", "GPS SV 32"
  };

  const char* svNameSbasQzss[] = {
    "SBAS SV 33", "SBAS SV 34", "SBAS SV 35", "SBAS SV 36", 
    "SBAS SV 37", "SBAS SV 38", "SBAS SV 39", "SBAS SV 40", 
    "SBAS SV 41", "SBAS SV 42", "SBAS SV 43", "SBAS SV 44", 
    "SBAS SV 45", "SBAS SV 46", "SBAS SV 47", "SBAS SV 48", 
    "SBAS SV 49", "SBAS SV 50", "SBAS SV 51", "Reserved", 
    "QZSS SV 193", "QZSS SV 194", "QZSS SV 195", "QZSS SV 196", 
    "QZSS SV 197", "Reserved", "Reserved", "Reserved", 
    "Reserved", "Reserved", "Reserved", "Reserved"
  };
  
  const char* svNameGlonass[] = {
    "GL SV 65", "GL SV 66", "GL SV 67", "GL SV 68", 
    "GL SV 69", "GL SV 70", "GL SV 71", "GL SV 72", 
    "GL SV 73", "GL SV 74", "GL SV 75", "GL SV 76", 
    "GL SV 77", "GL SV 78", "GL SV 79", "GL SV 80", 
    "GL SV 81", "GL SV 82", "GL SV 83", "GL SV 84", 
    "GL SV 85", "GL SV 86", "GL SV 87", "GL SV 88", 
    "Reserved", "Reserved", "Reserved", "Reserved", 
    "Reserved", "Reserved", "Reserved", "Reserved"
  };

  const char* svNameBBeidou2[] = {
    "BD SV 201", "BD SV 202", "BD SV 203", "BD SV 204", 
    "BD SV 205", "BD SV 206", "BD SV 207", "BD SV 208", 
    "BD SV 209", "BD SV 210", "BD SV 211", "BD SV 212", 
    "BD SV 213", "BD SV 214", "BD SV 215", "BD SV 216", 
    "BD SV 217", "BD SV 218", "BD SV 219", "BD SV 220", 
    "BD SV 221", "BD SV 222", "BD SV 224", "BD SV 224", 
    "BD SV 225", "BD SV 226", "BD SV 227", "BD SV 228", 
    "BD SV 229", "BD SV 230", "Reserved", "Reserved"
  };

  const char** svNamePtr = svNameGps;
  CString title;
  switch(m_svMode)
  {
  case RtkSvGps:
    svNamePtr = svNameGps;
    m_svReadAddr = 0xFE000078;
    m_svWriteAddr = 0xCE000008;
    title = "RTK Enable/Disable GPS SV Mechanism";
    break;
  case RtkSvSbasQzss:
    svNamePtr = svNameSbasQzss;
    m_svReadAddr = 0xFE000079;
    m_svWriteAddr = 0xCE000009;
    title = "RTK Enable/Disable SBAS + QZSS SV Mechanism";
    break;
  case RtkSvGlonass:
    svNamePtr = svNameGlonass;
    m_svReadAddr = 0xFE00007A;
    m_svWriteAddr = 0xCE00000A;
    title = "RTK Enable/Disable GLONASS SV Mechanism";
    break;
  case RtkSvBeidou2:
    svNamePtr = svNameBBeidou2;
    m_svReadAddr = 0xFE00007B;
    m_svWriteAddr = 0xCE00000B;
    title = "RTK Enable/Disable BEIDOU2 SV Mechanism";
    break;
  }

  SetWindowText(title);
  for(int i = 0; i < Dim(svId); ++i)
  {
    GetDlgItem(svId[i])->SetWindowText(svNamePtr[i]);
    if(strcmp(svNamePtr[i], "Reserved") == 0)
    {
      GetDlgItem(svId[i])->EnableWindow(FALSE);
    }
    ((CButton*)GetDlgItem(svId[i]))->SetCheck(1);
  }
}

// CConfigRtkReferencePosition
IMPLEMENT_DYNAMIC(CConfigRtkReferencePosition, CCommonConfigDlg)

CConfigRtkReferencePosition::CConfigRtkReferencePosition(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CFG_RTK_REF_POSITION, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigRtkReferencePosition, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigRtkReferencePosition::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkMode 
BOOL CConfigRtkReferencePosition::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	GetDlgItem(IDC_STT_EDT1)->SetWindowText("");
	GetDlgItem(IDC_STT_EDT2)->SetWindowText("");
	GetDlgItem(IDC_STT_EDT3)->SetWindowText("");

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigRtkReferencePosition::OnBnClickedOk()
{	
	CString txt;
	((CEdit*)GetDlgItem(IDC_STT_EDT1))->GetWindowText(txt);
	m_sttValue1 = atof(txt);
	((CEdit*)GetDlgItem(IDC_STT_EDT2))->GetWindowText(txt);
	m_sttValue2 = atof(txt);
	((CEdit*)GetDlgItem(IDC_STT_EDT3))->GetWindowText(txt);
	m_sttValue3 = (float)atof(txt);

	OnOK();
}

void CConfigRtkReferencePosition::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(22);

	*cmd.GetBuffer(0) = 0x6A;
	*cmd.GetBuffer(1) = 0x0B;
	//D64
	*cmd.GetBuffer(2) = *(((U08*)(&m_sttValue1)) + 7);
	*cmd.GetBuffer(3) = *(((U08*)(&m_sttValue1)) + 6);
	*cmd.GetBuffer(4) = *(((U08*)(&m_sttValue1)) + 5);
	*cmd.GetBuffer(5) = *(((U08*)(&m_sttValue1)) + 4);
	*cmd.GetBuffer(6) = *(((U08*)(&m_sttValue1)) + 3);
	*cmd.GetBuffer(7) = *(((U08*)(&m_sttValue1)) + 2);
	*cmd.GetBuffer(8) = *(((U08*)(&m_sttValue1)) + 1);
	*cmd.GetBuffer(9) = *(((U08*)(&m_sttValue1)) + 0);
	//D64
	*cmd.GetBuffer(10) = *(((U08*)(&m_sttValue2)) + 7);
	*cmd.GetBuffer(11) = *(((U08*)(&m_sttValue2)) + 6);
	*cmd.GetBuffer(12) = *(((U08*)(&m_sttValue2)) + 5);
	*cmd.GetBuffer(13) = *(((U08*)(&m_sttValue2)) + 4);
	*cmd.GetBuffer(14) = *(((U08*)(&m_sttValue2)) + 3);
	*cmd.GetBuffer(15) = *(((U08*)(&m_sttValue2)) + 2);
	*cmd.GetBuffer(16) = *(((U08*)(&m_sttValue2)) + 1);
	*cmd.GetBuffer(17) = *(((U08*)(&m_sttValue2)) + 0);
	//F32
	*cmd.GetBuffer(18) = *(((U08*)(&m_sttValue3)) + 3);
	*cmd.GetBuffer(19) = *(((U08*)(&m_sttValue3)) + 2);
	*cmd.GetBuffer(20) = *(((U08*)(&m_sttValue3)) + 1);
	*cmd.GetBuffer(21) = *(((U08*)(&m_sttValue3)) + 0);

	configCmd.SetData(cmd);
	configPrompt = "Configure RTK Reference Position successfully";

	AfxBeginThread(ConfigThread, 0);
}

// ConfigRtcmMeasurementDataOutDlg 
IMPLEMENT_DYNAMIC(ConfigRtcmMeasurementDataOutDlg, CCommonConfigDlg)

ConfigRtcmMeasurementDataOutDlg::ConfigRtcmMeasurementDataOutDlg(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CONFIG_RTCM_MEA_DAT_OUT, pParent)
{
}

BEGIN_MESSAGE_MAP(ConfigRtcmMeasurementDataOutDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDC_FIELD2, &ConfigRtcmMeasurementDataOutDlg::OnBnClickedField2)
	ON_BN_CLICKED(IDOK, &ConfigRtcmMeasurementDataOutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// ConfigBinaryMeasurementDataOutDlg
BOOL ConfigRtcmMeasurementDataOutDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
  CString title;
  BinaryData ackCmd;
  this->GetWindowText(title);

  if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QueryRtcmMeasurementDataOut(CGPSDlg::Return, &ackCmd))
  {
    title += " (Query success)";
    ((CButton*)GetDlgItem(IDC_FIELD2))->SetCheck(ackCmd[5]);
    ((CComboBox*)GetDlgItem(IDC_FIELD3))->SetCurSel(ackCmd[6]);
    ((CButton*)GetDlgItem(IDC_FIELD4))->SetCheck(ackCmd[7]);
    ((CButton*)GetDlgItem(IDC_FIELD5))->SetCheck(ackCmd[8]);
    ((CButton*)GetDlgItem(IDC_FIELD6))->SetCheck(ackCmd[9]);
    ((CButton*)GetDlgItem(IDC_FIELD8))->SetCheck(ackCmd[11]);
    ((CButton*)GetDlgItem(IDC_FIELD9))->SetCheck(ackCmd[12]);
    ((CButton*)GetDlgItem(IDC_FIELD10))->SetCheck(ackCmd[13]);
    ((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
  }
  else
  {
    title += " (Query failed)";
    ((CButton*)GetDlgItem(IDC_FIELD2))->SetCheck(1);
    ((CComboBox*)GetDlgItem(IDC_FIELD3))->SetCurSel(0);
    ((CButton*)GetDlgItem(IDC_FIELD4))->SetCheck(1);
    ((CButton*)GetDlgItem(IDC_FIELD5))->SetCheck(1);
    ((CButton*)GetDlgItem(IDC_FIELD6))->SetCheck(1);
    ((CButton*)GetDlgItem(IDC_FIELD8))->SetCheck(1);
    ((CButton*)GetDlgItem(IDC_FIELD9))->SetCheck(1);
    ((CButton*)GetDlgItem(IDC_FIELD10))->SetCheck(1);
    ((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
  }

  if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QueryConstellationCapability(CGPSDlg::Return, &ackCmd))
  {
    cnstMode = MAKEWORD(ackCmd[7], ackCmd[6]);
  }
  else
  {
    cnstMode = 0xFFFF;
  }
  this->SetWindowText(title);
	UpdateStatus();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void ConfigRtcmMeasurementDataOutDlg::OnBnClickedField2()
{
	UpdateStatus();
}

void ConfigRtcmMeasurementDataOutDlg::OnBnClickedOk()
{
	m_field2 = ((CButton*)GetDlgItem(IDC_FIELD2))->GetCheck();
	m_field3 = ((CComboBox*)GetDlgItem(IDC_FIELD3))->GetCurSel();
	m_field4 = ((CButton*)GetDlgItem(IDC_FIELD4))->GetCheck();
	m_field5 = ((CButton*)GetDlgItem(IDC_FIELD5))->GetCheck();
	m_field6 = ((CButton*)GetDlgItem(IDC_FIELD6))->GetCheck();
	m_field8 = ((CButton*)GetDlgItem(IDC_FIELD8))->GetCheck();
	m_field9 = ((CButton*)GetDlgItem(IDC_FIELD9))->GetCheck();
	m_field10 = ((CButton*)GetDlgItem(IDC_FIELD10))->GetCheck();
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void ConfigRtcmMeasurementDataOutDlg::UpdateStatus()
{
  BOOL enable = ((CButton*)GetDlgItem(IDC_FIELD2))->GetCheck();

	GetDlgItem(IDC_FIELD3)->EnableWindow(enable);
	GetDlgItem(IDC_FIELD4)->EnableWindow(enable);
	GetDlgItem(IDC_FIELD5)->EnableWindow(enable);
	GetDlgItem(IDC_FIELD6)->EnableWindow(enable);
  if((cnstMode & 0x0002) == 0)
  { //Not GLONASS modules
    GetDlgItem(IDC_FIELD6)->EnableWindow(FALSE);
    ((CButton*)GetDlgItem(IDC_FIELD6))->SetCheck(0);
  }
	GetDlgItem(IDC_FIELD8)->EnableWindow(enable);
	GetDlgItem(IDC_FIELD9)->EnableWindow(enable);
	GetDlgItem(IDC_FIELD10)->EnableWindow(enable);
  if((cnstMode & 0x0008) == 0)
  { //Not Beidou modules
    GetDlgItem(IDC_FIELD10)->EnableWindow(FALSE);
    ((CButton*)GetDlgItem(IDC_FIELD10))->SetCheck(0);
  } 
}

void ConfigRtcmMeasurementDataOutDlg::DoCommand()
{
	BinaryData cmd(17);
	*cmd.GetBuffer(0) = 0x20;
	*cmd.GetBuffer(1) = (U08)m_field2;
	*cmd.GetBuffer(2) = (U08)m_field3;
	*cmd.GetBuffer(3) = (U08)m_field4;
	*cmd.GetBuffer(4) = (U08)m_field5;
	*cmd.GetBuffer(5) = (U08)m_field6;
	*cmd.GetBuffer(7) = (U08)m_field8;
	*cmd.GetBuffer(8) = (U08)m_field9;
	*cmd.GetBuffer(9) = (U08)m_field10;
	*cmd.GetBuffer(16) = (U08)m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure RtcmMeasurementDataOut successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigWatchTrackback 
IMPLEMENT_DYNAMIC(CConfigWatchTrackback, CCommonConfigDlg)

CConfigWatchTrackback::CConfigWatchTrackback(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_RTK_MODE, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigWatchTrackback, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigWatchTrackback::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigWatchTrackback 
BOOL CConfigWatchTrackback::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	//((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	//((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigWatchTrackback::OnBnClickedOk()
{	
	//m_mode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	//m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigWatchTrackback::DoCommand()
{
	//CWaitCursor wait;
	//BinaryData cmd(4);
	//*cmd.GetBuffer(0) = 0x6A;
	//*cmd.GetBuffer(1) = 0x01;
	//*cmd.GetBuffer(2) = (U08)m_mode;
	//*cmd.GetBuffer(3) = (U08)m_attribute;

	//configCmd.SetData(cmd);
	//configPrompt = "Configure RTK mode successfully";
	//if(m_mode == 1)	//Base mode
	//{	//Base mode should set to 1 Hz.
	//	AfxBeginThread(ConfigRtkThread, 0);
	//}
	//else
	//{
	//	AfxBeginThread(ConfigThread, 0);
	//}
}


// CLogConfigureControlDlg 
IMPLEMENT_DYNAMIC(CLogConfigureControlDlg, CCommonConfigDlg)

CLogConfigureControlDlg::CLogConfigureControlDlg(CWnd* pParent /*=NULL*/, CmdType cmd)
	: CCommonConfigDlg(IDD_LOG_FILTER_DLG, pParent)
{
  m_cmd = cmd;
}

BEGIN_MESSAGE_MAP(CLogConfigureControlDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CLogConfigureControlDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_ENABLE, &CLogConfigureControlDlg::OnBnClickedEnable)

END_MESSAGE_MAP()

// ConfigRtcmMeasurementDataOutDlg
BOOL CLogConfigureControlDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
  CString title;
  BinaryData ackCmd;
  this->GetWindowText(title);

  CGPSDlg::CmdErrorCode ack = CGPSDlg::Timeout;
  int startI = 0;
  if(m_cmd == Cmd17h)
  {
    ack = CGPSDlg::gpsDlg->QueryDatalogLogStatus(CGPSDlg::Return, &ackCmd);
    startI = 13;
  }
  else if(m_cmd == Cmd740Ch)
  {
    ack = CGPSDlg::gpsDlg->QueryDatalogWatchLogStatus(CGPSDlg::Return, &ackCmd);
    startI = 15;
  }

  if(CGPSDlg::Ack == ack)
  {
    title += " (Query success)";
    int dataBase = startI;
    DisplayStatic(this, IDC_EMAXT, "%d", MAKELONG(MAKEWORD(ackCmd[dataBase], ackCmd[dataBase + 1]), MAKEWORD(ackCmd[dataBase + 2], ackCmd[dataBase + 3])));
    DisplayStatic(this, IDC_EMINT, "%d", MAKELONG(MAKEWORD(ackCmd[dataBase + 4], ackCmd[dataBase + 5]), MAKEWORD(ackCmd[dataBase + 6], ackCmd[dataBase + 7])));
    DisplayStatic(this, IDC_EMAXD, "%d", MAKELONG(MAKEWORD(ackCmd[dataBase + 8], ackCmd[dataBase + 9]), MAKEWORD(ackCmd[dataBase + 10], ackCmd[dataBase + 11])));
    DisplayStatic(this, IDC_EMIND, "%d", MAKELONG(MAKEWORD(ackCmd[dataBase + 12], ackCmd[dataBase + 13]), MAKEWORD(ackCmd[dataBase + 14], ackCmd[dataBase + 15])));
    DisplayStatic(this, IDC_EMAXV, "%d", MAKELONG(MAKEWORD(ackCmd[dataBase + 16], ackCmd[dataBase + 17]), MAKEWORD(ackCmd[dataBase + 18], ackCmd[dataBase + 19])));
    DisplayStatic(this, IDC_EMINV, "%d", MAKELONG(MAKEWORD(ackCmd[dataBase + 20], ackCmd[dataBase + 21]), MAKEWORD(ackCmd[dataBase + 22], ackCmd[dataBase + 23])));
    ((CButton*)GetDlgItem(IDC_ENABLE))->SetCheck(ackCmd[dataBase + 24]);
    ((CComboBox*)GetDlgItem(IDC_FIFOMODE))->SetCurSel(ackCmd[dataBase + 25]);
  }
  else
  {
    DisplayStatic(this, IDC_EMAXT, "%d", 3600);
    DisplayStatic(this, IDC_EMINT, "%d", 5);
    DisplayStatic(this, IDC_EMAXD, "%d", 100);
    DisplayStatic(this, IDC_EMIND, "%d", 0);
    DisplayStatic(this, IDC_EMAXV, "%d", 100);
    DisplayStatic(this, IDC_EMINV, "%d", 0);
    ((CButton*)GetDlgItem(IDC_ENABLE))->SetCheck(FALSE);
    ((CComboBox*)GetDlgItem(IDC_FIFOMODE))->SetCurSel(0);
  }
  this->SetWindowText(title);
	UpdateStatus();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CLogConfigureControlDlg::OnBnClickedOk()
{
  CString txt;

	m_enable = ((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();
	GetDlgItem(IDC_EMAXT)->GetWindowText(txt);
	m_maxT = atoi(txt);
	GetDlgItem(IDC_EMINT)->GetWindowText(txt);
	m_minT = atoi(txt);
	GetDlgItem(IDC_EMAXD)->GetWindowText(txt);
	m_maxD = atoi(txt);
	GetDlgItem(IDC_EMIND)->GetWindowText(txt);
	m_minD = atoi(txt);
	GetDlgItem(IDC_EMAXV)->GetWindowText(txt);
	m_maxV = atoi(txt);
	GetDlgItem(IDC_EMINV)->GetWindowText(txt);
	m_minV = atoi(txt);
  m_fifoMode = ((CComboBox*)GetDlgItem(IDC_FIFOMODE))->GetCurSel();

  OnOK();
}

void CLogConfigureControlDlg::UpdateStatus()
{
  BOOL enable = ((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();

	GetDlgItem(IDC_EMAXT)->EnableWindow(enable);
	GetDlgItem(IDC_EMINT)->EnableWindow(enable);
	GetDlgItem(IDC_EMAXD)->EnableWindow(enable);
	GetDlgItem(IDC_EMIND)->EnableWindow(enable);
	GetDlgItem(IDC_EMAXV)->EnableWindow(enable);
	GetDlgItem(IDC_EMINV)->EnableWindow(enable);
	GetDlgItem(IDC_FIFOMODE)->EnableWindow(enable);
}

void CLogConfigureControlDlg::OnBnClickedEnable()
{
	UpdateStatus();
}

void CLogConfigureControlDlg::DoCommand()
{
	BinaryData cmd;
  int i = 0;
  if(m_cmd == Cmd17h)
  {
    cmd.Alloc(27);
	  *cmd.GetBuffer(i++) = 0x18;
  }
  else if(m_cmd == Cmd740Ch)
  {
     cmd.Alloc(29);
    *cmd.GetBuffer(i++) = 0x7A;
    *cmd.GetBuffer(i++) = 0x0C;
    *cmd.GetBuffer(i++) = 0x0D;
  }
	*cmd.GetBuffer(i++) = HIBYTE(HIWORD(m_maxT));
	*cmd.GetBuffer(i++) = LOBYTE(HIWORD(m_maxT));
	*cmd.GetBuffer(i++) = HIBYTE(LOWORD(m_maxT));
	*cmd.GetBuffer(i++) = LOBYTE(LOWORD(m_maxT));

 	*cmd.GetBuffer(i++) = HIBYTE(HIWORD(m_minT));
	*cmd.GetBuffer(i++) = LOBYTE(HIWORD(m_minT));
	*cmd.GetBuffer(i++) = HIBYTE(LOWORD(m_minT));
	*cmd.GetBuffer(i++) = LOBYTE(LOWORD(m_minT));

	*cmd.GetBuffer(i++) = HIBYTE(HIWORD(m_maxD));
	*cmd.GetBuffer(i++) = LOBYTE(HIWORD(m_maxD));
	*cmd.GetBuffer(i++) = HIBYTE(LOWORD(m_maxD));
	*cmd.GetBuffer(i++) = LOBYTE(LOWORD(m_maxD));

	*cmd.GetBuffer(i++) = HIBYTE(HIWORD(m_minD));
	*cmd.GetBuffer(i++) = LOBYTE(HIWORD(m_minD));
	*cmd.GetBuffer(i++) = HIBYTE(LOWORD(m_minD));
	*cmd.GetBuffer(i++) = LOBYTE(LOWORD(m_minD));

	*cmd.GetBuffer(i++) = HIBYTE(HIWORD(m_maxV));
	*cmd.GetBuffer(i++) = LOBYTE(HIWORD(m_maxV));
	*cmd.GetBuffer(i++) = HIBYTE(LOWORD(m_maxV));
	*cmd.GetBuffer(i++) = LOBYTE(LOWORD(m_maxV));

	*cmd.GetBuffer(i++) = HIBYTE(HIWORD(m_minV));
	*cmd.GetBuffer(i++) = LOBYTE(HIWORD(m_minV));
	*cmd.GetBuffer(i++) = HIBYTE(LOWORD(m_minV));
	*cmd.GetBuffer(i++) = LOBYTE(LOWORD(m_minV));
  
  *cmd.GetBuffer(i++) = m_enable;
  *cmd.GetBuffer(i++) = m_fifoMode;

	configCmd.SetData(cmd);
	configPrompt = "Log Configure Data Logging Criteria successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CPositionUpdateRateDlg 
IMPLEMENT_DYNAMIC(CPositionUpdateRateDlg, CCommonConfigDlg)

CPositionUpdateRateDlg::CPositionUpdateRateDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_UPDATE_RATE_DLG, pParent)
{

}

BEGIN_MESSAGE_MAP(CPositionUpdateRateDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CPositionUpdateRateDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CPositionUpdateRateDlg 
BOOL CPositionUpdateRateDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
	GetDlgItem(IDC_WARNING)->SetWindowText("Warning :\r\nPlease disable SAEE before using multi-hz position update rate.");
	((CComboBox*)GetDlgItem(IDC_UPDATE_RATE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTRIBUTE))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CPositionUpdateRateDlg::OnBnClickedOk()
{
  CString txt;
  CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_UPDATE_RATE);
  pCmb->GetLBText(pCmb->GetCurSel(), txt);
  m_rate = atoi(txt);

	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTRIBUTE))->GetCurSel();

	OnOK();
}

UINT AFX_CDECL ConfigUpdateRateThread(LPVOID param)
{
	int rate = *((U08*)param);
	//CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), configPrompt, false);
	CGPSDlg::gpsDlg->ClearQue();
	if(CGPSDlg::gpsDlg->SendToTarget(configCmd.GetBuffer(), configCmd.Size(), configPrompt, true))
	{
#if(!NO_BOOST_UPDATE_RATE_CHANGE)
		Sleep(200);
#if(MODULE_SUP_800)
    // 20160810, request from Andrew, SUP800 fw can't support baud rate more than 230400
		if(rate > 20 && g_setting.GetBaudrate() < 230400)				//Boost to 230400 when update rate > 20Hz.
		{
			CGPSDlg::gpsDlg->ConfigBaudrate(6, rate);	
		}
#else
    if(rate > 40 && g_setting.GetBaudrate() < 961600)				//Boost to 961600 when update rate > 40Hz.
		{
			CGPSDlg::gpsDlg->ConfigBaudrate(8, rate);	
		}
		else if(rate > 25 && g_setting.GetBaudrate() < 460800)				//Boost to 460800 when update rate > 25Hz.
		{
			CGPSDlg::gpsDlg->ConfigBaudrate(7, rate);	
		}
		else if(rate > 20 && g_setting.GetBaudrate() < 230400)				//Boost to 230400 when update rate > 20Hz.
		{
			CGPSDlg::gpsDlg->ConfigBaudrate(6, rate);	
		}
#endif
		else if(rate > 10 && g_setting.GetBaudrate() < 115200)				//Boost to 115200 when update rate > 10Hz.
		{
			CGPSDlg::gpsDlg->ConfigBaudrate(5, rate);	
		}
		else if(rate > 2 && g_setting.GetBaudrate() < 38400)			//Boost to 38400 when update rate > 2Hz.
		{
			CGPSDlg::gpsDlg->ConfigBaudrate(3, rate);	
		}
		else if(rate > 1 && g_setting.GetBaudrate() < 9600)			//Boost to 9600 when update rate > 1Hz.
		{
			CGPSDlg::gpsDlg->ConfigBaudrate(1, rate);	
		
		//else
		//{
        //Andrew request 20160901, doesn't need restart in here
		  	//CGPSDlg::gpsDlg->SendRestartCommand(1);
		//}
#endif
	}
	CGPSDlg::gpsDlg->SetMode();
	CGPSDlg::gpsDlg->CreateGPSThread();
	return 0;
}

void CPositionUpdateRateDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(3);
	*cmd.GetBuffer(0) = 0x0E;
	*cmd.GetBuffer(1) = m_rate;
	*cmd.GetBuffer(2) = m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure position update rate successfully";
  AfxBeginThread(ConfigUpdateRateThread, &m_rate);
}

// CDrUpdateRateDlg 
IMPLEMENT_DYNAMIC(CDrUpdateRateDlg, CCommonConfigDlg)

CDrUpdateRateDlg::CDrUpdateRateDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_DR_UPDATE_RATE_DLG, pParent)
{

}

BEGIN_MESSAGE_MAP(CDrUpdateRateDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CDrUpdateRateDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CPositionUpdateRateDlg 
BOOL CDrUpdateRateDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
  CString title;
  BinaryData ackCmd;
  const int convertArray[] = { -1, 0, 1, -1, 2, 3, -1, -1, 4, -1, 5 };
  //1;2;4;5;8;10;
  this->GetWindowText(title);
  if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QueryDrRate(CGPSDlg::Return, &ackCmd))
  {
    title += " (Query success)";
    int rateIdx = (ackCmd[6] < Dim(convertArray)) ? convertArray[ackCmd[6]] : -1;
	  ((CComboBox*)GetDlgItem(IDC_UPDATE_RATE))->SetCurSel(rateIdx);
  }
  else
  {
    title += " (Query failed)";
	  ((CComboBox*)GetDlgItem(IDC_UPDATE_RATE))->SetCurSel(0);
  }
  this->SetWindowText(title);
	((CComboBox*)GetDlgItem(IDC_ATTRIBUTE))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDrUpdateRateDlg::OnBnClickedOk()
{
  CString txt;
  CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_UPDATE_RATE);
  pCmb->GetLBText(pCmb->GetCurSel(), txt);
  m_rate = atoi(txt);
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTRIBUTE))->GetCurSel();

	OnOK();
}

void CDrUpdateRateDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x6C;
	*cmd.GetBuffer(1) = 0x02;
	*cmd.GetBuffer(2) = m_rate;
	*cmd.GetBuffer(3) = m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure DR predict update rate successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CDrRawRateDlg 
IMPLEMENT_DYNAMIC(CDrRawRateDlg, CCommonConfigDlg)

CDrRawRateDlg::CDrRawRateDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_DR_RAW_RATE_DLG, pParent)
{

}

BEGIN_MESSAGE_MAP(CDrRawRateDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CDrRawRateDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CDrRawRateDlg 
BOOL CDrRawRateDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
  CString title;
  BinaryData ackCmd;
  const int convertArray[] = { 0, 1, 2, -1, 3, 4, -1, -1, 5, -1, 6 };
  //1;2;4;5;8;10;
  this->GetWindowText(title);
  if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QueryDrRawRate(CGPSDlg::Return, &ackCmd))
  {
    int rateIdx = 0;
    if(ackCmd[6] <= 10)
    {
      rateIdx = (ackCmd[6] < Dim(convertArray)) ? convertArray[ackCmd[6]] : -1;
    }

    title += " (Query success)";
	  ((CComboBox*)GetDlgItem(IDC_UPDATE_RATE))->SetCurSel(rateIdx);
  }
  else
  {
    title += " (Query failed)";
	  ((CComboBox*)GetDlgItem(IDC_UPDATE_RATE))->SetCurSel(0);
  }
  this->SetWindowText(title);
	((CComboBox*)GetDlgItem(IDC_ATTRIBUTE))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDrRawRateDlg::OnBnClickedOk()
{
  CString txt;
  CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_UPDATE_RATE);
  pCmb->GetLBText(pCmb->GetCurSel(), txt);
  m_rate = atoi(txt);
	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTRIBUTE))->GetCurSel();

	OnOK();
}

void CDrRawRateDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x6C;
	*cmd.GetBuffer(1) = 0x04;
	*cmd.GetBuffer(2) = m_rate;
	*cmd.GetBuffer(3) = m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure DR RAW sensor data output rate successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigRtkSlaveBaud 
IMPLEMENT_DYNAMIC(CConfigRtkSlaveBaud, CCommonConfigDlg)

CConfigRtkSlaveBaud::CConfigRtkSlaveBaud(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_RTK_SLAVE_BAUD, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigRtkSlaveBaud, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigRtkSlaveBaud::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigRtkSlaveBaud 
BOOL CConfigRtkSlaveBaud::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
  CString title;
  BinaryData ackCmd;

  CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_BAUD_RATE);
  g_setting.InitBaudrateCombo(pBox);

  this->GetWindowText(title);
  CGPSDlg::CmdErrorCode ack = CGPSDlg::Timeout;
  ack = CGPSDlg::gpsDlg->QueryRtkSlaveBaud(CGPSDlg::Return, &ackCmd);

  if(CGPSDlg::Ack == ack)
  {
    title += " (Query success)";
    pBox->SetCurSel(ackCmd[6]);
  }
  else
  {
    title += " (Query failed)";
    pBox->SetCurSel(5);
  }
	this->SetWindowText(title);;
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigRtkSlaveBaud::OnBnClickedOk()
{	
	CString txt;
	m_rate = ((CComboBox*)GetDlgItem(IDC_BAUD_RATE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();
	OnOK();
}

void CConfigRtkSlaveBaud::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x6A;
	*cmd.GetBuffer(1) = 0x0C;
	*cmd.GetBuffer(2) = m_rate;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

  configCmd.SetData(cmd);
	configPrompt = "Configure RTK slave serial port baud rate successfully";
  AfxBeginThread(ConfigThread, 0);
}

// C1ppsOutputModeDlg 
IMPLEMENT_DYNAMIC(C1ppsOutputModeDlg, CCommonConfigDlg)

C1ppsOutputModeDlg::C1ppsOutputModeDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_1PPS_OUTPUT_MODE, pParent)
{

}

BEGIN_MESSAGE_MAP(C1ppsOutputModeDlg, CCommonConfigDlg)
  ON_BN_CLICKED(IDC_NO_OUTPUT, &C1ppsOutputModeDlg::OnBnClickedNoOutput)
  ON_BN_CLICKED(IDC_HAVE_GPS_TIME, &C1ppsOutputModeDlg::OnBnClickedOutputHaveGpsTime)
	ON_BN_CLICKED(IDC_OUTPUT_ALIGN, &C1ppsOutputModeDlg::OnBnClickedOutputAlign)
	
	ON_BN_CLICKED(IDC_ALIGN_TO_GPS, &C1ppsOutputModeDlg::OnBnClickedAlignToGps)
	ON_BN_CLICKED(IDC_ALIGN_TO_UTC, &C1ppsOutputModeDlg::OnBnClickedAlignToUtc)
	ON_BN_CLICKED(IDC_ALIGN_TO_NAVIC, &C1ppsOutputModeDlg::OnBnClickedAlignToNavic)

	ON_BN_CLICKED(IDOK, &C1ppsOutputModeDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// C1ppsOutputModeDlg 
BOOL C1ppsOutputModeDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

  ((CButton*)GetDlgItem(IDC_NO_OUTPUT))->SetCheck(TRUE);
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_GPS))->SetCheck(TRUE);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

  if(!PPS_MODE_SUPPORT_ALIGN_TO_NAVIC)
  {
    GetDlgItem(IDC_ALIGN_TO_NAVIC)->ShowWindow(SW_HIDE);
  }
	return TRUE;  // return TRUE unless you set the focus to a control
}

void C1ppsOutputModeDlg::OnBnClickedNoOutput()
{
  ((CButton*)GetDlgItem(IDC_NO_OUTPUT))->SetCheck(TRUE);
  ((CButton*)GetDlgItem(IDC_HAVE_GPS_TIME))->SetCheck(FALSE);
  ((CButton*)GetDlgItem(IDC_OUTPUT_ALIGN))->SetCheck(FALSE);
}

void C1ppsOutputModeDlg::OnBnClickedOutputHaveGpsTime()
{
  ((CButton*)GetDlgItem(IDC_NO_OUTPUT))->SetCheck(FALSE);
  ((CButton*)GetDlgItem(IDC_HAVE_GPS_TIME))->SetCheck(TRUE);
  ((CButton*)GetDlgItem(IDC_OUTPUT_ALIGN))->SetCheck(FALSE);
}

void C1ppsOutputModeDlg::OnBnClickedOutputAlign()
{
  ((CButton*)GetDlgItem(IDC_NO_OUTPUT))->SetCheck(FALSE);
  ((CButton*)GetDlgItem(IDC_HAVE_GPS_TIME))->SetCheck(FALSE);
  ((CButton*)GetDlgItem(IDC_OUTPUT_ALIGN))->SetCheck(TRUE);
}

void C1ppsOutputModeDlg::OnBnClickedAlignToGps()
{
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_GPS))->SetCheck(TRUE);
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_UTC))->SetCheck(FALSE);
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_NAVIC))->SetCheck(FALSE);
}

void C1ppsOutputModeDlg::OnBnClickedAlignToUtc()
{
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_GPS))->SetCheck(FALSE);
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_UTC))->SetCheck(TRUE);
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_NAVIC))->SetCheck(FALSE);
}

void C1ppsOutputModeDlg::OnBnClickedAlignToNavic()
{
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_GPS))->SetCheck(FALSE);
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_UTC))->SetCheck(FALSE);
  ((CButton*)GetDlgItem(IDC_ALIGN_TO_NAVIC))->SetCheck(TRUE);
}

void C1ppsOutputModeDlg::OnBnClickedOk()
{
  if(((CButton*)GetDlgItem(IDC_NO_OUTPUT))->GetCheck())
  {
    m_mode = 0;
  }
  else if(((CButton*)GetDlgItem(IDC_HAVE_GPS_TIME))->GetCheck())
  {
    m_mode = 1;
  }
  else if(((CButton*)GetDlgItem(IDC_OUTPUT_ALIGN))->GetCheck())
  {
    m_mode = 2;
  }  

  if(((CButton*)GetDlgItem(IDC_ALIGN_TO_GPS))->GetCheck())
  {
    m_align = 0;
  }
  else if(((CButton*)GetDlgItem(IDC_ALIGN_TO_UTC))->GetCheck())
  {
    m_align = 1;
  }
  else if(((CButton*)GetDlgItem(IDC_ALIGN_TO_NAVIC))->GetCheck())
  {
    m_align = 2;
  } 

	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();
	OnOK();
}

void C1ppsOutputModeDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x55;
	*cmd.GetBuffer(1) = m_mode;
	*cmd.GetBuffer(2) = m_align;
	*cmd.GetBuffer(3) = m_attribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure 1PPS output mode successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigureRfIc 
/*
IMPLEMENT_DYNAMIC(CConfigureRfIc, CCommonConfigDlg)

CConfigureRfIc::CConfigureRfIc(CWnd* pParent)
: CCommonConfigDlg(IDD_CFG_RF_IC, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigureRfIc, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigureRfIc::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_TYPE, OnCbnSelChangeRtkMode)
END_MESSAGE_MAP()

// CConfigureRfIc 
UINT CConfigureRfIc::staticCtrlList[Setting::RFIC_REG_SIZE] = 
  { IDC_T_REG0, IDC_T_REG1, IDC_T_REG2, IDC_T_REG3, IDC_T_REG4, IDC_T_REG5,
    IDC_T_REG6, IDC_T_REG7, IDC_T_REG8, IDC_T_REG9, IDC_T_REG10, IDC_T_REG11,
    IDC_T_REG12, IDC_T_REG13, IDC_T_REG14, IDC_T_REG15, IDC_T_REG16, IDC_T_REG17 };

UINT CConfigureRfIc::editCtrlList[Setting::RFIC_REG_SIZE] = 
  { IDC_REG0, IDC_REG1, IDC_REG2, IDC_REG3, IDC_REG4, IDC_REG5,
    IDC_REG6, IDC_REG7, IDC_REG8, IDC_REG9, IDC_REG10, IDC_REG11,
    IDC_REG12, IDC_REG13, IDC_REG14, IDC_REG15, IDC_REG16, IDC_REG17 };

BOOL CConfigureRfIc::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();
  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
	  ((CEdit*)GetDlgItem(editCtrlList[i]))->SetLimitText(8);
  }

  ((CComboBox*)GetDlgItem(IDC_TYPE))->SetCurSel(g_setting.configRfIcType);
  OnCbnSelChangeRtkMode();

  CString txt;
  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
    txt.Format("%08X", g_setting.configRfIcReg[i]);
    GetDlgItem(editCtrlList[i])->SetWindowText(txt);
  }

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigureRfIc::OnBnClickedOk()
{	
  m_type = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();
  g_setting.configRfIcType = m_type;

  CString txt;
  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
    GetDlgItem(editCtrlList[i])->GetWindowText(txt);
	  m_reg[i] = ConvertCharToU32(txt);
    g_setting.configRfIcReg[i] = m_reg[i];

  }

  OnOK();
}

void CConfigureRfIc::OnCbnSelChangeRtkMode()
{
  const char* type0_name[Setting::RFIC_REG_SIZE] = 
    { "R00", "R01", "R02", "---", "---", "---",
      "---", "---", "---", "---", "---", "---",
      "---", "---", "---", "---", "---", "---" };

  const char* type1_name[Setting::RFIC_REG_SIZE] = 
    { "R02", "R04", "R06", "R10", "R12", "R13",
      "R14", "R16", "R17", "R18", "R20", "R49",
      "R15", "R15", "R19", "R19", "R23", "R23" };
  int t = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();

  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
    if(t == 0)
    {
      GetDlgItem(staticCtrlList[i])->SetWindowText(type0_name[i]);
      GetDlgItem(staticCtrlList[i])->EnableWindow(type0_name[i][0] != '-');
    }
    else if(t == 1)
    {
      GetDlgItem(staticCtrlList[i])->SetWindowText(type1_name[i]);
      GetDlgItem(staticCtrlList[i])->EnableWindow(TRUE);
    }
  }
}

void CConfigureRfIc::DoCommand()
{
  CGPSDlg::gpsDlg->TerminateGPSThread();
  int idx = 0;
	BinaryData cmd(75);
	*cmd.GetBuffer(idx++) = 0x64;
	*cmd.GetBuffer(idx++) = 0x78;
  //U08
	*cmd.GetBuffer(idx++) = m_type;
  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
	  //U32 
	  *cmd.GetBuffer(idx++) = HIBYTE(HIWORD(m_reg[i]));
	  *cmd.GetBuffer(idx++) = LOBYTE(HIWORD(m_reg[i]));
	  *cmd.GetBuffer(idx++) = HIBYTE(LOWORD(m_reg[i]));
	  *cmd.GetBuffer(idx++) = LOBYTE(LOWORD(m_reg[i]));
  }

	configCmd.SetData(cmd);
  configPrompt = "Configure RF IC successfully";
  AfxBeginThread(ConfigThread, (LPVOID)0);
}
*/
// CConfigRtkGlCpifBias 
IMPLEMENT_DYNAMIC(CConfigRtkGlCpifBias, CCommonConfigDlg)

CConfigRtkGlCpifBias::CConfigRtkGlCpifBias(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_RTK_GL_CPIF_BIAS, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigRtkGlCpifBias, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigRtkGlCpifBias::OnBnClickedOk)
  ON_CBN_SELCHANGE(IDC_MODE, &CConfigRtkGlCpifBias::OnCbnSelchangeMode)
  ON_BN_CLICKED(IDC_MASK_L1, &CConfigRtkGlCpifBias::OnBnClickedMask)
  ON_BN_CLICKED(IDC_MASK_L2, &CConfigRtkGlCpifBias::OnBnClickedMask)
END_MESSAGE_MAP()

// CConfigRtkGlCpifBias 
BOOL CConfigRtkGlCpifBias::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

  BinaryData ackCmd;
  CString txt, title;
  this->GetWindowText(title);
	if(CGPSDlg::Ack == CGPSDlg::gpsDlg->QueryRtkCpifBias(CGPSDlg::Return, &ackCmd))
	{
    title += " (Query success)";
	  ((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(ackCmd[6]);
    ((CButton*)GetDlgItem(IDC_MASK_L1))->SetCheck((ackCmd[7] & 0x01) ? TRUE : FALSE);
    ((CButton*)GetDlgItem(IDC_MASK_L2))->SetCheck((ackCmd[7] & 0x02) ? TRUE : FALSE);
    
    DisplayStatic(this, IDC_CIFB_L1, "%f", ConvertLeonDouble(ackCmd.Ptr(8)));
    DisplayStatic(this, IDC_CIFB_L2, "%f", ConvertLeonDouble(ackCmd.Ptr(16)));
    
    DisplayStatic(this, IDC_REV01, "%d", ackCmd[24]);
    DisplayStatic(this, IDC_REV02, "%d", ackCmd[25]);
  }
  else
  {
    title += " (Query failed)";
	  ((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
    ((CButton*)GetDlgItem(IDC_MASK_L1))->SetCheck(FALSE);
    ((CButton*)GetDlgItem(IDC_MASK_L2))->SetCheck(FALSE);
    
    DisplayStatic(this, IDC_CIFB_L1, "%f", 0);
    DisplayStatic(this, IDC_CIFB_L2, "%f", 0);
    
    DisplayStatic(this, IDC_REV01, "%d", 0);
    DisplayStatic(this, IDC_REV02, "%d", 0);
  }
  UpdateStatus();
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);
  this->SetWindowText(title);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigRtkGlCpifBias::UpdateStatus()
{
  int mode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();

  if(mode == 0 || mode == 1)
  {
    GetDlgItem(IDC_MASK_L1)->EnableWindow(FALSE);
    GetDlgItem(IDC_MASK_L2)->EnableWindow(FALSE);
    GetDlgItem(IDC_CIFB_L1)->EnableWindow(FALSE);
    GetDlgItem(IDC_CIFB_L2)->EnableWindow(FALSE);
    GetDlgItem(IDC_REV01)->EnableWindow(TRUE);
    GetDlgItem(IDC_REV02)->EnableWindow(TRUE);
  }
  else
  {
    GetDlgItem(IDC_MASK_L1)->EnableWindow(TRUE);
    GetDlgItem(IDC_MASK_L2)->EnableWindow(TRUE);
    GetDlgItem(IDC_REV01)->EnableWindow(TRUE);
    GetDlgItem(IDC_REV02)->EnableWindow(TRUE);
    GetDlgItem(IDC_CIFB_L1)->EnableWindow(
      ((CButton*)GetDlgItem(IDC_MASK_L1))->GetCheck());
    GetDlgItem(IDC_CIFB_L2)->EnableWindow(
      ((CButton*)GetDlgItem(IDC_MASK_L2))->GetCheck());
  }
}

void CConfigRtkGlCpifBias::OnBnClickedOk()
{	
	CString txt;

  m_mode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_freqMask = ((CButton*)GetDlgItem(IDC_MASK_L1))->GetCheck() | 
      (((CButton*)GetDlgItem(IDC_MASK_L2))->GetCheck() << 1);

  GetDlgItem(IDC_CIFB_L1)->GetWindowText(txt);
	m_l1Cifb = atof(txt);
  
  GetDlgItem(IDC_CIFB_L2)->GetWindowText(txt);
	m_l2Cifb = atof(txt);

  GetDlgItem(IDC_REV01)->GetWindowText(txt);
  m_rev01 = atoi(txt);
  GetDlgItem(IDC_REV02)->GetWindowText(txt);
	m_rev02 = atoi(txt);

	m_attribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigRtkGlCpifBias::OnBnClickedMask()
{
  UpdateStatus();
}

void CConfigRtkGlCpifBias::OnCbnSelchangeMode()
{
  UpdateStatus();
}

void CConfigRtkGlCpifBias::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(26);
	*cmd.GetBuffer(0) = 0x6A;
	*cmd.GetBuffer(1) = 0x0F;
  //Mode
	*cmd.GetBuffer(2) = (U08)m_mode;
  //Frequency Mask
	*cmd.GetBuffer(3) = (U08)m_freqMask;

	//D64 L1 CIFB
  *cmd.GetBuffer(4) = *(((U08*)(&m_l1Cifb)) + 7);
  *cmd.GetBuffer(5) = *(((U08*)(&m_l1Cifb)) + 6);
  *cmd.GetBuffer(6) = *(((U08*)(&m_l1Cifb)) + 5);
  *cmd.GetBuffer(7) = *(((U08*)(&m_l1Cifb)) + 4);
  *cmd.GetBuffer(8) = *(((U08*)(&m_l1Cifb)) + 3);
  *cmd.GetBuffer(9) = *(((U08*)(&m_l1Cifb)) + 2);
  *cmd.GetBuffer(10) = *(((U08*)(&m_l1Cifb)) + 1);
  *cmd.GetBuffer(11) = *(((U08*)(&m_l1Cifb)) + 0);

	//D64 L1 CIFB
  *cmd.GetBuffer(12) = *(((U08*)(&m_l2Cifb)) + 7);
  *cmd.GetBuffer(13) = *(((U08*)(&m_l2Cifb)) + 6);
  *cmd.GetBuffer(14) = *(((U08*)(&m_l2Cifb)) + 5);
  *cmd.GetBuffer(15) = *(((U08*)(&m_l2Cifb)) + 4);
  *cmd.GetBuffer(16) = *(((U08*)(&m_l2Cifb)) + 3);
  *cmd.GetBuffer(17) = *(((U08*)(&m_l2Cifb)) + 2);
  *cmd.GetBuffer(18) = *(((U08*)(&m_l2Cifb)) + 1);
  *cmd.GetBuffer(19) = *(((U08*)(&m_l2Cifb)) + 0);
  
  //U08 reserved
	*cmd.GetBuffer(20) = (U08)m_rev01;
  //U32 reserved
	*cmd.GetBuffer(21) = HIBYTE(HIWORD(m_rev02));
	*cmd.GetBuffer(22) = LOBYTE(HIWORD(m_rev02));
	*cmd.GetBuffer(23) = HIBYTE(LOWORD(m_rev02));
	*cmd.GetBuffer(24) = LOBYTE(LOWORD(m_rev02));

	*cmd.GetBuffer(25) = (U08)m_attribute;
	configCmd.SetData(cmd);
	configPrompt = "Configure RTK GLONASS CIFB bias successfully";
  AfxBeginThread(ConfigThread, 0);
}


// CConfigCpuBoostMode 
IMPLEMENT_DYNAMIC(CConfigCpuBoostMode, CCommonConfigDlg)

CConfigCpuBoostMode::CConfigCpuBoostMode(CWnd* pParent /*=NULL*/)
	: CCommonConfigDlg(IDD_CFG_CPU_BOOST_MODE, pParent)
{
	m_nEnable = 0;
	m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CConfigCpuBoostMode, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigCpuBoostMode::OnBnClickedOk)
END_MESSAGE_MAP()

// CConfigVeryLowSpeed 
BOOL CConfigCpuBoostMode::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigCpuBoostMode::OnBnClickedOk()
{
	m_nEnable = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}

void CConfigCpuBoostMode::DoCommand()
{
	BinaryData cmd(4);
	*cmd.GetBuffer(0) = 0x64;
	*cmd.GetBuffer(1) = 0x77;
	*cmd.GetBuffer(2) = (U08)m_nEnable;
	*cmd.GetBuffer(3) = (U08)m_nAttribute;

	configCmd.SetData(cmd);
	configPrompt = "Configure ISR CPU clock boost mode successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CConfigureAlphaKeyDlg 
IMPLEMENT_DYNAMIC(CConfigureAlphaKeyDlg, CCommonConfigDlg)

CConfigureAlphaKeyDlg::CConfigureAlphaKeyDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_CONFIG_ALPHA_KEY, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigureAlphaKeyDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CConfigureAlphaKeyDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_KEY, &CConfigureAlphaKeyDlg::OnEnChangeKey)
END_MESSAGE_MAP()

// CConfigureAlphaKeyDlg 
BOOL CConfigureAlphaKeyDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigureAlphaKeyDlg::OnBnClickedOk()
{
	if(m_keyData.Size() != KeyLength)
	{
    ::AfxMessageBox("Incorrect Key Format!");
		return;
	}
	OnOK();
}

void CConfigureAlphaKeyDlg::OnEnChangeKey()
{
 	CString strInput;
	GetDlgItem(IDC_KEY)->GetWindowText(strInput);

	
	if(!Utility::ConvertHexToBinary(strInput, m_keyData))
	{
		GetDlgItem(IDC_NOTIFY)->SetWindowText("Invalidate Format!");
		return;
	}

	if(m_keyData.Size() != KeyLength)
	{
		GetDlgItem(IDC_NOTIFY)->SetWindowText("Incorrect Key Length!");
		return;
	}
  GetDlgItem(IDC_NOTIFY)->SetWindowText("");
}

void CConfigureAlphaKeyDlg::DoCommand()
{
	CWaitCursor wait;
	BinaryData cmd(19);
  int idx = 0;
	*cmd.GetBuffer(idx++) = 0x7A;
	*cmd.GetBuffer(idx++) = 0x08;
	*cmd.GetBuffer(idx++) = 0x7D;
  for(int i = 0; i < KeyLength; ++i)
  {
	  *cmd.GetBuffer(idx++) = m_keyData[i];
  }

	configCmd.SetData(cmd);
	configPrompt = "Configure Alpha RTK Key successfully";
  AfxBeginThread(ConfigThread, 0);
}

// CDumpMemoryDlg 
IMPLEMENT_DYNAMIC(CDumpMemoryDlg, CCommonConfigDlg)

CDumpMemoryDlg::CDumpMemoryDlg(CWnd* pParent /*=NULL*/)
: CCommonConfigDlg(IDD_DUMP_MEMORY, pParent)
{
  m_status = FALSE;
}

BEGIN_MESSAGE_MAP(CDumpMemoryDlg, CCommonConfigDlg)
	ON_BN_CLICKED(IDOK, &CDumpMemoryDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CDumpMemoryDlg 
BOOL CDumpMemoryDlg::OnInitDialog()
{
	CCommonConfigDlg::OnInitDialog();

  CRegistry reg;
  U32 addr, size;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey(VIEWER_REG_PATH, true))
	{
		addr = reg.ReadInt("dmtf_address", 0);
		size = reg.ReadInt("dmtf_size", 0);
	}
	else
	{
		addr = 0;
		size = 0;
	}

  CString txt;
  if(addr == 0)
  {
    ((CComboBox*)GetDlgItem(IDC_ADDR))->SetCurSel(0);
  }
  else
  {
    txt.Format("%08X", addr);
    ((CComboBox*)GetDlgItem(IDC_ADDR))->SetWindowText(txt);
  }

  if(size == 0)
  {
    ((CComboBox*)GetDlgItem(IDC_DUMP_SIZE))->SetCurSel(0);
  }
  else
  {
    txt.Format("%d", size);
    ((CComboBox*)GetDlgItem(IDC_DUMP_SIZE))->SetWindowText(txt);
  }
  m_status = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDumpMemoryDlg::OnBnClickedOk()
{
  CString txt;
	GetDlgItem(IDC_ADDR)->GetWindowText(txt);
	m_startAddress = ConvertCharToU32(txt);

	GetDlgItem(IDC_DUMP_SIZE)->GetWindowText(txt);
	m_dumpSize = atoi(txt);

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey(VIEWER_REG_PATH, false))
	{
		reg.WriteInt("dmtf_address", m_startAddress);
		reg.WriteInt("dmtf_size", m_dumpSize);
	}

  OnOK();
}

void CDumpMemoryDlg::DoCommand()
{

}
