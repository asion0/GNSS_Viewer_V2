#pragma once
#include "NMEA.h"
#include "SkyTraqKml.h"
#include "afxcmn.h"

// CKmlDlg 對話方塊
class CKmlDlg : public CDialog
{
	DECLARE_DYNAMIC(CKmlDlg)
public:
	CKmlDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CKmlDlg();
	
	static CKmlDlg* kmlDlg;
// 對話方塊資料
	enum { IDD = IDD_PLAYER_DIALOG };
	enum { NumOfCheckBox = 75};

//Interface
	void SetConvertFinish(bool b) { isConvertFinish = b; }
	bool IsConvertFinish() { return isConvertFinish; }

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheck76();

protected:
	bool isConvertFinish;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	virtual BOOL OnInitDialog();	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedBplayer();
	afx_msg void OnBnClickedCheck78();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT CKmlDlg::OnProgress(WPARAM wParam, LPARAM lParam);
public:
	CString filepath;
	CString sPath;
	CString FileName[200];
	CString FilePath[200];
	void GetDatFile();
	CButton m_check[NumOfCheckBox];		
	void GetAllDatFile();	
	unsigned char num;
	unsigned char NumOfFile;
	unsigned char NoOfFile;
	CEdit m_directory;
	CEdit m_alltxt;
	CButton m_selectall;
	CButton m_hselectall;
	//void WriteKMLini();
	//void WriteKMLPath();
	CFile Fnmea;
	bool IsFlogOpen;
	U08 NMEA_PROC(const char*,int);

	//GPGLL msg_gpgll;
    GPGSA msg_gpgsa, msg_glgsa, msg_bdgsa, msg_gagsa;
    GPGGA msg_gpgga;
    GPGSV msg_gpgsv, msg_glgsv, msg_bdgsv, msg_gagsv;
    //GPZDA msg_gpzda;
    GPRMC msg_gprmc;
    //GPVTG msg_gpvtg;
	NMEA nmea;

	CString kml_filename;
	CSkyTraqKml kml;

	double lon, lat;
	bool WriteToFile(U08 type);
	void Convert(CFile& f);

	int GET_NMEA_SENTENCE(CFile&, U08*);
	CComboBox m_color;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void convert_all_file();
protected:
	enum UsingType
	{
		Unknown,
		UsingGGA,
		UsingRMC
	};
	UsingType ut;
//	bool is_Fixed(U16 gga_indicator);
	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_progress;
};
