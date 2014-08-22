#pragma once


// CConAntennaDetection 對話方塊

class CConAntennaDetection : public CDialog
{
	DECLARE_DYNAMIC(CConAntennaDetection)

public:
	CConAntennaDetection(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConAntennaDetection();

	U08 antenna_control;
	int attr;

	CButton m_chk_short,m_chk_antenna;
// 對話方塊資料
	enum { IDD = IDD_CON_ANTENNA_DETECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
