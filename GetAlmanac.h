#pragma once


// CGetAlmanac 對話方塊

class CGetAlmanac : public CDialog
{
	DECLARE_DYNAMIC(CGetAlmanac)

public:
	CGetAlmanac(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CGetAlmanac();

// 對話方塊資料
	enum { IDD = IDD_GETALMANAC };

	CEdit m_filename;
	CComboBox m_sv;
	CString fileName;
	int sv;
	U08 isGlonass;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAlmanacBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
