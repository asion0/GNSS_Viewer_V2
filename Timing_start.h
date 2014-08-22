#pragma once


#define EPH_4_FILE_NAME		"Eph_4.dat"
#define ALMANAC_FILE_NAME		"almanac.dat"

#define EPH_4_FILE_SIZE		3008

#define SATELLITES_COUNT	32
#define ALMANAC_ENTRY_SIZE		0x33
#define SATELLITE_EPH_SIZE	94

enum
{
	CMD_ACK = 0x01,
	CMD_NACK = 0x02,
	CMD_TIMEOUT = 0x09,

};
// CTiming_start 對話方塊

class CTiming_start : public CDialog
{
	DECLARE_DYNAMIC(CTiming_start)

public:
	CTiming_start(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CTiming_start();

	D64 latitude;
	D64 longitude;


	CEdit m_host;
	CEdit m_name;
	CEdit m_pwd;
	CEdit m_text;
	CProgressCtrl m_progress;

	CButton m_start;
	CButton m_exit;

// 對話方塊資料
	enum { IDD = IDD_TIMING_START };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	U08 Ftp_File(char *remote_file, const char *local_file);
	afx_msg void OnBnClickedStart();
	virtual BOOL OnInitDialog();
	U08 set_eph_4();
	U08 target_warmstart(int year,U08 mon,U08 day,U08 hour,U08 minute,U08 sec,D64 dlat,D64 dlon);
	U08 device_warmstart();
	U08 gps_set_almanac(U08 *almanac);
	U08 set_almanac();
	U08 FTP_utc();
	U08 FTP_almanac();
	U08 FTP_Eph();
};
