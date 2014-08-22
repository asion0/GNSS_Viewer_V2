#pragma once
#include "stdafx.h"
#include "DataLog.h"
#include "NMEA.h"
#include "skytraqkml.h"

// CCompressDlg 對話方塊

class CCompressDlg : public CDialog
{
	DECLARE_DYNAMIC(CCompressDlg)

public:
	CCompressDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCompressDlg();

	CString kml_file_name;
	CSkyTraqKml1 kml;
// 對話方塊資料
	enum { IDD = IDD_DLGCOMPRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_source;
	CEdit m_datacom;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
	
	//void WriteKMLini();
	//void WriteKMLPath();	
	CFile Fsource;
	CFile Flog;
	//CFile Fnmea;

	bool IsFSourceOpen;
	bool IsFlogOpen;
	bool inilog;

	POS_FIX_REC Current;
	POS_FIX_REC Last;
	void PosFixAlgorithm();
	void FULL_DATA();
    void INC_DATA();

	FIX_FULL FixFull;
	FIX_INC  FixInc;
	FIX_NONE FixNone;
	LogFlashInfo1 LogFlashInfo;
	
	void LLA2ECEF();
	//void Deg2Rad(double& degree);
	//void Rad2Deg(double& degree);
	double h,lon,lat,X,Y,Z,N;

	char cPos[50];
	virtual BOOL OnInitDialog();
	bool NMEA_PROC(const char* buffer,int offset);

	GPGLL msg_gpgll;

    GPGSA msg_gpgsa;
	GPGSA msg_glgsa;
	GPGSA msg_bdgsa;
	GPGSA msg_gagsa;
    GPGGA msg_gpgga;

    GPGSV msg_gpgsv;
	GPGSV msg_glgsv;
    GPGSV msg_bdgsv;
	GPGSV msg_gagsv;

    GPZDA msg_gpzda;
    GPRMC msg_gprmc;
    GPVTG msg_gpvtg;
	NMEA nmea;

	int GET_NMEA_SENTENCE(CFile&, U08*);

};
