#pragma once


// PromIniGeneratorDlg 
class PromIniGeneratorDlg : public CDialog
{
	DECLARE_DYNAMIC(PromIniGeneratorDlg)

public:
  enum UIType
  {
    V8,
    Phoenix,
  };
	PromIniGeneratorDlg(LPCSTR pszPromPath, UIType t = V8, CWnd* pParent = NULL);   // 
	virtual ~PromIniGeneratorDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
  afx_msg void OnEnChangeTagValue();
  afx_msg void OnEnChangeProductName();
  afx_msg void OnBnClickedEnableTag();
	afx_msg void OnBnClickedRename();
	afx_msg void OnBnClickedOk();

protected:
  CString promPath;
  UIType type;
  
	virtual void OnOK();
  void UpdateUiStatus();
};
