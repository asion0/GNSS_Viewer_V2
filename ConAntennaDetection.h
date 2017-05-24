#pragma once


// CConAntennaDetection 

class CConAntennaDetection : public CDialog
{
	DECLARE_DYNAMIC(CConAntennaDetection)

public:
	CConAntennaDetection(CWnd* pParent = NULL);
	virtual ~CConAntennaDetection();

	U08 antenna_control;
	int attr;

	CButton m_chk_short,m_chk_antenna;

	enum { IDD = IDD_CON_ANTENNA_DETECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
