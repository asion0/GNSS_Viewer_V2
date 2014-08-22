#pragma once


// CConPinningParameter 對話方塊

class CConPinningParameter : public CDialog
{
	DECLARE_DYNAMIC(CConPinningParameter)

public:
	CConPinningParameter(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConPinningParameter();

// 對話方塊資料
	enum { IDD = IDD_CON_PINNING };


	int pin_speed;
	int pin_cnt;
	int upin_speed;
	int upin_threshold;
	int upin_distance;
	int attr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
