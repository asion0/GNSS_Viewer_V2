#pragma once


// CPositionRateDlg 

class CPositionRateDlg : public CDialog
{
	DECLARE_DYNAMIC(CPositionRateDlg)

public:
	CPositionRateDlg(CWnd* pParent = NULL); 
	virtual ~CPositionRateDlg();

	enum { IDD = IDD_CONFIG_POSITIONRATE };
	
	int rate;
	int attr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	
	CComboBox cbo_rate;
	CComboBox cbo_attr;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
