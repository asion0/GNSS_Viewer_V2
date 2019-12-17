#pragma once

#include "resource.h"

// CConfigureRfIc
class CConfigureRfIc : public CDialog
{
	DECLARE_DYNAMIC(CConfigureRfIc)

public:
	CConfigureRfIc(CWnd* pParent = NULL);
	virtual ~CConfigureRfIc() {};
  
  static bool SetRfRegister(U08 addr, U32 data, bool silence);

	afx_msg void OnBnClickedSet(UINT id);
  afx_msg void OnBnClickedDefault();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedExport();

  virtual BOOL OnInitDialog();

protected:
  CString m_title;
  static UINT staticWCtrlList[Setting::RFIC_W_REG_SIZE];
  static UINT editWCtrlList[Setting::RFIC_W_REG_SIZE];
  static UINT staticRCtrlList[Setting::RFIC_R_REG_SIZE];
  static UINT editRCtrlList[Setting::RFIC_R_REG_SIZE];

  void UpdateUiSetting();

	afx_msg void OnClose();
  afx_msg void OnCbnSelChangeType();

	DECLARE_MESSAGE_MAP()
};
