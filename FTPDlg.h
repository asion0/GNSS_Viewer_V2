#pragma once
//#include "stdafx.h"

// CFTPDlg 

class CFTPDlg : public CDialog
{
	DECLARE_DYNAMIC(CFTPDlg)

public:
	CFTPDlg(CWnd* pParent = NULL);
	virtual ~CFTPDlg();

	enum { IDD = IDD_FTPDLG };

	virtual BOOL OnInitDialog();

	void GetAgpsFile();
	void SetMode(int m);

  static bool DownloadFromFtpServer(CProgressCtrl* pProgress, CWnd* pTextWnd,
    LPCSTR pszHost, LPCSTR pszUsername, LPCSTR pszPassword, int nPort,
    LPCSTR fileName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 
	virtual void OnCancel();

	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedClose();

	bool GetFtpFile();
	void FindEphemerisFile();
	void DownloadFileFromFTP(CString& ,CFtpConnection* );
	void GetAllDatFileSrec();
	void DoRomAgps();
	void DoPhoenixAgps();
	bool UploadSrec();
	bool UploadBin();
	bool TransferFile(BinaryData& ephData);

private:
//	HANDLE  m_hClose;

	CButton m_closeBtn;
	CButton m_connectBtn;

	CProgressCtrl m_progress;
	int m_agpsMode;		//0=normal,1=srec type
	CEdit m_host;
	CEdit m_name;
	CEdit m_pwd;
	CEdit m_port;
	CEdit m_text;
	CStatic m_ftp_title;
	CString m_ephFileName;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedErase();
	afx_msg void OnStnClickedHostT();
	afx_msg void OnStnClickedPwdT();
	afx_msg void OnEnChangePwd();
};
