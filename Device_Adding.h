#pragma once


// CDevice_Adding 對話方塊

class CDevice_Adding : public CDialog
{
	DECLARE_DYNAMIC(CDevice_Adding)

public:
	CDevice_Adding(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CDevice_Adding();

	CString m_comport;
	int m_baudrate;

	CComboBox m_cbo_com,m_cbo_baudrate;

// 對話方塊資料
	enum { IDD = IDD_DEVICE_ADDING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnBnClickedOk();
	void setPort_Baudrate(CString port,int baudrate);
	virtual BOOL OnInitDialog();
};
