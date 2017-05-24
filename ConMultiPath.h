#pragma once


// CConMultiPath 

class CConMultiPath : public CDialog
{
	DECLARE_DYNAMIC(CConMultiPath)

public:
	CConMultiPath(CWnd* pParent = NULL);
	virtual ~CConMultiPath();

	int multipath;
	int attribute;

	enum { IDD = IDD_CON_MULTIPATH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
};
