#pragma once


// CGpsdoDownload 

class CGpsdoDownload : public CDialog
{
	DECLARE_DYNAMIC(CGpsdoDownload)

public:
	CGpsdoDownload(CWnd* pParent = NULL); 
	virtual ~CGpsdoDownload();

	//enum { IDD = IDD_GPSDO_DOWNLOAD };
	CString m_strMasterPath;
	CString m_strSlavePath;
	int m_slaveSourceBaud;
	int m_slaveTargetBaud;

protected:
	void SaveSetting();
	void LoadSetting();
	void AutoDetectBaudRate(CString path, UINT uid);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowseMaster();
	afx_msg void OnBnClickedBrowseSlave();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedDownload();
};
