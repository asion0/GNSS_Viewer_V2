#pragma once


// CGpsdoDownload 對話方塊

class CGpsdoDownload : public CDialog
{
	DECLARE_DYNAMIC(CGpsdoDownload)

public:
	CGpsdoDownload(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CGpsdoDownload();

// 對話方塊資料
	//enum { IDD = IDD_GPSDO_DOWNLOAD };
	CString m_strMasterPath;
	CString m_strSlavePath;
	int m_slaveSourceBaud;
	int m_slaveTargetBaud;

protected:
	void SaveSetting();
	void LoadSetting();
	void AutoDetectBaudRate(CString path, UINT uid);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowseMaster();
	afx_msg void OnBnClickedBrowseSlave();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedDownload();
};
