#pragma once


// CConMultiMode 對話方塊

class CConMultiMode : public CDialog
{
	DECLARE_DYNAMIC(CConMultiMode)

public:
	CConMultiMode(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConMultiMode();

// 對話方塊資料
	enum { IDD = IDD_CON_MULTI_MODE };
	
	int mode;
	int attribute;

	CComboBox m_mode;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCustom();
	afx_msg void OnBnClickedBuildIn();
};
