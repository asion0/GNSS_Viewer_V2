#pragma once

// DrTestDlg
class BinaryData;

class DrTestDlg : public CDialog
{
	DECLARE_DYNAMIC(DrTestDlg)

public:
  enum WorkingState 
  {
    Idle, 
    Started,
    Querying,
  };

	DrTestDlg(CWnd* pParent = NULL);   // 
	virtual ~DrTestDlg();
	virtual BOOL OnInitDialog();

  afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

protected:
  WorkingState state;
  int queryCount;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
