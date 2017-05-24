#pragma once


// CGetTimeCorrection

class CGetTimeCorrection : public CDialog
{
	DECLARE_DYNAMIC(CGetTimeCorrection)

public:
	CGetTimeCorrection(CWnd* pParent = NULL);
	virtual ~CGetTimeCorrection();

	enum { IDD = IDD_GETTIMECORR };

	CString _filename;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
};
