#pragma once


// CGetAlmanac

class CGetAlmanac : public CDialog
{
	DECLARE_DYNAMIC(CGetAlmanac)

public:
	CGetAlmanac(CWnd* pParent = NULL);   //
	virtual ~CGetAlmanac();

// 
	enum { IDD = IDD_GETALMANAC };

	CEdit m_filename;
	CComboBox m_sv;
	CString fileName;
	int sv;
	U08 isGlonass;
	U08 isDecode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAlmanacBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
