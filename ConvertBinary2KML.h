#pragma once

#include "NMEA.h"
#include ".\skytraqkml.h"
#include "GPSDlg.h"

// CConvertBinary2KML 對話方塊

class CConvertBinary2KML : public CDialog
{
	DECLARE_DYNAMIC(CConvertBinary2KML)

public:
	CConvertBinary2KML(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConvertBinary2KML();

// 對話方塊資料
	enum { IDD = IDD_NI_BINARY_KML };

	CString filepath;
	CString sPath;
	CString FileName[200];
	CString FilePath[200];

	CButton m_check[75];		

	unsigned char num;
	unsigned char NumOfFile;
	unsigned char NoOfFile;
	CEdit m_directory;
	CEdit m_alltxt;
	CButton m_selectall;
	CButton m_hselectall;

	CFile Fnmea;
	CFile Fsource;
	bool IsFSourceOpen;
	bool IsFlogOpen;
	bool inilog;

	GPGLL msg_gpgll;
	GPGSA msg_gpgsa;
	GPGGA msg_gpgga;
	GPGSV msg_gpgsv;
	GPZDA msg_gpzda;
	GPRMC msg_gprmc;
	GPVTG msg_gpvtg;
	NMEA nmea;

	CString kml_filename;
	CSkyTraqKml kml;

	FILE *nmea_file;

	double lon,lat;

	bool IsConvertFinish;


	CComboBox m_color;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBplayer();
	void GetDatFile();
	void GetAllDatFile();
	afx_msg void OnBnClickedCheck76();
	afx_msg void OnBnClickedCheck78();
	afx_msg void OnBnClickedOk();
	void convert_all_file();
	void Convert();
	int GET_NMEA_SENTENCE(CFile& file, unsigned char* buffer);

	bool BINARY_PROC(unsigned char* buffer,int len);
	void Show_NI_Timing(U08 *buff);
	void Show_NI_PVT(U08 *buff);
	void convert_gps_time_to_utc( S16 wn, D64 tow, UTC_T* utc_p );
	void WriteToFile();
};
