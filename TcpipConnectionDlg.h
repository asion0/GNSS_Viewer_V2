#pragma once


// CTcpipConnectionDlg 
class CMySocket;
class CTcpipConnectionDlg : public CDialog
{
	DECLARE_DYNAMIC(CTcpipConnectionDlg)

public:
	CTcpipConnectionDlg(CWnd* pParent = NULL);   
	virtual ~CTcpipConnectionDlg();

	int m_workType;
  CString m_deviceHost;
  int m_port;
//	enum { IDD = IDD_TCPIP_CONNECT };
	CMySocket* m_psocket;
protected:


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 
	virtual BOOL OnInitDialog();

  afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()
};
