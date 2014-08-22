#pragma once


// CConGNSSSelectionforNAV 對話方塊

class CConGNSSSelectionforNAV : public CDialog
{
	DECLARE_DYNAMIC(CConGNSSSelectionforNAV)

public:
	CConGNSSSelectionforNAV(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConGNSSSelectionforNAV();

// 對話方塊資料
	enum { IDD = IDD_CON_GNSS_SELECTION };

	int selection;
	int attr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
