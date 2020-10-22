// Device_Adding.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "Device_Adding.h"

// CDevice_Adding 
IMPLEMENT_DYNAMIC(CDeviceAddingDlg, CDialog)
CDeviceAddingDlg::CDeviceAddingDlg(LPCSTR port, int baudrate, CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceAddingDlg::IDD, pParent)
{
	m_comport = port;
	m_baudrate = baudrate;
}

CDeviceAddingDlg::~CDeviceAddingDlg()
{
}

void CDeviceAddingDlg::DoDataExchange(CDataExchange* pDX)
{
	//DDX_Control(pDX,IDC_COMPORT,m_cbo_com);
	//DDX_Control(pDX,IDC_BAUDRATE,m_cbo_baudrate);
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDeviceAddingDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// CDevice_Adding 
void CDeviceAddingDlg::OnBnClickedOk()
{
	CString txt;
  GetDlgItem(IDC_BAUDRATE)->GetWindowText(txt);
	//m_cbo_baudrate.GetLBText(m_cbo_baudrate.GetCurSel(),tmp);
	m_baudrate = atoi(txt);
	OnOK();
}

BOOL CDeviceAddingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
  CComboBox* pComCombo = (CComboBox*)GetDlgItem(IDC_COMPORT);
  CComboBox* pBaudCombo = (CComboBox*)GetDlgItem(IDC_BAUDRATE);

	pComCombo->EnableWindow(FALSE);
  pComCombo->ResetContent();
  pComCombo->AddString(m_comport);
  pComCombo->SetCurSel(0);

  pBaudCombo->ResetContent();
  g_setting.InitBaudrateCombo(pBaudCombo);
  pBaudCombo->SetCurSel(g_setting.BaudrateIndex(m_baudrate));

	return TRUE;  // return TRUE unless you set the focus to a control
}
