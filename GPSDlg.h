// GPSDlg.h : header file
//
#pragma once

#include "resource.h"
#include "Global.h"
#include "DataLog.h"
#include "NMEA.h"
#include "ScanDlg.h"
#include "Savenmea.h"
#include "Agps_config.h"
#include "FTPDlg.h"
#include "Registry.h"
#include "ClipboardListBox.h"
#include "Label.h"
#include "ColorStatic.h"
#include "MsgList.h"
#include "SerialAgents.h"

#define UWM_SETPROGRESS			  (WM_USER + 0x100)
#define UWM_SETPROMPT_MSG		  (WM_USER + 0x101)
#define UWM_SETTIMEOUT			  (WM_USER + 0x102)
#define UWM_KERNEL_REBOOT		  (WM_USER + 0x103)
#define UWM_FIRST_NMEA			  (WM_USER + 0x104)
#define UWM_SHOW_TIME			    (WM_USER + 0x105)
#define UWM_UPDATE_UI			    (WM_USER + 0x106)
#define UWM_SHOW_RMC_TIME		  (WM_USER + 0x107)
#define UWM_GPSDO_HI_DOWNLOAD (WM_USER + 0x108)
#define UWM_UPDATE_RTK_INFO		(WM_USER + 0x109)
#define UWM_UPDATE_PSTI030		(WM_USER + 0x10A)
#define UWM_UPDATE_PSTI031		(WM_USER + 0x10B)
#define UWM_UPDATE_PSTI032		(WM_USER + 0x10C)
#define UWM_DO_ZENLANE_CMD    (WM_USER + 0x10D)
#define UWM_TEST_XN112_START  (WM_USER + 0x10E)
#define UWM_UPDATE_PSTI033		(WM_USER + 0x10F)
#define UWM_UPDATE_THS		    (WM_USER + 0x110)

#define GNSS_CHANEL_LIMIT	16

enum DownloadErrocCode
{
	RETURN_NO_ERROR = 0,
	RETURN_RETRY,
	RETURN_ERROR
};

struct GNSS_SATE_T
{
	S08 k_num;
	U08 slot_num;
	U08 snr;
};

struct GNSS_T
{
	U08 gnss_in_view;
	GNSS_SATE_T sate[GNSS_CHANEL_LIMIT];
};
//
//typedef struct {
//	U08 Timing_mode;
//	U32 Survey_Length;
//	U08 RT_Timing_mode;
//	U32 RT_Survey_Length;
//	D64 latitude;
//	D64 longitude;
//	F32 altitude;
//	D64 RT_latitude;
//	D64 RT_longitude;
//	F32 RT_altitude;
//	U08 attributes;
//	U32 Standard_deviation;
//} _1PPS_Timing_T;

struct LL2 {
	double lat;
	double lon;
	double speed;
	double alt;
	UtcTime utc;
};

struct PSTI030_Data
{
	F32 rtkAge;
	F32 rtkRatio;
};

struct PSTI031_Data
{
	F32 baseline;
};

struct PSTI032_Data
{
	F32 eastProjection;
	F32 northProjection;
	F32 upProjection;
	F32 baselineLength;
	F32 baselineCourse;
};

struct PSTI033_Data
{
	U08 version;
	U08 receiver;
	U16 numCycleSlippedTotal;
};
// Copy from HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\DeviceClasses
static const GUID GUID_DEVINTERFACE_LIST[] = 
{
	// GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },

	// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xB6BF, 0x11D0, { 0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B } },

	// GUID_DEVINTERFACE_HID, 
	{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },

	// GUID_NDIS_LAN_CLASS
	{ 0xAD498944, 0x762F, 0x11D0, { 0x8D, 0xCB, 0x00, 0xC0, 0x4F, 0xC3, 0x35, 0x8C } }

	//// GUID_DEVINTERFACE_COMPORT
	//{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },

	//// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
	//{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },

	//// GUID_DEVINTERFACE_PARALLEL
	//{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },

	//// GUID_DEVINTERFACE_PARCLASS
	//{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } }
};

enum { 
	SHOW_STATUS_TIMER = 1,
	TEST_KERNEL_TIMER,
	ECOM_CALIB_TIMER,
	DELAY_QUERY_TIMER,
	DELAY_PLUGIN_TIMER,
  ZENLANE_INIT_TIMER,
  ZENLANE_QUERY_TIMER,
  XN116_TESTER_TIMER,
  AUTO_GET_CLOCK_OFFSET_TIMER,
};

enum { 
  DO_NOTHING = 0,
  DO_QUERY_VERSION,
  DO_ZENLANE_INIT,
  DO_ZENLANE_QUERY,
};

class CSoftImDwDlg;
class CSerial;
class CSnrBarChartGps;
class CSnrBarChartGlonass;
class CSnrBarChartDual;
class CSnrBarChartMulti;
class CSnrBarChartL2;
class CSnrBarChartDualL2;
class CSnrBarChartBeidou;
class CSnrBarChartGalileo;
class CSnrBarChartNavic;
class CSnrBarChart;
class CPic_Scatter;
class CPic_Earth;
class CCigRgsDlg;
class CGetRgsDlg;
class BinaryCommand;
class BinaryData;
class CPanelBackground;
class CCommonConfigDlg;
class CCommonQueryDlg;

class CGPSDlg : public CDialog
{
public:
	static CGPSDlg* gpsDlg;

	CGPSDlg(CWnd* pParent = NULL);	// standard constructor
	~CGPSDlg();

public:
	enum BoostMode
	{
		ChangeToSram = 0,
		ChangeToFlashAndSram = 1,
		ChangeToTemp = 2,
	};

	enum MsgMode
	{
		NoOutputMode = 0,
		BinaryMessageMode,
		NmeaMessageMode,
	};

	enum CmdErrorCode {
		Ack = 0,
		Ack0,
		NACK,
		FormatError,
		Timeout,
		Ok,
		End,
		Error,
	};

	enum CmdExeMode {
		Display = 0,
		Return,
		NoWait,
	};

	enum DownloadMode {
    DownloadMode_Auto,
    DownloadMode_FileSrec,

		EnternalLoader,
		EnternalLoaderInBinCmd,
		InternalLoaderV6Gps,
		InternalLoaderV6Gnss,
		InternalLoaderV6Gg12a,
		InternalLoaderV8,
		InternalLoaderV6GpsAddTag,
		InternalLoaderV6GpsDelTag,
		InternalLoaderV6GnssAddTag,
		InternalLoaderV6GnssDelTag,
		InternalLoaderV8AddTag,
    InternalLoaderV8AddTagInBinCmd,
    InternalLoaderV9AddTagInBinCmd,
		CustomerDownload,
		InternalLoaderSpecial,
    ForceInternalLoaderV8,
		GpsdoMasterSlave,

		HostBasedDownload,
		HostBasedCmdOnly,
		HostBasedBinOnly,
		ParallelDownloadType0,
		ParallelDownloadType1,
		RomExternalDownload,
		CustomerUpgrade,
    FileLoader,
    FileLoaderInBinCmd,
	} m_DownloadMode;

  BOOL m_useLzmaDownload;
  BOOL m_v9NewDownloadCmd;
	int m_nDownloadBaudIdx;
	int m_nDownloadBufferIdx;
	UINT m_nDownloadResource;
	CString m_strDownloadImage;
	CString m_strDownloadImage2;

  enum DownloadCmdType {
    DirectlyDownload0x0B,
    DirectlyDownload0x644E,
    ExternalLoader0x641B,
    ExternalLoader0x644F,
  };

	enum InfoTabStat {
		BasicInfo = 0,
		RtkInfo,
		RtkInfo2,
	};

	static UINT UWM_PLAYNMEA_EVENT;
	static UINT UWM_SAVENMEA_EVENT;
	static UINT UWM_UPDATE_EVENT;
#if (SPECIAL_TEST)
	U08* specCmd;
	U32	 specSize;
#endif

#if MORE_ENU_SCALE
	enum { DefauleEnuScale = 5 };
#else
	enum { DefauleEnuScale = 0 };
#endif

protected:
	enum { IDD = IDD_GPS_DIALOG };
	InfoTabStat m_InfoTabStat;
	HICON m_hIcon;
	CString m_lastGpEphFile;
	CString m_lastGlEphFile;
	CString m_lastBdEphFile;
	CString m_lastGiEphFile;
	CString m_lastGpQzEphFile;
	CString m_lastQzEphFile;
	CString m_lastAgpsEphFile;

	CColorStatic m_ttff;
	CColorStatic m_date;
	CColorStatic m_time;
	CColorStatic m_bootStatus;
	CColorStatic m_swKernel;
	CColorStatic m_swRev;
	CColorStatic m_longitude;
	CColorStatic m_latitude;
	CColorStatic m_altitude;
	CColorStatic m_direction;
	CColorStatic m_speed;
	CColorStatic m_hdop;	
	CColorStatic m_lbl_firmware_path;
	CColorStatic m_rtkAge;	
	CColorStatic m_rtkRatio;

	CBitmapButton m_CoorSwitch1Btn;
	CBitmapButton m_CoorSwitch2Btn;
	CBitmapButton m_AltitudeSwitchBtn;

#if(_TAB_LAYOUT_)
	//CColorStatic m_date2;	
	CColorStatic m_cycleSlip;	
	CColorStatic m_trueHeading;	
	CColorStatic m_time2;	
	CColorStatic m_eastProjection;	
	CColorStatic m_baselineLength;	
	CColorStatic m_northProjection;	
	CColorStatic m_baselineCourse;	
	CColorStatic m_upProjection;
#endif

	PSTI030_Data m_psti030;
	PSTI031_Data m_psti031;
	PSTI032_Data m_psti032;
	PSTI033_Data m_psti033R;
	F32 m_fTrueHeading;

	CComboBox m_ComPortCombo;
	CComboBox m_BaudRateCombo;	
	CComboBox m_coordinate;
	CComboBox m_scale;
	CComboBox m_mapscale;

	CPanelBackground* m_infoPanel;
	CPanelBackground* m_earthPanel;
	CPanelBackground* m_scatterPanel;
	CPanelBackground* m_downloadPanel;

	CEdit m_twodrms;
	CEdit m_cep;
	CColorStatic m_twodrms2;
	CColorStatic m_cep2;
	CEdit m_clock_offset;
	CEdit m_noise;
	CColorStatic m_centerAlt;

	CBitmapButton m_ConnectBtn;
	CBitmapButton m_PlayBtn;
	CBitmapButton m_StopBtn;
	CBitmapButton m_RecordBtn;
	CColorStatic m_fwCrc1;
	CColorStatic m_fwCrc2;
  CColorStatic m_fwCrc3;
	CColorStatic m_fwCheckSum;

public:
	CBitmapButton m_CloseBtn;
	CString m_nmeaPlayFilePath;
	HANDLE m_nmeaPlayThread;

	int m_nmeaPlayInterval;
	bool m_nmeaPlayPause;

	CCriticalSection _nmeaPlayInterval;
	CCriticalSection csSatelliteStruct;

#if defined(SAINTMAX_UI)
	CButton m_nmea0183msg;
#endif

	Satellites satecopy_gp;
	Satellites sate_gp;	
	Satellites satecopy_gl;
	Satellites sate_gl;	
	Satellites satecopy_bd;
	Satellites sate_bd;	
	Satellites satecopy_ga;
	Satellites sate_ga;	
	Satellites satecopy_gi;
	Satellites sate_gi;	

  BOOL NeedUpdate();
	PSTI033_Data m_psti033B;

protected:
  DWORD m_mouseMouingTick;
  BOOL  m_mouseNoMoving;

  CBitmapButton m_SetOriginBtn;	
	CBitmapButton m_ClearBtn;		
	CBitmapButton m_DownloadBtn;
	CBitmapButton m_EarthSettingBtn;
	CBitmapButton m_ScatterSettingBtn;

	CStatic m_connectT;
	CListCtrl m_kNumList;
	CToolTipCtrl m_tip;
	CClipboardListBox m_responseList;	

	char m_currentDir[MyMaxPath];
	//bool m_gpsdoInProgress;
	CFile m_convertFile;
	CString m_nmeaFilePath;
	GNSS_T m_gnss;
	GNSS_T m_gnssTemp;
	bool m_isFlogOpen;
	int maplondeg, maplonmin, maplonsec, maplatdeg, maplatmin, maplatsec;
	CCriticalSection _save_nmea_cs;

  bool QuitIqPlot();
	void UpdateCooridate();
	void DisplayComportError(int com, DWORD errorCode);
	bool NmeaInput();
  int GetSelectComNumber();
	bool ComPortInput();
	void ClearInformation(bool onlyQueryInfo = false);
	bool DoDownload(int dlBaudIdx);
	bool DoDownload(int dlBaudIdx, UINT rid);
public:
	static CFont m_textFont;
	static CFont m_infoFontS;
	static CFont m_infoFontM;
	static CFont m_infoFontL;
	static CFont comboFont;;	
	static CFont messageFont;

	CMsgList m_nmeaList;
	CFile m_ephmsFile;
  CString m_sEphFileName;

	CFile m_nmeaFile;
	U32 m_nmeaFileSize;

	CSaveNmea* m_saveNmeaDlg;
	CPlayNmea* m_playNmea;
	CScanDlg* m_pScanDlg;
	CSerial* m_serial;
	CSoftImDwDlg* m_psoftImgDlDlg;

	GPGGA m_gpggaMsgBk;
	GPGLL m_gpgllMsg, m_gpgllMsgCopy, m_gpgllMsgCopy1;
	GPGSA m_glgsaMsg, m_glgsaMsgCopy, m_glgsaMsgCopy1;
	GPGSV m_glgsvMsg, m_glgsvMsgCopy, m_glgsvMsgCopy1;
	GPGGA m_gpggaMsg, m_gpggaMsgCopy, m_gpggaMsgCopy1;
	GPGSA m_gpgsaMsg, m_gpgsaMsgCopy, m_gpgsaMsgCopy1;

	GPGSV m_gpgsvMsg, m_gpgsvMsgCopy, m_gpgsvMsgCopy1;
	GPRMC m_gprmcMsg, m_gprmcMsgCopy, m_gprmcMsgCopy1;
	GPVTG m_gpvtgMsg, m_gpvtgMsgCopy, m_gpvtgMsgCopy1;

#if(MODULE_SUP_800)
	PSTI004001 m_psti004001, m_psti004001Copy, m_psti004001Copy1;
#endif

	GPZDA m_gpzdaMsg, m_gpzdaMsgCopy, m_gpzdaMsgCopy1;
	//for Beidou
	GPGSA m_bdgsaMsg, m_bdgsaMsgCopy, m_bdgsaMsgCopy1;
	GPGSV m_bdgsvMsg, m_bdgsvMsgCopy, m_bdgsvMsgCopy1;

	//for Galileo
	GPGSA m_gagsaMsg, m_gagsaMsgCopy, m_gagsaMsgCopy1;
	GPGSV m_gagsvMsg, m_gagsvMsgCopy, m_gagsvMsgCopy1;

	//for Navic
	GPGSA m_gigsaMsg, m_gigsaMsgCopy, m_gigsaMsgCopy1;
	GPGSV m_gigsvMsg, m_gigsvMsgCopy, m_gigsvMsgCopy1;

	LogFlashInfo1 m_logFlashInfo;
	U32 m_ttffCount;	
	bool m_initTtff;
	bool m_setTtff;
	U32  m_regAddress;	
	bool m_isPressCloseButton;
	bool m_isConnectOn;		
	bool m_isNmeaUpdated;

	enum { NmeaBufferSize = 1024 };
	char m_nmeaBuffer[NmeaBufferSize];		//NMEA_MSG[1024]
	static U08 m_inputMsg[200];

	void SetTTFF(int t)
	{
		CString str;
		str.Format("%d", t);
		GetDlgItem(IDC_TTFF)->SetWindowText(str);
	}

	CmdErrorCode GetCommandReturnType(U08* buff, int tail, bool showMsg = true);
	//CmdErrorCode GetCommandReturnTypeWithoutMsg(U08* buff, int tail, bool showMsg = true);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//bool IsEphmsEmpty(const BYTE* buffer, int size, int throughold);
	bool CeheckOrigin(CString,int);	
	bool CfgPortSendToTarget(U08*,U16,char*);
	bool CheckConnect();
	bool CheckGPS(U08*,U16,char*);
	bool CheckTimeOut(DWORD duration, DWORD timeOut = 10000,  bool silent = false);
	bool CloseOpenUart();
	bool ListSoftVersion(unsigned char* ,int);
	bool NmeaProc(const char*, int, NmeaType&);
	void NmeaOutput(LPCSTR pt, int len);

	bool SendMsg();
	//bool SendToTarget(U08* ,U16 ,const char*, bool quick = false);
  //bool SendToTargetOld(U08* message, U16 length, const char* Msg, bool quick);
  bool SendToTarget(U08* message, U16 length, const char* Msg, int timeout);
  bool SendToTargetAndCheckAck(U08* message, U16 length, const char* Msg, int timeout, int idLen);

	bool SendToTargetNoAck(U08*,U16);
	bool SendToTargetNoWait(U08*,U16,LPCSTR);
	bool TIMEOUT_METHOD(time_t,time_t);
	U16 RtcmProc(unsigned char*,int);
	U16 UbloxProc(U08* buffer, int len, CFile* fo = NULL);
	U08 BinaryProc(U08* buffer, int len, CFile* fo = NULL);
	void CopyNmeaToUse();
	void ClearQue();
	void Copy_NMEA_Memery(bool needUpdate = true);
	void CreateGPSThread();	
	void DataLogDecompress(bool mode = true);
	void DeleteNmeaMemery();
	void GetLogStatus(U08*);
	//void GetRegister(U08*);	
	//void LogConfigure();
	//void MSG_PROC();
  void DoFlag();
  void ParsingMessage(BOOL isPlayer = FALSE);
	void QueryMsg(unsigned char*);
	void Restart(U08* messages, BOOL restoreConnection = TRUE);
	void ScanGPS();
	void ScanGPS1();
	void ScanGPS2();
	void ScatterPlot(CDC *dc);
	//void SetEphms(U08 continues);
	bool SetPort(U08, int mode);
  CmdErrorCode SetPort0Baud(U08 baudIdx, U08 mode);
#if(MODULE_SUP_800)
	void ShowPsti004001();
#endif
	void ShowBootStatus();
	void ShowDirection(bool reset = false);	
	void ShowAltitude(bool reset = false);
	void ShowPDOP(bool reset = false);
	void ShowSpeed(bool reset = false);

	void ShowDate();
	void ShowKNumber();
	void ShowVersion();
  void DisplayCycleSliped();

	void ShowEarth(CDC *dc);
	void ShowLongitudeLatitude(void);
  bool CheckTimeContinuity(const CString& txt, int lastH, int lastM, int lastS, int h, int m, int s);
	//void DisplayTime(int h, int m, int s);
	void DisplayTime(int h, int m, D64 s);
	void DisplayDate(int y, int m, int d);

	void DisplayLongitude(LPCWSTR txt);
	void DisplayLatitude(LPCWSTR txt);
	void DisplayLongitude(D64 lon, U08 c);
	void DisplayLatitude(D64 lat, U08 c);
	//void DisplayLongitude(int h, int m, double s, char c);
	//void DisplayLatitude(int h, int m, double s, char c);

	void DisplaySpeed(D64 speed);
	void DisplayDirection(D64 direction);
	void DisplayStatus(GnssData::QualityMode m);
	void DisplayAltitude(D64 alt);
	void DisplayHdop(D64 hdop);

	void ShowRMCTime();
	void ShowStatus();
	void ShowTime(void);
	void Terminate(void);
	void TerminateGPSThread();
	void SetFacMsg(unsigned char*);	
	void continue_write_nmea();
protected:
  void InitDownloadBaudRate();
  void Initialization();

private:
	HANDLE handle_version;
	HANDLE wait_version_complete;
	U08 Soft_Version;
	void LoadMenu();

	CDC bk_dc;
	CDC bar_dc;
	CDC earth_dc;
	CDC chart_dc;

	CButton m_rom_mode;
	int m_noisePower;

	CStatic m_lbl_download;
	CButton m_btn_browse;

public:
  void SetFreqText(UINT id, int sig1, int sig2, int sig3, int sig4);
	int SetMessage(U08* msg, int len);
	int SetMessage2(U08* dst, U08* src, int srcLen);
	int GetCoordinateSel() { return m_coordinate.GetCurSel(); }
	int GetScaleSel() { return m_scale.GetCurSel(); }
	int GetMapScaleSel() { return m_mapscale.GetCurSel(); }
	bool IsFixed() { return ::IsFixed(m_gpggaMsg.GPSQualityIndicator) || ::IsFixed(m_gprmcMsg.ModeIndicator); }
	void SetNmeaUpdated(bool b);
	bool SetFirstDataIn(bool b);
	void SendRestartCommand(int mode);		
	void target_only_restart(int mode);
	bool TIMEOUT_METHOD_QUICK(time_t start,time_t end);
	void WaitEvent();
	void add_msgtolist(LPCTSTR msg);
	bool SendToTargetBatch(U08* message,U16 length,char* Msg);

	void CLEAR_NMEA_TO_USE();
	int datalog_read_offset_ctl( int start_id,int total_sector,int offset, U08 *buff,long size ,long *receive_count);
	int query_log_boundary(U16 *end,U16 *total);
	void verify_log_format(U08 *datalog,long *size);
	int datalog_read_all( int start_id,int offset, U08 *datalog,long size,long *receive_count );
	bool verify_read_buff(U08 *buff,U08 *datalog,U08 *ptr_last,int size,long *receive_count);
	bool download_eph();
	D64 calculate_tk_double( S16 ref_wn, S32 ref_tow, S16 wn, D64 tow );
	void getBuffWnToc(U08* ephptr,U16 *wn,S32 *toc);
	void get_wn_tow(S16* wn,D64* tow);
	U08 CheckEphAndDownload();
	void ShowColdStartEarth();

	CButton m_bnt_warmstart;
	CButton m_btn_coldstart;

	CButton m_agps_enable;
	CButton m_agps_disable;

	double warmstart_latitude;
	double warmstart_longitude;
	double warmstart_altitude;
	void ConfigBaudrate(int baud, int attribute);
	void WaitReady();
	void Show_Noise();
	void GetAlmanac();
	void close_minitor_1pps_window();
	//=============================================
	void GetGpsAlmanac(CString m_almanac_filename,U08 sv,U08 continues);
	void SetGpsAlmanac(U08 continues);

	void GetAlmanac_tmp();
	//void activate_minihomer();
	//void set_minihomerkey(U08* key,int len);
	//void set_minihomerid(U08* id,int len);
	void Close_Open_Port(WPARAM wParam,CString port_name);
	//U08 MinihomerQuerytag();
	//void MinihomerSettagecco();
	void query_dr_info();
	void Create_earth_pic(CDC *dc);
	void clear_login_password();
	U08 Base_Rom();
	void ClearGlonass();
	char* BootloaderRom_Combination(CString prom_path,int *buff_size);
#ifdef GG12A
	bool check_gg12a_format(const char *file_path);
#endif
	void SetGlonassAlmanac(U08 continues);
	void GetGlonassAlmanac(CString m_almanac_filename,U08 sv,U08 continues);
	void SetBeidouAlmanac(U08 continues);
	void GetBeidouAlmanac(CString m_almanac_filename,U08 sv,U08 continues);

	void SetBeidouEphms(U08 continues);	
  void SetNavicEphms(U08 continues);	
	void SetGlonassEphms(U08 continues);
  void SetGpsQzssEphms(U08 continues);	
  void SetAgpsEphms(LPCSTR pszFilename, bool restoreConnection, bool continuously = false);	
	//bool IsGlonassEphmsEmpty(BYTE* buffer);

	void GetTimeCorrection(CString m_filename);
	void SetTimeCorrection(CString m_filename);
	U08 parse_psti_others(const char *buff, int psti_id);

	//For Common Download Clasases
public:


  U32 tagPos;
  bool emptyTag;
  bool uniqueTag;

	int m_nSlaveSourceBaud;
	int m_masterFwBaudIdx;
  BOOL downloadAddTag;
  U32 tagAddress;
  U32 tagValue;
  
  enum DataLogCommandSet
  {
    DataLogCommandSet1,
    DataLogCommandSet2
  };
  
	bool CheckTagType();
	bool RealDownload(bool restoreConnection = true, bool boostBaudRate = true);
	bool Download();
	bool Download2();
	bool DownloadMasterSlave();
  bool NewDownload(DownloadMode dm, int baufIdx, const CString& image);
  bool NewRealDownload(DownloadCmdType type, DownloadMode dm, int baufIdx, const CString& image);

	void SetBaudrate(int b);
	BOOL GetShowBinaryCmdData() { return m_bShowBinaryCmdData; }
	bool BoostBaudrate(BOOL bRestore, BoostMode mode = ChangeToTemp, bool isForce = false);
	void TempBoostBaudrate(BOOL bRestore, int boostBaudIndex = 5, int timeout = 1000);
	
	void SetInputMode(MsgMode i) { m_inputMode = i; }
	void SetMode() { m_inputMode = GetMsgType(); }
	MsgMode GetMsgType() { return m_msgType; };
	void SetMsgType(MsgMode m) 
	{ 
		if(m_msgType != m)
		{
			DeleteNmeaMemery();	
			ClearInformation();
			m_msgType = m; 
		}
	};
	U08 GetRestartMode() { return m_restartMode; }
	bool ExecuteConfigureCommand(U08 *cmd, int size, LPCSTR msg, bool restoreConnect = true);
	void LogReadBatchControl(DataLogCommandSet cmdSet);
	void GetEphms(U08 SV, U08 continues = FALSE);	
	void GetGlonassEphms(U08 SV, U08 continues = FALSE);
	void GetBeidouEphms(U08 SV, U08 continues = FALSE);
	void GetNavicEphms(U08 SV, U08 continues = FALSE);
	void GetGpsQzssEphms(U08 SV, U08 continues = FALSE);
	void GetQzssEphms(U08 SV, U08 continues = FALSE);
  void GetGenericEphemeris(U08 sv, U08 continues, LPCSTR pszType, CString& lastEphFile,
    U08 cmdId, U08 cmdSubId, U08 revId, U08 revSubId, U08 setId, U08 setSubId);

	bool SaveEphemeris(U08* buff,U08 id);
	bool SaveEphemeris2(U08* buff, WORD id);
	void Refresh_EarthChart(CDC *earth_dc);
	int GetCustomerID()
	{ return m_customerID; }
	NMEA nmea;
	bool DownloadLoader2(BOOL useBinCmd, BOOL needSleep, const BinaryData& srec, CWnd* notifyWnd = NULL);
	bool DownloadLoader3(DownloadCmdType type, BOOL needSleep, const BinaryData& srec, CWnd* notifyWnd = NULL);
  bool BinSizeCmd(int size, U08 ck);
  CmdErrorCode GetTextAck(CString& strAckCmd, DWORD timeout);
  bool SendFwBufferPhoenix(const BinaryData &fwFile, CWnd* notifyWnd);
private:
  CString m_lastErrorMsg;
	BOOL m_bShowBinaryCmdData;
	int downloadTotalSize;
	int downloadProgress;
	MsgMode m_msgType;
	U08 m_restartMode;
	MsgMode m_inputMode;
	CString datalogFilename;
	CFile dataLogFile;

	enum DataLogType
	{
		FIXNONE = 0,
		FIX_FULL = 0x04,
		FIX_COMPACT = 0x08,
		FIX_FULL_POI = 0x06,
		FIX_MULTI_HZ = 0x02,
		FIX_MULTI_HZ_POI = 0x0C,
    //DataLog with crc16
    FIX_FULL_C = 0x05,
		FIX_COMPACT_C = 0x09,
		FIX_FULL_POI_C = 0x07,
		FIX_MULTI_HZ_C = 0x03,
		FIX_MULTI_HZ_POI_C = 0x0D,
	};

  CString CGPSDlg::EncodeLzma(const CString& promPath);
	UINT GetBinFromResource(int baud);
	U08 PlRomNoAlloc2(const CString& prom_path);
	U08 PlRomNoAllocV8(const CString& prom_path);
	U08 PlRomNoAlloc7z(const CString& prom_path);
	U08 PlRomCustomerUpgrade(UINT rid);
	bool FirmwareUpdate(const CString& strFwPath);
	int SendRomBuffer3(const U08* sData, int sDataSize, BinaryData &binData, int fbinSize, 
		bool needSleep, CWnd* notifyWnd);
	int SendRomBufferCustomerUpgrade(const U08* sData, int sDataSize, BinaryData &f, int fbinSize, 
		bool needSleep, CWnd* notifyWnd);
	bool DownloadLoader(DownloadMode mode);
	UINT GetSrecFromResource(int baud);
	//bool QueryPassword();
	void GetLoaderDownloadCmd(char* msg, int size);
	//Data Log functions
	bool DatalogReadAll(DataLogCommandSet cmdSet, int start_id,int offset, U08 *datalog,long size,long *receive_count );
	bool VerifyDataLogBuffer(U08 *buff, U08 *datalog, U08 *ptr_last, int size, long *receive_count);
	bool DataLogReadOffsetCtrl(DataLogCommandSet cmdSet, int start_id, int total_sector, int offset, U08 *buff, long size , long *receive_count);
	//bool QueryDataLogBoundary(U16 *end, U16 *total);
	void VerifyDataLogFormat(U08 *datalog,long *size);
	DataLogType GetDataLogType(U16 word);
	//For Common Binary Clasases
public:
  CmdErrorCode ExcuteBinaryCommand(U08 ackId, U08 ackSubId, BinaryCommand* cmd, BinaryData* ackCmd, DWORD timeOut = g_setting.defaultTimeout, bool silent = false);
	CmdErrorCode ExcuteBinaryCommand(int cmdIdx, BinaryCommand* cmd, BinaryData* ackCmd, DWORD timeOut = g_setting.defaultTimeout, bool silent = false);
	CmdErrorCode ExcuteBinaryCommandNoWait(int cmdIdx, BinaryCommand* cmd);
	CmdErrorCode GetBinaryResponse(BinaryData* ackCmd, U08 cAck, U08 cAckSub, DWORD timeOut, bool silent, bool noWaitAck = false, int cmdSize = -1, int cmdLen = 0);
	CmdErrorCode QueryRtkMode2(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRtkSlaveBaud(CmdExeMode nMode, void* outputData);
	CmdErrorCode ClearRtkSlaveData(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRtkCpifBias(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRtkElevationAndCnrMask(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryBasePosition(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPsti(CmdExeMode nMode, void* outputData);
	//CmdErrorCode QueryPsti032(CmdExeMode nMode, void* outputData);
	//CmdErrorCode QueryPsti033(CmdExeMode nMode, void* outputData);
	//CmdErrorCode QueryPsti(CmdExeMode nMode, void* outputData);
	//CmdErrorCode QueryPsti004(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGeofenceEx(CmdExeMode nMode, void* outputData);
	CmdErrorCode ReCalcuteGlonassIfb(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryBinaryMeasurementDataOut(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRtcmMeasurementDataOut(CmdExeMode nMode, void* outputData);
  CmdErrorCode QueryRegisterx(CmdExeMode nMode, U32 addr, void* outputData);
	CmdErrorCode QueryRegister(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRegister16x(CmdExeMode nMode, U32 addr, void* outputData);
	CmdErrorCode QueryRegister16(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryConstellationCapability(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryTiming(CmdExeMode nMode, void* outputData);
	CmdErrorCode BinaryQueryClockOffset(CmdExeMode nMode, void* outputData);
#if(NAVSPARK_MINI_GPIO_QUERY)
	CmdErrorCode QueryGpioData(CmdExeMode nMode, void* outputData);
#endif
	CmdErrorCode InsdrAccelerometerSelfTest(CmdExeMode nMode, void* outputData);
	CmdErrorCode InsdrGyroscopeSelfTest(CmdExeMode nMode, void* outputData);
	CmdErrorCode InsdrAccumulateAngleStart(CmdExeMode nMode, void* outputData);
	CmdErrorCode InsdrAccumulateAngleStop(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySbasDefault(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySbas(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySbas2(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDataLogStatus(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDataLogStatus2(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDataLogStatusWatch(CmdExeMode nMode, void* outputData);
	CmdErrorCode DatalogClear(CmdExeMode nMode, void* outputData);
	CmdErrorCode DatalogClear2(CmdExeMode nMode, void* outputData);
	CmdErrorCode DatalogWatchClear(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDrRate(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDrRawRate(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDrMemsNoiseLevel(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryAdrOdometerScalingFactor(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPpsOutputMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssConstellationType(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRfIc(CmdExeMode nMode, void* outputData, U08 type);
	CmdErrorCode QueryOneRfRegister(CmdExeMode nMode, void* outputData, U08 id);
  CmdErrorCode QueryNavicMessageInterval(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySoftwareVersionSystemCode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySoftwareCrcSystemCode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySoftwareCrc32SystemCode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPhoenixSoftwareFeature(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryV9TagAddress(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryV9Tag(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySecurityTag(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySecurityTagOnly(CmdExeMode nMode, void* outputData);
  CmdErrorCode QueryNmeaStringX(CmdExeMode nMode, LPCSTR nmeaStr, void* outputData);

  U08 m_nGeofecingNo;

	//Query Functions
	int m_nDefaultTimeout;
	BOOL m_bClearPsti032;
protected:
  void ConfigV9ClockToGpio0(bool on);
  void ConfigV9ClockOut(bool on);

	typedef CmdErrorCode (CGPSDlg::*QueryFunction)(CmdExeMode, void*);
	void GenericQuery(QueryFunction pfn);

	//CmdErrorCode AgepTest(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPositionRate(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDatum(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySha1String(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryVersionExtension(CmdExeMode nMode, void* outputData);
	CmdErrorCode SendZenlandInitCmd(CmdExeMode nMode, void* outputData);
	CmdErrorCode SendZenlandQueryCmd(CmdExeMode nMode, void* outputData);

	CmdErrorCode QueryPositionPinning(CmdExeMode nMode, void* outputData);
	CmdErrorCode Query1ppsMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPowerMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryV8PowerSavingParameters(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryProprietaryMessage(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDopMask(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryElevationAndCnrMask(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryAntennaDetection(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNoisePower(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDrInfo(CmdExeMode nMode, void* outputData);
	//CmdErrorCode QueryDrHwParameter(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssSelectionForNavigationSystem(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySagps(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryQzss(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNoisePowerControl(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryInterferenceDetectControl(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNmeaBinaryOutputDestination(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryParameterSearchEngineNumber(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryAgpsStatus(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGetGpsAlmanac(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGetBeidouAlmanac(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPositionFixNavigationMask(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryChannelDoppler(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNmeaIntervalV8(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNmeaInterval2V8(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryEricssonInterval(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryClockOffset(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRefTimeSyncToGpsTime(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySearchEngineSleepCriteria(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDatumIndex(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryVeryLowSpeed(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDofunUniqueId(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryCpuBoostMode(CmdExeMode nMode, void* outputData);

	CmdErrorCode QueryUartPass(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoResetSlave(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoEnterRom(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoLeaveRom(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoEnterDownload(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoLeaveDownload(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoEnterDownloadHigh(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoEnterUart(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoLeaveUart(CmdExeMode nMode, void* outputData);
	CmdErrorCode InsdrEnterUart(CmdExeMode nMode, void* outputData);
	CmdErrorCode InsdrEnterDownload(CmdExeMode nMode, void* outputData);
	CmdErrorCode InsdrLeaveUart(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNavigationModeV8(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssBootStatus(CmdExeMode nMode, void* outputData);
	//CmdErrorCode QueryDrMultiHz(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssKnumberSlotCnr2(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssNmeaTalkId(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryCustomerID(CmdExeMode nMode, void* outputData);
	CmdErrorCode Query1ppsFreqencyOutput(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySerialNumber(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDgps(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySmoothMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryTimeStamping(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGpsTime(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySignalDisturbanceStatus(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySignalDisturbanceData(CmdExeMode nMode, void* outputData);
	CmdErrorCode ResetOdometer(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryCableDelay(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGeofenceResult(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGeofenceResultEx(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRtkMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRtkParameters(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPstmDeviceAddress(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPstnLatLonDigits(CmdExeMode nMode, void* outputData);
	CmdErrorCode EnterRtkDebugMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode BackRtkDebugMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode Query1ppsPulseWidth(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryAlphaUniqueId(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryAlphaKey(CmdExeMode nMode, void* outputData);
	CmdErrorCode TmpActiveLicense(CmdExeMode nMode, void* outputData);

	CmdErrorCode AlphaAgCalibrationUp(CmdExeMode nMode, void* outputData);
	CmdErrorCode AlphaAgCalibrationDown(CmdExeMode nMode, void* outputData);
	CmdErrorCode AlphaEcompassCalibration(CmdExeMode nMode, void* outputData);

	CmdErrorCode QueryV9UniqueId(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryV9ExtendedId(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryV9PromAesTag(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryV9ExternalAesTag(CmdExeMode nMode, void* outputData);
	CmdErrorCode ResetV9AesTag(CmdExeMode nMode, void* outputData);

protected:
  int m_nHotKeyID[2];
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
  bool WriteAddressToFile(const CString& filename, const U32 start, const U32 size);
  bool PatchRomEphemeris();
  void SetFwInfo(const CString& fwPath);
	void DoDatalogLogReadBatch(DataLogCommandSet cmdSet);
  
  afx_msg LONG OnHotKey( WPARAM  wParam, LPARAM  lParam);
  afx_msg void OnChar(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedRecord();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedClose();
	afx_msg void OnCbnCloseupCoordinate();
	afx_msg void OnCbnCloseupEnuscale();
	afx_msg void OnCbnCloseupMapscale();
	afx_msg void OnBnClickedSetOrigin();
	afx_msg void OnFileSavescatterdata();
	afx_msg void OnFileSavepath();
	afx_msg void OnFileSaveplacemark();
	afx_msg void OnBnClickedGpgga();
	afx_msg void OnBnClickedGpgll();
	afx_msg void OnBnClickedGpgsa();
	afx_msg void OnBnClickedGpvtg();
	afx_msg void OnBnClickedGpgsv();
	afx_msg void OnBnClickedGprmc();
	afx_msg void OnBnClickedGpzda();
	afx_msg void OnFileSetup();
	afx_msg void OnFileExit();
	afx_msg void OnClose();
	afx_msg void OnHelpAbout();
	afx_msg void OnDRomwrite();
	afx_msg void OnDownloadRomwrite();
	afx_msg void OnBinarySystemRestart();
	afx_msg void OnBinaryDumpData();
	afx_msg void OnConfigureSerialPort();
	//afx_msg void OnSetFactoryDefaultNoReboot();
	afx_msg void OnSetFactoryDefaultReboot();
	//afx_msg void OnBinaryConfigurenmeaoutput();
	afx_msg void OnConfigureNmeaIntervalV8();
	afx_msg void OnConfigureEricssonSentecneInterval();
	afx_msg void OnConfigureSerialNumber();
	//afx_msg void OnBinaryConfiguredatum();
	//afx_msg void OnBinaryConfiguredopmask();
	afx_msg void OnConverterDecompress();	
	afx_msg void OnCovDecopre();
	afx_msg void OnConverterCompress();
	//afx_msg void OnLoggerConvert();	
	//afx_msg void OnDatalogClearControl();
	afx_msg void OnDatalogConfigure();
	afx_msg void OnDatalogConfigure2();
	afx_msg void OnDatalogWatchLogConfigureControl();	
	afx_msg void OnSoftwareimagedownloadLoaderimage();
	afx_msg void OnSoftwareimagedownloadImageonly();
	afx_msg void OnBinaryGetrgister();
	//afx_msg void OnBinaryGetrgister16();
	afx_msg void OnBinaryConfigureregister();
	afx_msg void OnBinaryConfigureregister16();
	afx_msg void OnBinaryConfigureClockOffset();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedHotstart();
	afx_msg void OnBnClickedWarmstart();
	afx_msg void OnBnClickedColdstart();
	afx_msg void OnBnClickedNoOutput();
	afx_msg void OnBnClickedNmeaOutput();
	afx_msg void OnBnClickedBinaryOutput();
	afx_msg void OnBnClickedDownload();
	afx_msg void OnFileSaveNmea();
	afx_msg void OnFileSaveBinary();
	afx_msg void OnSaveBinaryNoParsing();
	afx_msg void OnSaveTelitNoParsing();
	afx_msg void OnVerifyFirmware();
	afx_msg void OnFilePlayNmea();
	afx_msg void OnConverterKml();
	afx_msg void OnRawMeasurementOutputConvert();
	afx_msg void OnUbloxBinaryOutputConvert();
	afx_msg void OnHosLogToNmea();
	afx_msg void OnBnClickedScanAll();
	afx_msg void OnBnClickedScanPort();
	afx_msg void OnBnClickedScanBaudrate();
	afx_msg void OnBinaryConfiguremessagetype();
	afx_msg void OnEphemerisGetephemeris();
	//afx_msg void OnEphemerisSetephemeris();
	//afx_msg void OnAgpsConfig();
	afx_msg void OnFileCleannema();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnWaasQuerywaasstatus();
	afx_msg void OnWaasWaasenable();
	afx_msg void OnWaasWaasdisable();
	afx_msg void OnBinaryConfiguremessageType();
	afx_msg void OnBinaryConfigurebinarymsginterval();
	afx_msg void OnBinaryConfigureBinaryInterval();
	afx_msg void OnBinaryPositionfinder();
	afx_msg void OnConfigurePositionUpdateRate();
	afx_msg void OnBinaryConfigDrMultiHz();
	afx_msg void OnDatalogLogReadBatch();
	afx_msg void OnDatalogLogReadBatch2();
	afx_msg void OnBinaryConfigurepositionpinning();
	afx_msg void OnBinaryConfigurepinningparameters();
	afx_msg void OnMultimodeConfiguremode();
	afx_msg void OnMultimodeQuerymode();
	afx_msg void OnBinaryConfiguresubsecregister();
	afx_msg void OnConfigGpsMeasurementMode();
	afx_msg void OnBinaryQuery1pps();
	afx_msg void OnBinaryConfigurepowermode();
	afx_msg void OnBinaryConfiguremultipath();
	//afx_msg void OnWaasWaas();
	afx_msg void OnShowGpsAlmanac();
	afx_msg void OnShowBeidouAlmanac();
	//afx_msg void OnDecodeGpsAlmanac();
	afx_msg void OnGetGpsAlmanac();
	afx_msg void OnBinaryQuerybinarymsginterval();
	afx_msg void OnBinaryResetodometer();
	afx_msg void OnConfigTiming();
	afx_msg void OnConfigTimingCableDelay();
	afx_msg void OnConfigureDopMask();
	afx_msg void OnConfigElevationAndCnrMask();
	afx_msg void OnMonitoring1Pps();
	afx_msg void OnConfigureProprietaryNmea();
	afx_msg void OnSetGpsAlmanac();

	//afx_msg void OnMinihomerActivate();
	afx_msg LRESULT OnMyDeviceChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnKernelReboot(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlayNmeaEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSaveNmeaEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFirstNmea(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowTime(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowRMCTime(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateUI(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGpsdoHiDownload(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateRtkInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatePsti030(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatePsti031(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatePsti032(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatePsti033(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDoZenlaneCmd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTestXn112Start(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateTHS(WPARAM wParam, LPARAM lParam);

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedRomMode();
	afx_msg void OnBnClickedKnumEnable();
	afx_msg void OnBnClickedKnumDisable();
	afx_msg void OnConfigNmeaOutputComPort();
	afx_msg void OnBinaryConfigurenmeatalkerid();
	afx_msg void OnGetGlonassAlmanac();
	afx_msg void OnSetGlonassAlmanac();
	afx_msg void OnGetBeidouAlmanac();
	afx_msg void OnSetBeidouAlmanac();
	//afx_msg void OnEphemerisSetgpsglonass();
	//afx_msg void OnEphemerisSetgpsglonassAlmanac();
	afx_msg void OnEphemerisGettimecorrections();
	afx_msg void OnEphemerisSettimecorrections();
	afx_msg void OnConfigProprietaryMessage();
	afx_msg void OnConfigPowerSavingParameters();
	//afx_msg void OnConfigPowerSavingParametersRom();
	afx_msg void OnBinaryConfigureantennadetection();
	afx_msg void OnConfigure1PpsOutputMode();	//new
	afx_msg void OnQuery1PpsOutputMode();
	afx_msg void OnConfigGnssDozeMode();
	afx_msg void OnBnClickedECompassCalibration();
	afx_msg void OnConfigure1PpsPulseWidth();
	//afx_msg void OnQuery1PpsPulseWidth();
	afx_msg void OnConfigGnssConstellationType();
	afx_msg void OnConfigBinaryMeasurementDataOut();
	afx_msg void OnConfigRtcmMeasurementDataOut();
	afx_msg void OnConfig1ppsFrequencyOutput();
	afx_msg void OnLineAssistance();
	//afx_msg void On1ppstimingQueryppspulseclksrc();
	afx_msg void OnBinaryConfigureSBAS();
	afx_msg void OnBinaryConfigureSBAS2();
	afx_msg void OnBinaryConfigureSAGPS();
	afx_msg void OnBinaryConfigureQZSS();
	afx_msg void OnBinaryConfigureDGPS();
	afx_msg void OnBinaryConfigureSmoothMode();
	afx_msg void OnBinaryConfigTimeStamping();
	afx_msg void OnConfigLeapSeconds();
	afx_msg void OnConfigParamSearchEngineSleepCriteria();
	afx_msg void OnConfigDatumIndex();
	afx_msg void OnConfigVeryLowSpeed();
	afx_msg void OnConfigCpuBoostMode();
	afx_msg void OnConfigDofunUniqueId();
	afx_msg void OnEraseDofunUniqueId();
	afx_msg void OnConfigureNoisePowerControl();
	afx_msg void OnConfigureInterferenceDetectControl();
	afx_msg void OnConfigNMEABinaryOutputDestination();
  afx_msg void OnConfigParameterSearchEngineNumber();
	//afx_msg void OnAgpsFtpSrec();
	afx_msg void OnAgpsTest();
	afx_msg void OnRomAgpsFtpSrec();
	afx_msg void OnRomAgpsFtpNew();
  afx_msg void OnPhoenixAgpsFtp();
	afx_msg void OnClockOffsetPredict();
	afx_msg void OnClockOffsetPredictOld();
	afx_msg void OnHostBasedDownload();
	afx_msg void OnFiremareDownload();
	afx_msg void OnConfigPositionFixNavigationMask();
	afx_msg void OnParallelDownload();
	afx_msg void OnConfigRefTimeSyncToGpsTime();
	afx_msg void OnNmeaChecksumCalculator();
	afx_msg void OnBinaryChecksumCalculator();
	afx_msg void OnBinaryCommandTester();
	afx_msg void OnPromIniGenerator();
	afx_msg void OnPromIniGenerator2();
	afx_msg void OnHostLog();
	afx_msg void OnTestExternalSrec();
	afx_msg void OnIqPlot();
  afx_msg void OnTestAlphaIo();
  afx_msg void OnTestAlphaPlusIo();
  //afx_msg void OnTmpActiveLicense();
	afx_msg void OnReadMemToFile();
	afx_msg void OnReadMemToFile2();
	afx_msg void OnReadMemToFile3();
	afx_msg void OnPatchRom20130221Ephemeris();
	afx_msg void OnIoTester();
	afx_msg void OnConfigDcdc();
	afx_msg void OnConfigRfIc();
  afx_msg void OnConfigMultiRfIc();
	afx_msg void OnConfigIir();
	afx_msg void OnConfigV9PowerSaveByRtc30();
	afx_msg void OnConfigV9PowerSaveByRtc60();
	afx_msg void OnConfigV9PowerSaveByRtc90();
	afx_msg void OnConfigV9PowerSaveByRtc120();
	afx_msg void OnConfigV9ClockToGpio0On();
	afx_msg void OnConfigV9ClockToGpio0Off();
	afx_msg void OnConfigV9ClockOutOn();
	afx_msg void OnConfigV9ClockOutOff();
	afx_msg void OnPx100ExternalBurning();

  afx_msg void OnWriteMemToFile();
	afx_msg void OnUpgradeDownload();
	afx_msg void OnPatch();
	afx_msg void OnGetGlonassEphemeris();
	//afx_msg void OnSetGlonassEphemeris();
	afx_msg void OnGetBeidouEphemeris();
	//afx_msg void OnSetBeidouEphemeris();
	afx_msg void OnGetNavicEphemeris();
	//afx_msg void OnSetNavicEphemeris();
	afx_msg void OnGetGpsQzssEphemeris();
	afx_msg void OnGetQzssEphemeris();
	//afx_msg void OnSetGpsQzssEphemeris();
	afx_msg void OnSetAgpsEphemerisFile();
	afx_msg void OnSetAgpsEphemerisFileContinuously();

  void OnGetGenericEphemeris(int t);

	afx_msg void OnSup800EraseData();
	afx_msg void OnSup800WriteData();
	afx_msg void OnSup800ReadData();
	afx_msg void OnConfigGeofence();
	afx_msg void OnConfigIoRegister();
	afx_msg void OnConfigGeofence1();
	afx_msg void OnConfigGeofence2();
	afx_msg void OnConfigGeofence3();
	afx_msg void OnConfigGeofence4();
	afx_msg void OnConfigRtkMode();
	afx_msg void OnConfigRtkMode2();
	afx_msg void OnConfigRtkSlaveBaud();
	afx_msg void OnConfigBasePosition();
	afx_msg void OnConfigRtkParameters();
  afx_msg void OnConfigRtkCpifBias();
  afx_msg void OnConfigRtkElevationAndCnrMask();

	afx_msg void OnRtkReset();
	afx_msg void OnConfigMessageOut();
	afx_msg void OnConfigSubSecRegister();
	afx_msg void OnConfigPstmDeviceAddress();
	afx_msg void OnConfigPstmLatLonDigits();
	afx_msg void OnConfigureSignalDisturbanceStatus();
	afx_msg void OnConfigureGpsUtcLeapSecondsInUtc();
	afx_msg void OnGpsdoFirmwareDownload();
	afx_msg void OnStnClickedInformationB();
	afx_msg void OnStnClickedRtkInfoB();
	afx_msg void OnStnClickedRtkInfo2B();
	afx_msg void OnBnClickedCoorSwitch();
	afx_msg void OnBnClickedAltitudeSwitch();
	afx_msg void OnConfigPsti030();
	afx_msg void OnConfigPsti032();
	afx_msg void OnConfigPsti033();
	afx_msg void OnConfigPsti063();
	afx_msg void OnConfigPsti065();
	afx_msg void OnConfigPsti067();
	afx_msg void OnConfigPsti068();
	afx_msg void OnConfigPsti070();
	afx_msg void OnConfigPsti004();
	afx_msg void OnConfigNavicMessageInterval();

	afx_msg void OnRtkOnOffGpsSv();
	afx_msg void OnRtkOnOffSbasQzssSv();
	afx_msg void OnRtkOnOffGlonassSv();
	afx_msg void OnRtkOnOffBeidouSv();
	afx_msg void OnRtkOnOffGalileoSv();
	afx_msg void OnQueryRtkReferencePosition();

	afx_msg void OnInsdrTest();
	afx_msg void OnConfigWatchTrackback();
	afx_msg void OnConfigDrRate();
	afx_msg void OnConfigDrRawRate();
	afx_msg void OnConfigDrMemsNoiseLevel();
	afx_msg void OnConfigAdrOdometerScalingFactor();

	afx_msg void OnConfigAlphaKey();
  afx_msg void OnDumpMemory();

	afx_msg void OnConfigV9AesTag();
  afx_msg void OnConfigQueryPstiInterval();
  
  afx_msg void OnConfigCustomStringInterval();
  afx_msg void OnQueryCustomStringInterval();
  afx_msg void OnQueryStringInterval(UINT id);

	//afx_msg void OnAgepTest()
	//{ GenericQuery(&CGPSDlg::AgepTest); }
	afx_msg void OnQueryPositionRate()
	{ GenericQuery(&CGPSDlg::QueryPositionRate); }
	afx_msg void OnQueryDatum()
	{ GenericQuery(&CGPSDlg::QueryDatum); }
	afx_msg void OnQuerySha1String()
	{ GenericQuery(&CGPSDlg::QuerySha1String); }
	afx_msg void OnQueryConstellationCapability()
	{ GenericQuery(&CGPSDlg::QueryConstellationCapability); }
	afx_msg void OnQueryVersionExtension()
	{ GenericQuery(&CGPSDlg::QueryVersionExtension); }
	afx_msg void OnQuerySoftwareVersionSystemCode()
	{ GenericQuery(&CGPSDlg::QuerySoftwareVersionSystemCode); }
	afx_msg void OnQuerySoftwareCrcSystemCode()	
	{ GenericQuery(&CGPSDlg::QuerySoftwareCrcSystemCode); }
	afx_msg void OnQuerySoftwareCrc32SystemCode()	
	{ GenericQuery(&CGPSDlg::QuerySoftwareCrc32SystemCode); }
	afx_msg void OnQueryPositionPinning()
	{ GenericQuery(&CGPSDlg::QueryPositionPinning); }
	afx_msg void OnQuery1ppsMode()
	{ GenericQuery(&CGPSDlg::Query1ppsMode); }
	afx_msg void OnQueryPowerMode()
	{ GenericQuery(&CGPSDlg::QueryPowerMode); }
	afx_msg void OnQueryV8PowerSavingParameters()
	{ GenericQuery(&CGPSDlg::QueryV8PowerSavingParameters); }
	afx_msg void OnQueryProprietaryMessage()
	{ GenericQuery(&CGPSDlg::QueryProprietaryMessage); }
	afx_msg void OnQueryTiming()
	{ GenericQuery(&CGPSDlg::QueryTiming); }
	afx_msg void OnQueryDopMask()
	{ GenericQuery(&CGPSDlg::QueryDopMask); }
	afx_msg void OnQueryElevationAndCnrMask()
	{ GenericQuery(&CGPSDlg::QueryElevationAndCnrMask); }
	afx_msg void OnQueryAntennaDetection()
	{ GenericQuery(&CGPSDlg::QueryAntennaDetection); }
	afx_msg void OnQueryNoisePower()
	{ GenericQuery(&CGPSDlg::QueryNoisePower); }
	afx_msg void OnQueryDrInfo()
	{ GenericQuery(&CGPSDlg::QueryDrInfo); }
	//afx_msg void OnQueryDrHwParameter()
	//{ GenericQuery(&CGPSDlg::QueryDrHwParameter); }
	afx_msg void OnQuerySbasDefault()
	{ GenericQuery(&CGPSDlg::QuerySbasDefault); }
	afx_msg void OnQuerySbas()
	{ GenericQuery(&CGPSDlg::QuerySbas); }
	afx_msg void OnQuerySbas2()
	{ GenericQuery(&CGPSDlg::QuerySbas2); }
	afx_msg void OnQuerySagps()
	{ GenericQuery(&CGPSDlg::QuerySagps); }
	afx_msg void OnQueryQzss()
	{ GenericQuery(&CGPSDlg::QueryQzss); }
	afx_msg void OnQueryNoisePowerControl()
	{ GenericQuery(&CGPSDlg::QueryNoisePowerControl); }
	afx_msg void OnQueryInterferenceDetectControl()
	{ GenericQuery(&CGPSDlg::QueryInterferenceDetectControl); }
	afx_msg void OnQueryNmeaBinaryOutputDestination()
	{ GenericQuery(&CGPSDlg::QueryNmeaBinaryOutputDestination); }
	afx_msg void OnQueryParameterSearchEngineNumber()
	{ GenericQuery(&CGPSDlg::QueryParameterSearchEngineNumber); }
	//afx_msg void OnQueryAgpsStatus()
	//{ GenericQuery(&CGPSDlg::QueryAgpsStatus); }

	afx_msg void OnQueryDataLogStatus()
	{ GenericQuery(&CGPSDlg::QueryDataLogStatus); }
	afx_msg void OnQueryDataLogStatus2()
	{ GenericQuery(&CGPSDlg::QueryDataLogStatus2); }
	afx_msg void OnQueryDataLogStatusWatch()
	{ GenericQuery(&CGPSDlg::QueryDataLogStatusWatch); }
	afx_msg void OnDatalogClear()
	{ GenericQuery(&CGPSDlg::DatalogClear); }
	afx_msg void OnDatalogClear2()
	{ GenericQuery(&CGPSDlg::DatalogClear2); }
	afx_msg void OnDatalogWatchClear()
	{ GenericQuery(&CGPSDlg::DatalogWatchClear); }
	afx_msg void OnQueryDrRate()
	{ GenericQuery(&CGPSDlg::QueryDrRate); }
	afx_msg void OnQueryDrRawRate()
	{ GenericQuery(&CGPSDlg::QueryDrRawRate); }
	afx_msg void OnQueryDrMemsNoiseLevel()
	{ GenericQuery(&CGPSDlg::QueryDrMemsNoiseLevel); }
	afx_msg void OnQueryAdrOdometerScalingFactor()
	{ GenericQuery(&CGPSDlg::QueryAdrOdometerScalingFactor); }
	afx_msg void OnQueryPpsOutputMode()
	{ GenericQuery(&CGPSDlg::QueryPpsOutputMode); }
	afx_msg void OnQueryPositionFixNavigationMask()
	{ GenericQuery(&CGPSDlg::QueryPositionFixNavigationMask); }
	afx_msg void OnQueryNmeaIntervalV8()
#if (CUSTOMER_ID == SWID)	//SWID customize
	{ GenericQuery(&CGPSDlg::QueryNmeaInterval2V8); }
#else
	{ GenericQuery(&CGPSDlg::QueryNmeaIntervalV8); }
#endif
	afx_msg void OnQueryEricssonInterval()
	{ GenericQuery(&CGPSDlg::QueryEricssonInterval); }
	afx_msg void OnQueryRefTimeSyncToGpsTime()
	{ GenericQuery(&CGPSDlg::QueryRefTimeSyncToGpsTime); }
	afx_msg void OnQuerySearchEngineSleepCriteria()
	{ GenericQuery(&CGPSDlg::QuerySearchEngineSleepCriteria); }
	afx_msg void OnQueryDatumIndex()
	{ GenericQuery(&CGPSDlg::QueryDatumIndex); }
	afx_msg void OnQueryVeryLowSpeed()
	{ GenericQuery(&CGPSDlg::QueryVeryLowSpeed); }
	afx_msg void OnQueryDofunUniqueId()
	{ GenericQuery(&CGPSDlg::QueryDofunUniqueId); }
	afx_msg void OnQueryCpuBoostMode()
	{ GenericQuery(&CGPSDlg::QueryCpuBoostMode); }
	afx_msg void OnQueryNavicMessageInterval()
	{ GenericQuery(&CGPSDlg::QueryNavicMessageInterval); }

	afx_msg void OnQueryUartPass()
	{ GenericQuery(&CGPSDlg::QueryUartPass); }
	afx_msg void OnGpsdoResetSlave()
	{ GenericQuery(&CGPSDlg::GpsdoResetSlave); }
	afx_msg void OnGpsdoEnterRom()
	{ GenericQuery(&CGPSDlg::GpsdoEnterRom); }
	afx_msg void OnGpsdoLeaveRom()
	{ GenericQuery(&CGPSDlg::GpsdoLeaveRom); }
	afx_msg void OnGpsdoEnterDownload()
	{ GenericQuery(&CGPSDlg::GpsdoEnterDownload); }
	afx_msg void OnGpsdoLeaveDownload()
	{ GenericQuery(&CGPSDlg::GpsdoLeaveDownload); }
	afx_msg void OnGpsdoEnterDownloadHigh()
	{ GenericQuery(&CGPSDlg::GpsdoEnterDownloadHigh); }
	afx_msg void OnGpsdoEnterUart()
	{ GenericQuery(&CGPSDlg::GpsdoEnterUart); }
	afx_msg void OnGpsdoLeaveUart()
	{ GenericQuery(&CGPSDlg::GpsdoLeaveUart); }
  afx_msg void OnInsdrEnterUart()
	{ GenericQuery(&CGPSDlg::InsdrEnterUart); }
	afx_msg void OnInsdrEnterDownload()
	{ GenericQuery(&CGPSDlg::InsdrEnterDownload); }
	afx_msg void OnInsdrLeaveUart()
	{ GenericQuery(&CGPSDlg::InsdrLeaveUart); }
	afx_msg void OnQueryNavigationModeV8()
	{ GenericQuery(&CGPSDlg::QueryNavigationModeV8); }
	afx_msg void OnQueryGnssBootStatus()
	{ GenericQuery(&CGPSDlg::QueryGnssBootStatus); }
	//afx_msg void OnQueryDrMultiHz()
	//{ GenericQuery(&CGPSDlg::QueryDrMultiHz); }
	afx_msg void OnQueryGnssKnumberSlotCnr2()
	{ GenericQuery(&CGPSDlg::QueryGnssKnumberSlotCnr2); }
	afx_msg void OnQueryGnssNmeaTalkId()
	{ GenericQuery(&CGPSDlg::QueryGnssNmeaTalkId); }
	afx_msg void OnQueryGnssConstellationType()
	{ GenericQuery(&CGPSDlg::QueryGnssConstellationType); }
	afx_msg void OnQueryCustomerID()
	{ GenericQuery(&CGPSDlg::QueryCustomerID); }
	afx_msg void OnQuery1ppsFreqencyOutput()
	{ GenericQuery(&CGPSDlg::Query1ppsFreqencyOutput); }
	afx_msg void OnQueryBinaryMeasurementDataOut()
	{ GenericQuery(&CGPSDlg::QueryBinaryMeasurementDataOut); }
	afx_msg void OnQueryRtcmMeasurementDataOut()
	{ GenericQuery(&CGPSDlg::QueryRtcmMeasurementDataOut); }
	afx_msg void OnQuerySerialNumber()
	{ GenericQuery(&CGPSDlg::QuerySerialNumber); }
	afx_msg void OnQueryDgps()
	{ GenericQuery(&CGPSDlg::QueryDgps); }
	afx_msg void OnQuerySmoothMode()
	{ GenericQuery(&CGPSDlg::QuerySmoothMode); }
	afx_msg void OnQueryTimeStamping()
	{ GenericQuery(&CGPSDlg::QueryTimeStamping); }
	afx_msg void OnQueryGpsTime()
	{ GenericQuery(&CGPSDlg::QueryGpsTime); }
	afx_msg void OnQuerySignalDisturbanceStatus()
	{ GenericQuery(&CGPSDlg::QuerySignalDisturbanceStatus); }
	afx_msg void OnQuerySignalDisturbanceData()
	{ GenericQuery(&CGPSDlg::QuerySignalDisturbanceData); }
	afx_msg void OnResetOdometer()
	{ GenericQuery(&CGPSDlg::ResetOdometer); }
	afx_msg void OnQueryCableDelay()
	{ GenericQuery(&CGPSDlg::QueryCableDelay); }
	//afx_msg void OnQueryGeofence()
	//{ m_nGeofecingNo = 0; GenericQuery(&CGPSDlg::QueryGeofence); }
	afx_msg void OnQueryGeofence1()
	{ m_nGeofecingNo = 1; GenericQuery(&CGPSDlg::QueryGeofenceEx); }
	afx_msg void OnQueryGeofence2()
	{ m_nGeofecingNo = 2; GenericQuery(&CGPSDlg::QueryGeofenceEx); }
	afx_msg void OnQueryGeofence3()
	{ m_nGeofecingNo = 3; GenericQuery(&CGPSDlg::QueryGeofenceEx); }
	afx_msg void OnQueryGeofence4()
	{ m_nGeofecingNo = 4; GenericQuery(&CGPSDlg::QueryGeofenceEx); }
	afx_msg void OnQueryGeofenceResult()
	{ GenericQuery(&CGPSDlg::QueryGeofenceResult); }
	afx_msg void OnQueryGeofenceResultEx()
	{ GenericQuery(&CGPSDlg::QueryGeofenceResultEx); }
	afx_msg void OnQueryRtkMode()
	{ GenericQuery(&CGPSDlg::QueryRtkMode); }
	afx_msg void OnQueryRtkMode2()
	{ GenericQuery(&CGPSDlg::QueryRtkMode2); }
	afx_msg void OnQueryRtkSlaveBaud()
	{ GenericQuery(&CGPSDlg::QueryRtkSlaveBaud); }	
	afx_msg void OnQueryRtkCpifBias()
	{ GenericQuery(&CGPSDlg::QueryRtkCpifBias); }	
	afx_msg void OnQueryRtkElevationAndCnrMask()
	{ GenericQuery(&CGPSDlg::QueryRtkElevationAndCnrMask); }	
  afx_msg void OnClearRtkSlaveData()
	{ GenericQuery(&CGPSDlg::ClearRtkSlaveData); }	
  afx_msg void OnQueryBasePosition()
	{ GenericQuery(&CGPSDlg::QueryBasePosition); }
	afx_msg void OnQueryRtkParameters()
	{ GenericQuery(&CGPSDlg::QueryRtkParameters); }
	afx_msg void OnQueryPstmDeviceAddress()
	{ GenericQuery(&CGPSDlg::QueryPstmDeviceAddress); }
	afx_msg void OnQueryPstnLatLonDigits()
	{ GenericQuery(&CGPSDlg::QueryPstnLatLonDigits); }
	afx_msg void OnQueryPsti030()
	{ m_nPstiNo = 30; GenericQuery(&CGPSDlg::QueryPsti); }
	afx_msg void OnQueryPsti032()
	{ m_nPstiNo = 32; GenericQuery(&CGPSDlg::QueryPsti); }
	afx_msg void OnQueryPsti033()
	{ m_nPstiNo = 33; GenericQuery(&CGPSDlg::QueryPsti); }
	afx_msg void OnQueryPsti063()
	{ m_nPstiNo = 63; GenericQuery(&CGPSDlg::QueryPsti); }
	afx_msg void OnQueryPsti065()
	{ m_nPstiNo = 65; GenericQuery(&CGPSDlg::QueryPsti); }
	afx_msg void OnQueryPsti067()
	{ m_nPstiNo = 67; GenericQuery(&CGPSDlg::QueryPsti); }
	afx_msg void OnQueryPsti068()
	{ m_nPstiNo = 68; GenericQuery(&CGPSDlg::QueryPsti); }
	afx_msg void OnQueryPsti070()
	{ m_nPstiNo = 70; GenericQuery(&CGPSDlg::QueryPsti); }

	afx_msg void OnQueryPsti()
	{ GenericQuery(&CGPSDlg::QueryPsti); }
  afx_msg void OnQueryPsti004()
	{ m_nPstiNo = 4; GenericQuery(&CGPSDlg::QueryPsti); }
	afx_msg void OnReCalcuteGlonassIfb()
	{ GenericQuery(&CGPSDlg::ReCalcuteGlonassIfb); }
	afx_msg void OnBinaryQueryClockOffset()
	{ GenericQuery(&CGPSDlg::BinaryQueryClockOffset); }
#if(NAVSPARK_MINI_GPIO_QUERY)
  int m_gpio;
	afx_msg void OnQueryGpio4Data()
	{ m_gpio = 4; GenericQuery(&CGPSDlg::QueryGpioData); }
	afx_msg void OnQueryGpio5Data()
	{ m_gpio = 5; GenericQuery(&CGPSDlg::QueryGpioData); }
	afx_msg void OnQueryGpio28Data()
	{ m_gpio = 28; GenericQuery(&CGPSDlg::QueryGpioData); }
	afx_msg void OnQueryGpio29Data()
	{ m_gpio = 29; GenericQuery(&CGPSDlg::QueryGpioData); }
	afx_msg void OnQueryGpio30Data()
	{ m_gpio = 30; GenericQuery(&CGPSDlg::QueryGpioData); }
	afx_msg void OnQueryGpio31Data()
	{ m_gpio = 31; GenericQuery(&CGPSDlg::QueryGpioData); }
#endif
	afx_msg void OnInsdrAccelerometerSelfTest()
	{ GenericQuery(&CGPSDlg::InsdrAccelerometerSelfTest); }
	afx_msg void OnInsdrGyroscopeSelfTest()
	{ GenericQuery(&CGPSDlg::InsdrGyroscopeSelfTest); }
	afx_msg void OnInsdrAccumulateAngleStart()
	{ GenericQuery(&CGPSDlg::InsdrAccumulateAngleStart); }
	afx_msg void OnInsdrAccumulateAngleStop()
	{ GenericQuery(&CGPSDlg::InsdrAccumulateAngleStop); }

	afx_msg void OnEnterRtkDebugMode()
	{ GenericQuery(&CGPSDlg::EnterRtkDebugMode); }
	afx_msg void OnBackRtkDebugMode()
	{ GenericQuery(&CGPSDlg::BackRtkDebugMode); }
	afx_msg void OnQuery1ppsPulseWidth()
	{ GenericQuery(&CGPSDlg::Query1ppsPulseWidth); }
	afx_msg void OnQueryAlphaUniqueId()
	{ GenericQuery(&CGPSDlg::QueryAlphaUniqueId); }
	afx_msg void OnQueryAlphaKey()
	{ GenericQuery(&CGPSDlg::QueryAlphaKey); }
	afx_msg void OnTmpActiveLicense()
	{ GenericQuery(&CGPSDlg::TmpActiveLicense); }

	afx_msg void OnAlphaAgCalibrationUp()
	{ GenericQuery(&CGPSDlg::AlphaAgCalibrationUp); }
	afx_msg void OnAlphaAgCalibrationDown()
	{ GenericQuery(&CGPSDlg::AlphaAgCalibrationDown); }
	afx_msg void OnAlphaEcompassCalibration()
	{ GenericQuery(&CGPSDlg::AlphaEcompassCalibration); }

  afx_msg void OnQueryV9TagAddress()
	{ GenericQuery(&CGPSDlg::QueryV9TagAddress); }
  afx_msg void OnQueryV9UniqueId()
	{ GenericQuery(&CGPSDlg::QueryV9UniqueId); }
  afx_msg void OnQueryPhoenixSoftwareFeature()
	{ GenericQuery(&CGPSDlg::QueryPhoenixSoftwareFeature); }
  afx_msg void OnQueryV9ExtendedId()
	{ GenericQuery(&CGPSDlg::QueryV9ExtendedId); }
  afx_msg void OnQueryV9Tag()
	{ GenericQuery(&CGPSDlg::QueryV9Tag); }
  afx_msg void OnQuerySecurityTag()
	{ GenericQuery(&CGPSDlg::QuerySecurityTag); }
  afx_msg void OnQuerySecurityTagOnly()
	{ GenericQuery(&CGPSDlg::QuerySecurityTagOnly); }
  afx_msg void OnQueryV9PromAesTag()
	{ GenericQuery(&CGPSDlg::QueryV9PromAesTag); }
  afx_msg void OnQueryV9ExternalAesTag()
	{ GenericQuery(&CGPSDlg::QueryV9ExternalAesTag); }
  afx_msg void OnResetV9AesTag()
	{ GenericQuery(&CGPSDlg::ResetV9AesTag); }
  //afx_msg void OnQueryCustomStringInterval()
	//{ GenericQuery(&CGPSDlg::QueryCustomStringInterval); }

	struct MenuItemEntry {
		BOOL showOption;
		UINT type;			//MF_STRING, MF_SEPARATOR, MF_POPUP
		UINT id;
		LPCSTR	pszText;
		MenuItemEntry*	subMenu;
	};

	struct UiLocationEntry {
		BOOL showOption;
		UINT type;			//MF_STRING, MF_SEPARATOR, MF_POPUP
		UINT id;
		RECT rect;
	};

	enum PrnType
	{
		Unknown = 0,
		Gps = 1,
		Glonass = 2,
		Beidou = 3,
		Gallilo = 4,
	};

	enum CoorFormat
	{
		Degree = 0,
		DegreeMinute,
		DegreeMinuteSecond,
	};

	enum AltitudeFormat
	{
		Altitude = 0,
		EllipsoidHeight,
	};

	BOOL m_copyLatLon;
	CoorFormat m_coorFormat;
	AltitudeFormat m_altFormat;
	void Show_EarthChart(CDC *dc);

	//void DrawGnssSatellite(CDC* dc, int id, int centerX, int centerY);
	//void DrawBdSatellite(CDC* dc, int id, int centerX, int centerY);
	//void DrawGaSatellite(CDC* dc, int id, int centerX, int centerY);

	void parse_sti_03_message(const char *buff,int len); /* for timing module */
	void parse_sti_04_001_message(const char *buff, int len); /* for timing module */
	void parse_sti_message(const char *buff,int len);
	void parse_pirnsf_message(const char *buff,int len);
	void parse_sti_0_message(const char *buff,int len); /* for timing module */
	//	void parse_rtoem_message(const char *buff, int len);
	void parse_psti_50(const char *buff);
	void parse_sti_20_message(const char *buff,int len); /* for timing module */

#if(_MORE_INFO_ || _TAB_LAYOUT_)
	void parse_sti_30_message(const char *buff,int len); /* for RTK module */
#endif
#if (_SHOW_RTK_BASELINE_ || _TAB_LAYOUT_)
	void parse_sti_31_message(const char *buff,int len); /* for RTK module */
#endif
#if(_TAB_LAYOUT_)
	void parse_sti_32_message(const char *buff,int len); /* for RTK module moving base*/
	void parse_sti_33_message(const char *buff,int len); /* for RTK module moving base*/
#endif

	//	void Config_silab_baudrate(HANDLE *m_DeviceHandle);
	//	void Config_silab_baudrate_flash(HANDLE *m_DeviceHandle);
	bool DoCommonConfig(CCommonConfigDlg* dlg);
	bool DoCommonQuery(CCommonQueryDlg* dlg);
	void DoCommonConfigNoDisconnect(CCommonConfigDlg* dlg);
	void DoCommonConfigDirect(CCommonConfigDlg* dlg, int type);

	//Functions for combain GPS / GNSS Viewer UI Layout.
	int CreateSubMenu(const HMENU hMenu, const MenuItemEntry* menuItemTable, LPCSTR pszSubMenuText);
	//PrnType GetPrnType(int id);
	void RescaleDialog();
	void SwitchToConnectedStatus(BOOL bSwitch);
	void SetFactoryDefault(bool isReboot);
	void GetGPSStatus();
	bool DoZenlandInit();
	bool DoZenlandQuery();
	void SetConnectTitle(bool isInConnect);
	//Test Clock Offser
	U08 QueryChanelFreq(int chanel, U16 *prn, double *freq);
	U08 PredictClockOffset(double *clk_offset);
	//End
	bool WriteRegister(U32 addr, U32 data, LPCSTR prompt = NULL);

	//	int m_comPort;
	//	int m_baudrate;
	WPARAM plugin_wParam;
	CString plugin_port_name;
	U16 m_customerId;

	CLabel m_wgs84_x,m_wgs84_y,m_wgs84_z;
	CLabel m_enu_e,m_enu_n,m_enu_u;
#if(!MERGE_BD_GA_GI)
	CSnrBarChartGalileo* gaSnrBar;
	CSnrBarChartNavic* giSnrBar;
#endif

#if(SUPPORT_BDL2_GSV2)
  CSnrBarChartDualL2* gpsSnrBar;
	CSnrBarChartL2* bdSnrBar;
#elif(!MERGE_BD_GA_GI)
	CSnrBarChartDual* gpsSnrBar;
	CSnrBarChartBeidou* bdSnrBar;
#else
	CSnrBarChartMulti* gpsSnrBar;
	CSnrBarChartMulti* bdSnrBar;
#endif
	CPic_Scatter* pic_scatter;
	CPic_Earth* pic_earth;
	CLabel m_fixed_status;

	CEdit m_odo_meter;
	//CEdit m_gyro_data;
	CEdit m_backward_indicator;
	CStatic m_lbl_odo_meter;
	CStatic m_lbl_gyro_data;
	CStatic m_lbl_backward_indicator;

	BinaryData m_versionInfo;
	BinaryData m_bootInfo;

	bool m_firstDataIn;
	U32 m_customerID;

	DECLARE_MESSAGE_MAP()
public:
  void OpenAndSetAgpsEphemerisFile(const CString& file, bool restoreConnection);
  void AutoAgpsBackgroundDownload();
  int m_nPstiNo;
  void AutoAgpsSilentModeOn() { m_autoAgpsSilentMode = TRUE; }
  void AutoAgpsSilentModeOff() { m_autoAgpsSilentMode = FALSE; }
protected:
  CTime m_autoGi3EphTime;
  BinaryData m_autoGi3Eph;
  BOOL m_autoAgpsSilentMode;

  int m_nDoFlag;
#if(!MERGE_BD_GA_GI)
  CRect m_rcLeftSnr, m_rcRightSnr;
  CRect m_rcLeftSnrT, m_rcRightSnrT;
  CRect m_rcLeftFreqT, m_rcRightFreqT;
#endif
	bool ShowCommand(U08 *buffer, int length);
	void ShowFormatError(U08* cmd, U08* ack);
	void SwitchInfoTab();
  void InitMessageBox(MsgMode mode);
#if defined(XN120_TESTER)
  CString m_strXn120Version;
  int m_nXn120TestSatus;
  void DoXn120Tester();
  void Xn116TesterEvent();
#endif

public:
  static CmdErrorCode SendComCmdWithAck(HANDLE com, U08* cmd, int size, DWORD timeOut);
  bool DoCConfigRegisterDirect(U32 addr, U32 value);
  bool DoCConfigRfIcDirect(U08 type, U32* reg, int size);

protected:
  enum RtkStatusChanged
  {
    BecomeRtkFloat = 1,
    BecomeGnssFixed,
    BecomeNoFixed,
    RebootDetected,
  };
  int m_becomeRtkFloat;
  int m_becomeGnssFixed;
  void OutputRtkStatusChangedMessage(RtkStatusChanged type);
};
