#pragma once

#include "resource.h"

// CExternalSrecDlg 

class CExternalSrecDlg : public CDialog
{
	DECLARE_DYNAMIC(CExternalSrecDlg)

public:
	CExternalSrecDlg(CWnd* pParent = NULL);
	virtual ~CExternalSrecDlg();
	void AddMsgToList(LPCTSTR msg);

	enum { IDD = IDD_EXTERNAL_SREC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	bool DownloadLoader(CString externalSrecFile, BOOL useBinCmd = false);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedGo();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
protected:
	virtual void OnOK();
  bool m_isV6;
};
