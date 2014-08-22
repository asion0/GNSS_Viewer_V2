#pragma once


// CRawMeasmentOutputConvertDlg 對話方塊

class CRawMeasmentOutputConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CRawMeasmentOutputConvertDlg)

public:
	CRawMeasmentOutputConvertDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CRawMeasmentOutputConvertDlg();

// 對話方塊資料
	//enum { IDD = IDD_RAW_MEAS_OUT_CONV };
	LPCSTR GetFilePath() { return m_filePath; }
protected:
	CString m_filePath;
	bool	m_convertRunning;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	void UpdateConvertUI();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedGo();
	afx_msg LRESULT OnRawProgress(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};
