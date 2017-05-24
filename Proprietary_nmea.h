#pragma once


// CProprietary_nmea 

class CProprietary_nmea : public CDialog
{
	DECLARE_DYNAMIC(CProprietary_nmea)

public:
	CProprietary_nmea(CWnd* pParent = NULL);   
	virtual ~CProprietary_nmea();

	enum { IDD = IDD_PROPRIETARY_NMEA };

	CComboBox m_psti_id;
	CComboBox m_psti,m_attr;

	int psti_id, psti_interval, attr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
