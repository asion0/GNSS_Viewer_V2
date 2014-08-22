#pragma once


// CConPosPinning 對話方塊

class CConPosPinning : public CDialog
{
	DECLARE_DYNAMIC(CConPosPinning)

public:
	CConPosPinning(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConPosPinning();

// 對話方塊資料
	enum { IDD = IDD_CON_POSITION_PINNING };

	int m_position_pinning;
	int m_attributes;

	CComboBox position_pinning;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
