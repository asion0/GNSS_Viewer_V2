#pragma once


// CDevice_Adding

class CDevice_Adding : public CDialog
{
	DECLARE_DYNAMIC(CDevice_Adding)

public:
	CDevice_Adding(CWnd* pParent = NULL);
	virtual ~CDevice_Adding();

	CString m_comport;
	int m_baudrate;

	CComboBox m_cbo_com,m_cbo_baudrate;

	enum { IDD = IDD_DEVICE_ADDING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnBnClickedOk();
	void setPort_Baudrate(CString port,int baudrate);
	virtual BOOL OnInitDialog();
};
