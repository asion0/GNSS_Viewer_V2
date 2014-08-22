#pragma once


// CPositionFinderDlg 對話方塊

class CPositionFinderDlg : public CDialog
{
	DECLARE_DYNCREATE(CPositionFinderDlg)

public:
	CPositionFinderDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CPositionFinderDlg();

// 對話方塊資料
	enum { IDD = IDD_CONFIG_POSSET};

	D64 lat;
	D64 lon;
	int unit_sel;
	int attr;

	CEdit m_lat,m_lon;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
