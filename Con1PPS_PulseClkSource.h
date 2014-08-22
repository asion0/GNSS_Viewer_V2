#pragma once
#include "afxwin.h"


// CCon1PPS_PulseClkSource

class CCon1PPS_PulseClkSource : public CDialog
{
	DECLARE_DYNAMIC(CCon1PPS_PulseClkSource)

public:
	CCon1PPS_PulseClkSource(CWnd* pParent = NULL);
	virtual ~CCon1PPS_PulseClkSource();

	enum { IDD = IDD_CON_1PPS_PULSE_CLK_SOURCE };

	int clk_source;
	int attr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_pulse_clk_src;
	CComboBox m_attr;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
