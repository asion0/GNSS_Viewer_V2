#pragma once
//#include "afxcmn.h"
//#include "afxwin.h"


// CConfigNmeaIntervalDlg

class CConfigNmeaIntervalDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigNmeaIntervalDlg)

public:
	CConfigNmeaIntervalDlg(CWnd* pParent = NULL);   
	virtual ~CConfigNmeaIntervalDlg();

#if (CUSTOMER_ID == SWID)	//SWID customize
	enum { IDD = IDD_CONFIG_NMEA_INTERVAL2_V8 };
#else
	enum { IDD = IDD_CONFIG_NMEA_INTERVAL_V8 };
#endif

protected:
	CComboBox m_attr;
	U08 m_attrib;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 
	virtual BOOL OnInitDialog();
	void AdjustValue(int nPos, CScrollBar* pScrollBar);

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()
public:
	U08 GetParam(int index);
	U08 GetAttribute()
	{
		return m_attrib;
	}

};
