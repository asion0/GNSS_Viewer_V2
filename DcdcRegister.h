#pragma once

#include "resource.h"

// CIoTester
class CDcdcRegister : public CDialog
{
	DECLARE_DYNAMIC(CDcdcRegister)

public:
	CDcdcRegister(CWnd* pParent = NULL);  
	virtual ~CDcdcRegister();

	enum { IDD = IDD_DCDC_REGISTER };

	virtual BOOL OnInitDialog();

protected:
  BOOL SetOneRegister(U32 addr, U32 showId, U32 checkId, U32 editId);
  void UpdateOneRegister(U32 checkId, U32 editId);
  void ConfigureOneRegister(U32 addr, U32 showId, U32 editId, U32 checkId);
  BOOL GetAllRegister();
  BOOL SwitchAutoUpdate();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
  BOOL autoUpdateMode;

	DECLARE_MESSAGE_MAP()
public:

  afx_msg void OnBnClickedCheckM1(UINT nID);
  afx_msg void OnBnClickedCheckM2(UINT nID);
  afx_msg void OnBnClickedCheckM3(UINT nID);
  afx_msg void OnBnClickedCheckM4(UINT nID);
  afx_msg void OnBnClickedCheckM5(UINT nID);
  afx_msg void OnBnClickedCheckM6(UINT nID);
  afx_msg void OnBnClickedCheckM7(UINT nID);
  afx_msg void OnBnClickedCheckMC(UINT nID);
  afx_msg void OnBnClickedCheckMV(UINT nID);
  afx_msg void OnBnClickedConfigRegBtn();
  afx_msg void OnBnClickedGetAgainBtn();
  afx_msg void OnBnClickedConfigReg2Btn();
  afx_msg void OnBnClickedAutoUpdate();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
};
