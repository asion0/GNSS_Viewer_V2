#pragma once


// CCon1PPS_DOP

class CCon1PPS_DOP : public CDialog
{
	DECLARE_DYNAMIC(CCon1PPS_DOP)

public:
	CCon1PPS_DOP(CWnd* pParent = NULL); 
	virtual ~CCon1PPS_DOP();
	
	CButton m_dop_auto,m_dop_pdop,m_dop_hdop,m_dop_gdop,m_dop_disable;
	CEdit m_pdop,m_hdop,m_gdop;
	
	CComboBox m_attr;
	int dop_sel,attr;
	float pdop,hdop,gdop;

	enum { IDD = IDD_1PPS_DOP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioDopAuto();
	afx_msg void OnBnClickedRadioDopPdop();
	afx_msg void OnBnClickedRadioDopHdop();
	afx_msg void OnBnClickedRadioDopGdop();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioDopDiable();
	afx_msg void OnBnClickedOk();
};
