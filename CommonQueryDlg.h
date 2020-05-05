#pragma once

// CCommonQueryDlg
class CCommonQueryDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommonQueryDlg)

public:
	CCommonQueryDlg(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CCommonQueryDlg();

	virtual void DoCommand() = 0;
	virtual INT_PTR DoDirect(int type);
	virtual BOOL OnInitDialog();
protected:
	CButton *pCancelBtn;
	CButton *pAcceptBtn;

	DECLARE_MESSAGE_MAP()
};

// CQueryCustomStringIntervalDlg
class CQueryCustomStringIntervalDlg : public CCommonQueryDlg
{
	DECLARE_DYNAMIC(CQueryCustomStringIntervalDlg)
public:
  enum { NmeaStringSize = 3 };

	CQueryCustomStringIntervalDlg(CWnd* pParent = NULL);   
	virtual ~CQueryCustomStringIntervalDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();
  
	afx_msg void OnBnClickedOk();

  void UpdateInterval();
protected:
	char m_nmeaId[NmeaStringSize];
	
	DECLARE_MESSAGE_MAP()
};
