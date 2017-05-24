#pragma once


// CConfig_Proprietary 

class CConfig_Proprietary : public CDialog
{
	DECLARE_DYNAMIC(CConfig_Proprietary)

public:
	CConfig_Proprietary(CWnd* pParent = NULL);   
	virtual ~CConfig_Proprietary();


	enum { IDD = IDD_CON_PROPRIETARY };

	int enable;
	int attr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
};
