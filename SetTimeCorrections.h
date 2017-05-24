#pragma once


// CSetTimeCorrections 

class CSetTimeCorrections : public CDialog
{
	DECLARE_DYNAMIC(CSetTimeCorrections)

public:
	CSetTimeCorrections(CWnd* pParent = NULL); 
	virtual ~CSetTimeCorrections();

	enum { IDD = IDD_SETTIMECORR };

	CString _filename;
	int _attr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
	virtual BOOL OnInitDialog();
};
