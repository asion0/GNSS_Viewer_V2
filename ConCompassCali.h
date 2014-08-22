#pragma once

struct COMPASS_CALI_T 
{
	int offset_x;
	int offset_y;
	int offset_z;
	int sens_x;
	int sens_y;
	int sens_z;

};
// CConCompassCali 對話方塊

class CConCompassCali : public CDialog
{
	DECLARE_DYNAMIC(CConCompassCali)

public:
	CConCompassCali(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConCompassCali();

// 對話方塊資料
	enum { IDD = IDD_CON_COMPASS };

	CEdit m_offset_x,m_offset_y,m_offset_z;
	CEdit m_sens_x,m_sens_y,m_sens_z;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCompassGet();
	afx_msg void OnBnClickedCompassSet();
};
