#pragma once

// CSoftImDwDlg 對話方塊


class CSoftImDwDlg : public CDialog
{
	DECLARE_DYNAMIC(CSoftImDwDlg)

public:
	CSoftImDwDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CSoftImDwDlg();

// 對話方塊資料
	enum { IDD = IDD_SOFTWAREIMAGEDOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg  LRESULT OnSetTimeout(WPARAM wParam, LPARAM lParam);
	afx_msg  LRESULT OnSetProgress(WPARAM wParam, LPARAM lParam);
	afx_msg  LRESULT OnSetPromptMessage(WPARAM wParam, LPARAM lParam);

public:
	void SetFinish(bool finish) { isFinish = finish; }
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CProgressCtrl m_progress;	
	bool isFinish;
	
	CEdit m_msg;
	CEdit m_percent;
//	CButton m_close;
};
