// TcpipConnectionDlg.cpp
//

#include "stdafx.h"
#include "TcpipConnectionDlg.h"
#include "GPSDlg.h"
#include "MySockey.h"

// CTcpipConnectionDlg 
IMPLEMENT_DYNAMIC(CTcpipConnectionDlg, CDialog)

CTcpipConnectionDlg::CTcpipConnectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TCPIP_CONNECT, pParent)
{
  m_psocket = NULL;
}

CTcpipConnectionDlg::~CTcpipConnectionDlg()
{
}

void CTcpipConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTcpipConnectionDlg, CDialog)
  ON_BN_CLICKED(IDOK, &CTcpipConnectionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTcpipConnectionDlg
BOOL CTcpipConnectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

  CString txt;
  ((CComboBox*)GetDlgItem(IDC_WORK_TYPE))->SetCurSel(m_workType);
  GetDlgItem(IDC_IP_ADDR)->SetWindowText(m_deviceHost);
  txt.Format("%d", m_port);
  GetDlgItem(IDC_PORT)->SetWindowText(txt);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CTcpipConnectionDlg::OnBnClickedOk()
{
  CString txt;

  m_workType = ((CComboBox*)GetDlgItem(IDC_WORK_TYPE))->GetCurSel();
  GetDlgItem(IDC_IP_ADDR)->GetWindowText(m_deviceHost);
	GetDlgItem(IDC_PORT)->GetWindowText(txt);
  m_port = atoi(txt);

  if(m_psocket == NULL)
  {
    m_psocket = new CMySocket();
  }
  //SOCK_STREAM, SOCK_DGRAM
	m_psocket->Create();
  if(!m_psocket->Connect(m_deviceHost, m_port))
	{
	  m_psocket->Close();
    delete m_psocket;
    OnCancel();
    return;
	}

	//socket.Close();
	OnOK();
}