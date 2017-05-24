#pragma once


// CRawMeasmentOutputConvertDlg

class CRawMeasmentOutputConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CRawMeasmentOutputConvertDlg)

public:
	CRawMeasmentOutputConvertDlg(CWnd* pParent = NULL);  
	virtual ~CRawMeasmentOutputConvertDlg();

	enum ConvertMode
  { 
    RawMeasment,
    HostLog,
    UbloxBinary,
  };
	LPCSTR GetFilePath() { return m_filePath; }
  void SetMode(ConvertMode cm) { mode = cm; }
protected:
	CString m_filePath;
	bool	m_convertRunning;
  ConvertMode mode;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	void UpdateConvertUI();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedGo();
	afx_msg LRESULT OnRawProgress(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};
