#pragma once

#include "resource.h"

// CConfigureRfIc
class CConfigureRfIc : public CDialog
{
	DECLARE_DYNAMIC(CConfigureRfIc)

public:
	CConfigureRfIc(CWnd* pParent = NULL);
	virtual ~CConfigureRfIc() {};
	
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedClose();

  virtual BOOL OnInitDialog();

protected:
  static UINT staticWCtrlList[Setting::RFIC_REG_SIZE];
  static UINT editWCtrlList[Setting::RFIC_REG_SIZE];
  static UINT staticRCtrlList[Setting::RFIC_REG_SIZE];
  static UINT editRCtrlList[Setting::RFIC_REG_SIZE];

	afx_msg void OnCbnSelChangeRtkMode();

	DECLARE_MESSAGE_MAP()
};
