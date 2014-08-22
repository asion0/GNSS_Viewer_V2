#pragma once
#include "afxwin.h"


// CCon1PPS_OutputMode 對話方塊

class CCon1PPS_OutputMode : public CDialog
{
	DECLARE_DYNAMIC(CCon1PPS_OutputMode)

public:
	CCon1PPS_OutputMode(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCon1PPS_OutputMode();
	CButton m_no_output,m_output_always,m_output_gps_time,m_output_align;
	CButton m_align_to_GPS,m_align_to_UTC;

// 對話方塊資料
	enum { IDD = IDD_CON_1PPS_OUTPUT_MODE };
	int mode;
	int attr;
	int align_to;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioPpsNoOutput();
	afx_msg void OnBnClickedRadioPpsOutputAlways();
	afx_msg void OnBnClickedRadioPpsOutputHaveGpsTime();
	afx_msg void OnBnClickedRadioPpsOutputAlign();
	virtual BOOL OnInitDialog();
	CComboBox m_attr;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioPpsAlignToGps();
	afx_msg void OnBnClickedRadioPpsAlignToUtc();
};
