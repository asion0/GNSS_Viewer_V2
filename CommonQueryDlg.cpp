#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"
#include "CommonQueryDlg.h"

// CCommonQueryDlg 
static BinaryCommand configCmd;
static CString configPrompt;

// UINT AFX_CDECL ConfigThread(LPVOID param)
// {
	// bool restoreConnect = (((int)(param))==0);
	// CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), configPrompt, restoreConnect);
	// return 0;
// }

IMPLEMENT_DYNAMIC(CCommonQueryDlg, CDialog)

CCommonQueryDlg::CCommonQueryDlg(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	pCancelBtn = new CButton();
	pAcceptBtn = new CButton();
}

CCommonQueryDlg::~CCommonQueryDlg()
{
	delete pCancelBtn;
	delete pAcceptBtn;
}

BEGIN_MESSAGE_MAP(CCommonQueryDlg, CDialog)
END_MESSAGE_MAP()

BOOL CCommonQueryDlg::OnInitDialog()
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

INT_PTR CCommonQueryDlg::DoDirect(int type)
{
	return IDCANCEL;
}

// CQueryCustomStringIntervalDlg 
IMPLEMENT_DYNAMIC(CQueryCustomStringIntervalDlg, CCommonQueryDlg)

CQueryCustomStringIntervalDlg::CQueryCustomStringIntervalDlg(CWnd* pParent /*=NULL*/)
	: CCommonQueryDlg(IDD_QRY_NMEA_STRING_INTV, pParent)
{//IDD_CFG_NMEA_STRING_INTV
	//m_nAttribute = 0;
}

BEGIN_MESSAGE_MAP(CQueryCustomStringIntervalDlg, CCommonQueryDlg)
	ON_BN_CLICKED(IDOK, &CQueryCustomStringIntervalDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CQueryCustomStringIntervalDlg 
BOOL CQueryCustomStringIntervalDlg::OnInitDialog()
{
	CCommonQueryDlg::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CQueryCustomStringIntervalDlg::UpdateInterval()
{
  CString txt;
  CComboBox* cmb = (CComboBox*)GetDlgItem(IDC_NMEA_STR);
  if(cmb->GetCurSel() == -1)
  {
    GetDlgItem(IDC_NMEA_STR)->GetWindowText(txt);
  }
  else
  {
    cmb->GetLBText(cmb->GetCurSel(), txt);
  }
  if(txt.GetLength() != 3)
  {
    ::AfxMessageBox("NMEA string is invalid, it can only 3 uppercase letters!");
    return;
  }

	CGPSDlg::gpsDlg->QueryNmeaStringX(CGPSDlg::Display, txt, NULL);
}

void CQueryCustomStringIntervalDlg::OnBnClickedOk()
{
  UpdateInterval();
	//OnOK();
}
	
void CQueryCustomStringIntervalDlg::DoCommand()
{
  
	//BinaryData cmd(6);
	//*cmd.GetBuffer(0) = 0x69;
	//*cmd.GetBuffer(1) = 0x01;
	//*cmd.GetBuffer(2) = (U08)m_bEnable;
	//*cmd.GetBuffer(3) = HIBYTE(m_nOverdueSeconds);
	//*cmd.GetBuffer(4) = LOBYTE(m_nOverdueSeconds);
	//*cmd.GetBuffer(5) = (U08)m_nAttribute;

	//configCmd.SetData(cmd);
	//configPrompt = "Configure DGPS successfully";
 // AfxBeginThread(ConfigThread, 0);
}
