#pragma once
#include "afxwin.h"

// CSysRestartDlg 對話方塊

class CSysRestartDlg : public CDialog
{
	DECLARE_DYNAMIC(CSysRestartDlg)

public:
	CSysRestartDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CSysRestartDlg();

// 對話方塊資料
	enum { IDD = IDD_SYSTEM_RESET };
//	static CSysRestartDlg* pSysRestartDlg;	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_startmode;
	virtual BOOL OnInitDialog();
	CEdit m_utcyear;
	CComboBox m_utcmonth;
	CComboBox m_utcday;
	CComboBox m_utchour;
	CComboBox m_utcminute;
	CComboBox m_utcsecond;
	CEdit m_latitude;
	CEdit m_longitude;
	CEdit m_altitude;
	unsigned short int utcyear;
    signed short int lat;
    signed short int lon;
    signed short int alt;

	

	U08 ReStartType;
	void  Restart();
		
	afx_msg void OnBnClickedOk();
	bool CheckMessage();
	

	afx_msg void OnCbnCloseupSrmode();
private:
	static unsigned char      restartMode;
	static unsigned char      SRutcmonth;
	static unsigned char      SRutcday;
	static unsigned char      SRutchour;
	static unsigned char      SRutcmin;
	static unsigned char      SRutcsec;
	static unsigned short int SRutcyear;
	static signed short int   SRlat;
	static signed short int   SRlon;
	static signed short int   SRalt;
};
