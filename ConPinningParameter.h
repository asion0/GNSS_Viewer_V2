#pragma once


// CConPinningParameter 

class CConPinningParameter : public CDialog
{
	DECLARE_DYNAMIC(CConPinningParameter)

public:
	CConPinningParameter(CWnd* pParent = NULL);
	virtual ~CConPinningParameter();

	enum { IDD = IDD_CON_PINNING };

	int pin_speed;
	int pin_cnt;
	int upin_speed;
	int upin_threshold;
	int upin_distance;
	int attr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
