#pragma once

#include "resource.h"

// CConfigureIir
class CConfigureIir : public CDialog
{
	DECLARE_DYNAMIC(CConfigureIir)

public:
	CConfigureIir(CWnd* pParent = NULL);
	virtual ~CConfigureIir() {};

	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedClose();

  virtual BOOL OnInitDialog();

protected:
  static UINT staticWCtrlList[];
  static UINT editWCtrlList[];

	afx_msg void OnCbnSelChangeRtkMode();

	DECLARE_MESSAGE_MAP()
};
