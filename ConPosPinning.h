#pragma once


// CConPosPinning

class CConPosPinning : public CDialog
{
	DECLARE_DYNAMIC(CConPosPinning)

public:
	CConPosPinning(CWnd* pParent = NULL);
	virtual ~CConPosPinning();

	enum { IDD = IDD_CON_POSITION_PINNING };

	int m_position_pinning;
	int m_attributes;

	CComboBox position_pinning;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
