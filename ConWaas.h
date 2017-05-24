#pragma once


// CConWaas

class CConWaas : public CDialog
{
	DECLARE_DYNAMIC(CConWaas)

public:
	CConWaas(CWnd* pParent = NULL);
	virtual ~CConWaas();
	int waas;
	int attribute;

	enum { IDD = IDD_CON_WAAS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
