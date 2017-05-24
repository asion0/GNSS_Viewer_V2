#pragma once


// CConMultiMode

class CConMultiMode : public CDialog
{
	DECLARE_DYNAMIC(CConMultiMode)

public:
	CConMultiMode(CWnd* pParent = NULL);
	virtual ~CConMultiMode();

	enum { IDD = IDD_CON_MULTI_MODE };
	
	int mode;
	int attribute;

	CComboBox m_mode;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCustom();
	afx_msg void OnBnClickedBuildIn();
};
