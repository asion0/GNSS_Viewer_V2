#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CConNMEADlg 對話方塊

class CConNMEADlg : public CDialog
{
	DECLARE_DYNAMIC(CConNMEADlg)

public:
	CConNMEADlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConNMEADlg();

// 對話方塊資料
	enum { IDD = IDD_CON_NMEA_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_slgga;
	CSliderCtrl m_slgsa;
	CSliderCtrl m_slgsv;
	CSliderCtrl m_slgll;
	CSliderCtrl m_slrmc;
	CSliderCtrl m_slvtg;
	CSliderCtrl m_slzda;
	CSliderCtrl m_slgns;
	CEdit m_gga;
	CEdit m_gsa;
	CEdit m_gsv;
	CEdit m_gll;
	CEdit m_rmc;
	CEdit m_vtg;
	CEdit m_zda;
	CEdit m_gns;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl m_spingga;
	CSpinButtonCtrl m_spingsa;
	CSpinButtonCtrl m_spingsv;
	CSpinButtonCtrl m_spingll;
	CSpinButtonCtrl m_spinrmc;
	CSpinButtonCtrl m_spinvtg;
	CSpinButtonCtrl m_spinzda;
	CSpinButtonCtrl m_spingns;

	CStatic m_nmea1_zda,m_nmea2_zda,m_nmea3_zda;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBnClickedOk();
	CComboBox m_attribute;
};
