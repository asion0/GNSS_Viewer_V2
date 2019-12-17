#pragma once


// PromIniGeneratorDlg 
class PromIniGeneratorDlg : public CDialog
{
	DECLARE_DYNAMIC(PromIniGeneratorDlg)

public:
	PromIniGeneratorDlg(LPCSTR pszPromPath, int t = 0, CWnd* pParent = NULL);   // 
	virtual ~PromIniGeneratorDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
  afx_msg void OnEnChangeInput();
  afx_msg void OnBnClickedEnableTag();
	afx_msg void OnBnClickedRename();
	afx_msg void OnBnClickedOk();

protected:
  CString promPath;
  int type;
  
	virtual void OnOK();
  void UpdateUiStatus();
};
