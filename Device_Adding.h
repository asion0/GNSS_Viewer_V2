#pragma once

// CDeviceAddingDlg
class CDeviceAddingDlg : public CDialog
{
	DECLARE_DYNAMIC(CDeviceAddingDlg)

public:
	CDeviceAddingDlg(LPCSTR port, int baudrate, CWnd* pParent = NULL);
	virtual ~CDeviceAddingDlg();


	//CComboBox m_cbo_com,m_cbo_baudrate;
  int GetBaudrate() { return m_baudrate; }
	enum { IDD = IDD_DEVICE_ADDING };

public:
	CString m_comport;
	int m_baudrate;

 	afx_msg void OnBnClickedOk();
	
	virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()

};
