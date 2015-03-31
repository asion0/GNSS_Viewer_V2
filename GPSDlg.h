// GPSDlg.h : header file
//
#pragma once

#include "resource.h"
#include "Global.h"
#include "DataLog.h"

#include "NMEA.h"
#include "ScanDlg.h"
#include "CfgMsg.h"
#include "Savenmea.h"
#include "Agps_config.h"
#include "FTPDlg.h"
#include "Registry.h"
#include "Con_binary_msg_interval.h"
#include "Login.h"
#include "ClipboardListBox.h"
#include "Label.h"
#include "ColorStatic.h"
#include "MsgList.h"

#include <Dbt.h>
#include <setupapi.h>

#define RA   (6378137.0)   // semi-major earth axis(ellipsoid equatorial radius)
#define RB   (6356752.0)   // semi-major earth axis(ellipsoid polar radius)
#define E2   (0.006694478) // eccentricity squared: (RA*RA-RB*RB)/RA*RA    
#define E2P  (0.006739596) // eccentricity squared: (RA*RA-RB*RB)/RB*RB
#define ICD_PI           (3.1415926535898)  // ICD 200-c p.101 

#define UWM_KERNEL_REBOOT	(WM_USER + 0x134)
#define UWM_FIRST_NMEA		(WM_USER + 0x135)
#define UWM_SHOW_TIME		(WM_USER + 0x136)
#define UWM_UPDATE_UI		(WM_USER + 0x137)
#define UWM_SHOW_RMC_TIME   (WM_USER + 0x138)
#define UWM_GPSDO_HI_DOWNLOAD   (WM_USER + 0x139)

enum DownloadErrocCode
{
	RETURN_NO_ERROR = 0,
	RETURN_RETRY,
	RETURN_ERROR
};

#define GNSS_CHANEL_LIMIT	16

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

typedef struct EllipsoidList
{
	U32 a;
    U32 I_F;
}EL;

typedef struct ellipsoidlist
{
	D64 a;
    D64 I_F;
}TEL;

typedef struct datumreferencelist
{
	S16 DeltaX;
	S16 DeltaY;
	S16 DeltaZ;
	D64 Semi_Major_Axis;
	D64 Inversd_Flattening;
	U08 EllipsoidIndex;
}TDRL;

typedef struct DatumReferenceList
{
	S16 DeltaX;
	S16 DeltaY;
	S16 DeltaZ;
	U32 Semi_Major_Axis;
	U32 Inversd_Flattening;
	U08 EllipsoidIndex;
}DRL;

#define sma  6370000000
#define IF   293000000000
#define IF1  2930000000

typedef struct {
	U08 Timing_mode;
	U32 Survey_Length;
	U08 RT_Timing_mode;
	U32 RT_Survey_Length;
	D64 latitude;
	D64 longitude;
	F32 altitude;
	D64 RT_latitude;
	D64 RT_longitude;
	F32 RT_altitude;
	U08 attributes;
	U32 Standard_deviation;

} _1PPS_Timing_T;


struct LL2 {
	double lat;
	double lon;
	double speed;
	double alt;
	UTC_T utc;
};

// Copy from HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\DeviceClasses
static const GUID GUID_DEVINTERFACE_LIST[] = 
{
	// GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },

		// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },

	// GUID_DEVINTERFACE_HID, 
	{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },

	// GUID_NDIS_LAN_CLASS
	{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }

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
};

enum WlfResult {
	wlf_None,
	wlf_ok,
	wlf_end,
	wlf_error1,
	wlf_error2,
	wlf_error3,
	wlf_error4,
	wlf_error5,
	wlf_resend,
	wlf_reset,
	wlf_resendbin,
	wlf_timeout,
	//for Loader debug
	wlf_error41,
	wlf_error42,
	wlf_error43,
	wlf_Ready,
	wlf_Ready1,
	wlf_Ready2,
};

class CSoftImDwDlg;
class CSerial;
class CSnrBarChartGps;
class CSnrBarChartGlonass;
class CSnrBarChartGpsGlonass;
class CSnrBarChartBeidou;
class CSnrBarChartGalileo;
class CSnrBarChart;
class CPic_Scatter;
class CPic_Earth;
class CCigRgsDlg;
class CGetRgsDlg;
class BinaryCommand;
class BinaryData;
class CPanelBackground;
class CCommonConfigDlg;
// CGPSDlg dialog
class CGPSDlg : public CDialog
{
// Construction
public:
	CGPSDlg(CWnd* pParent = NULL);	// standard constructor
	~CGPSDlg();

	static UINT UWM_PLAYNMEA_EVENT;
	static UINT UWM_SAVENMEA_EVENT;
	static UINT UWM_UPDATE_EVENT;
	static CGPSDlg* gpsDlg;

	enum { IDD = IDD_GPS_DIALOG };
#if MORE_ENU_SCALE
	enum { DefauleEnuScale = 3 };
#else
	enum { DefauleEnuScale = 0 };
#endif
	CString m_lastGpEphFile;
	CString m_lastGlEphFile;
	CString m_lastBdEphFile;
protected:
	HICON m_hIcon;

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
	CEdit m_clock_offset;
	CEdit m_noise;

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedRecord();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedClose();
	afx_msg void OnCbnCloseupCoordinate();
	afx_msg void OnCbnCloseupEnuscale();
	afx_msg void OnCbnCloseupMapscale();
	afx_msg void OnBnClickedSetorigin();
	//afx_msg void OnFileSaveNmea0183();
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
	afx_msg void OnSetFactoryDefaultNoReboot();
	afx_msg void OnSetFactoryDefaultReboot();
	afx_msg void OnConfigureoutputmessagetypeNooutput();
	afx_msg void OnConfigureoutputmessagetypeNmeamessage();
	afx_msg void OnConfigureoutputmessagetypeBinarymessage();
	afx_msg void OnBinaryConfigurenmeaoutput();
	afx_msg void OnConfigureNmeaIntervalV8();
	afx_msg void OnConfigureEricssonSentecneInterval();
	afx_msg void OnConfigureSerialNumber();
	afx_msg void OnBinaryConfiguredatum();
	afx_msg void OnBinaryConfiguredopmask();
	afx_msg void OnBinaryConfigureelevationmask();
	afx_msg void OnBinaryConfigurebinarydata();
	afx_msg void OnConverterDecompress();	
	afx_msg void OnCovDecopre();
	afx_msg void OnConverterCompress();
	afx_msg void OnLoggerConvert();	
	afx_msg void OnDatalogClearControl();
	afx_msg void OnDatalogLogconfigurecontrol();	
	afx_msg void OnSoftwareimagedownloadLoaderimage();
	afx_msg void OnSoftwareimagedownloadImageonly();
	afx_msg void OnBinaryGetrgister();
	afx_msg void OnBinaryConfigureregister();
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
	afx_msg void OnVerifyFirmware();
	afx_msg void OnFilePlayNmea();
	afx_msg void OnConverterKml();
	afx_msg void OnRawMeasurementOutputConvert();
	afx_msg void OnBnClickedScanall();
	afx_msg void OnBnClickedScanport();
	afx_msg void OnBnClickedScanbaudrate();
	afx_msg void OnSetUartPassThrough();
	afx_msg void OnBinaryConfiguremessagetype();
	afx_msg void OnEphemerisGetephemeris();
	afx_msg void OnEphemerisSetephemeris();
//	afx_msg void OnAgpsFtp();
//	afx_msg void OnDownloadDownload();
	afx_msg void OnAgpsConfig();
//	afx_msg void OnSoarcommLogin();
//	afx_msg void OnSoarcommLogout();
//	afx_msg void OnSoarcommConfigpassword();
	afx_msg void OnFileCleannema();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnWaasQuerywaasstatus();
	afx_msg void OnWaasWaasenable();
	afx_msg void OnWaasWaasdisable();
	afx_msg void OnBinaryConfiguremessageType();
	afx_msg void OnBinaryConfigurebinarymsginterval();
	afx_msg void OnBinaryConfigureBinaryInterval();
	afx_msg void OnBinaryPositionfinder();
	afx_msg void OnBinaryConfigurepositionrate();
	afx_msg void OnBinaryConfigDrMultiHz();
	afx_msg void OnDatalogLogReadBatch();
	afx_msg void OnBinaryConfigurepositionpinning();
	afx_msg void OnBinaryConfigurepinningparameters();
	afx_msg void OnMultimodeConfiguremode();
	afx_msg void OnMultimodeQuerymode();
	afx_msg void OnBinaryConfiguresubsecregister();
	afx_msg void OnBinaryConfigure1pps();
	afx_msg void OnBinaryQuery1pps();
//	afx_msg void OnBnClickedSetoriginUser();
	afx_msg void OnBinaryConfigurepowermode();
	afx_msg void OnBinaryConfiguremultipath();
	afx_msg void OnWaasWaas();
	afx_msg void OnGetGpsAlmanac();
	afx_msg void OnBinaryQuerybinarymsginterval();
	afx_msg void OnBinaryResetodometer();
	afx_msg void OnConfigure1ppstimingConfigure1ppstiming();
	afx_msg void OnConfigure1ppstimingConfigure1ppscabledelay();
	afx_msg void OnConfigure1ppstimingConfigure1pps();
	afx_msg void OnConfigElevationAndCnrMask();
	afx_msg void OnQuery1ppstimingQuerycabledelay();
	afx_msg void On1ppstimingMonitoring1pps();

	afx_msg void On1ppstimingConfigureproprietarynmea();
	afx_msg void OnSetGpsAlmanac();
	afx_msg void OnMinihomerActivate();
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

	afx_msg void OnMinihomerSettagecco();
	afx_msg void OnMinihomerQuerytag();

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
//	afx_msg void OnGlonassConfigureusbdriver();
	afx_msg void OnBnClickedRomMode();
//	afx_msg void OnBinaryConfiguregnssselectionfornavigationsystem();
	afx_msg void OnBnClickedKnumEnable();
	afx_msg void OnBnClickedKnumDisable();
	afx_msg void OnBinaryConfigurenmeaoutput32953();
	afx_msg void OnBinaryConfigurenmeatalkerid();
//	afx_msg void OnCfgGlonassAcquisitionMode();
	afx_msg void OnGetGlonassAlmanac();
	afx_msg void OnSetGlonassAlmanac();
	afx_msg void OnGetBeidouAlmanac();
	afx_msg void OnSetBeidouAlmanac();
	afx_msg void OnEphemerisGetgpsglonass();
	afx_msg void OnEphemerisSetgpsglonass();
	afx_msg void OnEphemerisGetgpsglonassalmanac();
	afx_msg void OnEphemerisSetgpsglonassAlmanac();
	afx_msg void OnEphemerisGettimecorrections();
	afx_msg void OnEphemerisSettimecorrections();

	afx_msg void OnConfigProprietaryMessage();
	afx_msg void OnConfigPowerSavingParameters();
	afx_msg void OnConfigPowerSavingParametersRom();
	afx_msg void OnBinaryConfigureantennadetection();
//	afx_msg void On1ppstimingConfigure1ppsnmeadelay();
//	afx_msg void On1ppstimingQuery1ppsnmeadelay();
	afx_msg void On1ppstimingConfigureppsoutputmode();	//new
	afx_msg void On1ppstimingQueryppsoutputmode();
//	afx_msg void On1ppstimingEnterreferenceposition32961();
	afx_msg void OnConfigGnssDozeMode();

	afx_msg void OnBnClickedECompassCalibration();
	afx_msg void On1ppstimingConfigurePulseWidth();
	afx_msg void On1ppsTimingQuery1ppsPulseWidth();
	afx_msg void OnConfigQueryGnssNavSol();
	afx_msg void OnConfigBinaryMeasurementDataOut();

	afx_msg void OnConfig1ppsFrequencyOutput();
	afx_msg void On1ppstimingConfigureppspulseclksrc();
	afx_msg void On1ppstimingEnterreferenceposition32977();
	afx_msg void On1ppstimingQueryppspulseclksrc();

	afx_msg void OnBinaryConfigureSBAS();
	afx_msg void OnBinaryConfigureSAGPS();
	afx_msg void OnBinaryConfigureQZSS();
	afx_msg void OnBinaryConfigureDGPS();
	afx_msg void OnBinaryConfigureSmoothMode();
	afx_msg void OnBinaryConfigTimeStamping();
	afx_msg void OnConfigLeapSeconds();
	afx_msg void OnConfigParamSearchEngineSleepCriteria();
	afx_msg void OnConfigDatumIndex();
	afx_msg void OnConfigureNoisePowerControl();
	afx_msg void OnConfigureInterferenceDetectControl();
	afx_msg void OnConfigNMEABinaryOutputDestination();
	afx_msg void OnConfigParameterSearchEngineNumber();
	afx_msg void OnAgpsFtpSrec();
	afx_msg void OnRomAgpsFtpSrec();
	afx_msg void OnRomAgpsFtpNew();
	afx_msg void OnClockOffsetPredict();
	afx_msg void OnClockOffsetPredictOld();
	afx_msg void OnHostBasedDownload();
	afx_msg void OnFiremareDownload();
	afx_msg void OnConfigPositionFixNavigationMask();
	afx_msg void OnParallelDownload();
	afx_msg void OnConfigRefTimeSyncToGpsTime();
	afx_msg void OnNmeaChecksumCalculator();
	afx_msg void OnBinaryChecksumCalculator();
	afx_msg void OnTestExternalSrec();

	afx_msg void OnGetGlonassEphemeris();
	afx_msg void OnSetGlonassEphemeris();
	afx_msg void OnGetBeidouEphemeris();
	afx_msg void OnSetBeidouEphemeris();

	afx_msg void OnSup800EraseData();
	afx_msg void OnSup800WriteData();
	afx_msg void OnSup800ReadData();

	afx_msg void OnConfigureSignalDisturbanceStatus();
	afx_msg void OnConfigureGpsUtcLeapSecondsInUtc();
	afx_msg void OnGpsdoFirmwareDownload();

	CBitmapButton m_ConnectBtn;
	CBitmapButton m_PlayBtn;
	CBitmapButton m_StopBtn;
	CBitmapButton m_RecordBtn;
public:
	CBitmapButton m_CloseBtn;
	CString m_nmeaPlayFilePath;
	HANDLE m_nmeaPlayThread;
	int m_nmeaPlayInterval;
	bool m_nmeaPlayPause;
	CCriticalSection _nmeaPlayInterval;
	CCriticalSection csSatelliteStruct;

protected:
    CBitmapButton m_SetOriginBtn;	
	CBitmapButton m_ClearBtn;		
	CBitmapButton m_DownloadBtn;
	CBitmapButton m_EarthSettingBtn;
	CBitmapButton m_ScatterSettingBtn;

	U08 MSG_TYPE_STORAGE[1275];
	bool NmeaInput();
	bool ComPortInput();
    
    int maplondeg,    maplonmin,    maplonsec,    maplatdeg,    maplatmin,    maplatsec;
 //   void error(LPSTR ) ;
	
	//CCriticalSection _GETNMEA0183CS;
	CCriticalSection _BINMSGCS;
	CCriticalSection _save_nmea_cs;
	
	Satellite satecopy_gps[MAX_SATELLITE];
	Satellite sate_gps[MAX_SATELLITE];	
	Satellite satecopy_gnss[MAX_SATELLITE];
	Satellite sate_gnss[MAX_SATELLITE];	
	Satellite satecopy_bd[MAX_SATELLITE];
	Satellite sate_bd[MAX_SATELLITE];	
	Satellite satecopy_ga[MAX_SATELLITE];
	Satellite sate_ga[MAX_SATELLITE];	


	void UpdateCooridate();
private:
	char m_currentDir[MyMaxPath];
	bool m_gpsdoInProgress;
	CButton m_binarymsg;
	CButton m_nmea0183msg;
	CButton m_no_output;
//	CCigRgsDlg* pCRDlg;
	CStatic m_connectT;
	CFile m_convertFile;
	CListCtrl m_kNumList;
	CString m_nmeaFilePath;
	CToolTipCtrl m_tip;
	GNSS_T m_gnss;
	GNSS_T m_gnssTemp;
	bool m_isFlogOpen;
	CClipboardListBox m_responseList;	
	void ClearInformation(bool onlyQueryInfo = false);
	bool DoDownload(int dlBaudIdx);
public:
	static CFont m_textFont;
	static CFont m_infoFontS;
	static CFont m_infoFontL;
	static CFont comboFont;;	
	static CFont messageFont;

	CMsgList m_nmeaList;
	CFile m_ephmsFile;

	CFile m_nmeaFile;
	U32 m_nmeaFileSize;

	CSaveNmea* m_saveNmeaDlg;
	CPlayNmea* m_playNmea;
	CScanDlg* m_pScanDlg;
	CSerial* m_serial;
	CSoftImDwDlg* m_psoftImgDlDlg;
//	FILE *fbin;
	GPGGA m_gpggaMsgBk;
	GPGLL m_gpgllMsg, m_gpgllMsgCopy, m_gpgllMsgCopy1;
	GPGSA m_glgsaMsg, m_glgsaMsgCopy, m_glgsaMsgCopy1;
	GPGSV m_glgsvMsg, m_glgsvMsgCopy, m_glgsvMsgCopy1;
	GPGGA m_gpggaMsg, m_gpggaMsgCopy, m_gpggaMsgCopy1;
    GPGSA m_gpgsaMsg, m_gpgsaMsgCopy, m_gpgsaMsgCopy1;

    GPGSV m_gpgsvMsg, m_gpgsvMsgCopy, m_gpgsvMsgCopy1;
    GPRMC m_gprmcMsg, m_gprmcMsgCopy, m_gprmcMsgCopy1;
    GPVTG m_gpvtgMsg, m_gpvtgMsgCopy, m_gpvtgMsgCopy1;

#if(_MODULE_SUP_800_)
	PSTI004001 m_psti004001, m_psti004001Copy, m_psti004001Copy1;
#endif

    GPZDA m_gpzdaMsg, m_gpzdaMsgCopy, m_gpzdaMsgCopy1;
	//for Beidou
    GPGSA m_bdgsaMsg, m_bdgsaMsgCopy, m_bdgsaMsgCopy1;
    GPGSV m_bdgsvMsg, m_bdgsvMsgCopy, m_bdgsvMsgCopy1;
	//for Galileo
    GPGSA m_gagsaMsg, m_gagsaMsgCopy, m_gagsaMsgCopy1;
    GPGSV m_gagsvMsg, m_gagsvMsgCopy, m_gagsvMsgCopy1;


	LogFlashInfo1 m_logFlashInfo;
	U08 m_ms;
	U08 m_ns;
	U08 m_pllDiv;	
//	U08 Binary_mode;
	U16 m_ttffCount;	
	bool m_initTtff;
	bool m_setTtff;
	U32  m_regAddress;	
	bool m_isPressCloseButton;
	bool m_isConnectOn;		
//	bool m_isPressNmeaCommend;
//	bool m_isNmeaFileOpen;	
//	bool Reconnect;
	bool m_isNmeaUpdated;
	//int  m_nmeaCount;	
	
	enum { NmeaBufferSize = 1024 };
	char m_nmeaBuffer[NmeaBufferSize];		//NMEA_MSG[1024]
//	char binmsg[100];
	static U08 m_inputMsg[200];

	void SetTTFF(int t)
	{
		CString str;
		str.Format("%d", t);
		GetDlgItem(IDC_TTFF)->SetWindowText(str);
	}
	void SetKernelTestTTFF(int t, int c)
	{
		CString str;
		str.Format("%d(%d)", t, c);
		GetDlgItem(IDC_TTFF)->SetWindowText(str);
	}
	//尚未整理分隔線
	const char* GetCurrentDir() { return m_currentDir; }

	U08 IsSuccessful(U08* buff, int tail, bool show_msg = true);
//	U08 Rom(CString prom_path);
	bool IsEphmsEmpty(BYTE* buffer);
	bool CeheckOrigin(CString,int);	
	bool CfgPortSendToTarget(U08*,U16,char*);
	bool CheckConnect();
	bool CheckGPS(U08*,U16,char*);
	bool CheckTimeOut(DWORD duration, DWORD timeOut = 10000,  bool silent = false);
	bool CloseOpenUart();
//	bool FTPSetEphms(CFTPDlg *ftpDlg);
//	bool Flash(CString flash_path);
	bool ListSoftVersion(unsigned char* ,int);
	bool NmeaProc(const char*, int, NmeaType& );
//	bool OpenUart();
	bool SendMsg();
	bool SendToTarget(U08* ,U16 ,const char*, bool quick = false);
	bool SendToTargetNoAck(U08*,U16);
	bool SendToTargetNoShow(U08*,U16,char*);
	bool SendToTargetNoWait(U08*,U16,LPCSTR);
	bool TIMEOUT_METHOD(time_t,time_t);
//	bool chk_gp(char* buff,int size);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void BinaryProc(unsigned char*,int);
//	void BaudrateError(void);
	void CopyNmeaToUse();
	void ClearQue();
//	void ConfigureRegister(U08*);
	void Copy_NMEA_Memery();
//	void CreatWaitAck();
	void CreateGPSThread();	
	void DataLogDecompress(bool);
//	void Deg2Rad(double&);    
	void DeleteNmeaMemery();
	void GetLogStatus(U08*);
	void GetRegister(U08*);	
//	void LogClear(U08*);
	void LogConfigure();
	void MSG_PROC();
	void QueryMsg(unsigned char*);
	void Rad2Deg(double&);
	void Restart(U08*);
	void ScanGPS();
	void ScanGPS1();
	void ScanGPS2();
	void ScatterPlot(CDC *dc);
	void SetEphms(U08 continues);
	void SetMode();
//	void SetOrigiPort();
	void SetPort(U08,int mode);
//	void SetRotationMatrix();
#if(_MODULE_SUP_800_)
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

	void ShowEarth(CDC *dc);
	void ShowLongitudeLatitude(void);
	void DisplayTime(int h, int m, int s);
	void DisplayTime(int h, int m, D64 s);
	void DisplayDate(int y, int m, int d);
	void DisplayLongitude(int h, int m, double s, char c);
	void DisplayLatitude(int h, int m, double s, char c);
	void DisplaySpeed(D64 speed);
	void DisplayDirection(D64 direction);
	void DisplayStatus(GnssData::QualityMode m);
	void DisplayLongitude(D64 lon, U08 c);
	void DisplayLatitude(D64 lat, U08 c);
	void DisplayAltitude(D64 alt);
	void DisplayHdop(D64 hdop);

	void ShowRMCTime();
	void ShowStatus();
	void ShowTime(void);
	void Terminate(void);
	void TerminateGPSThread();
	void WriteIni();
	void WriteKMLPath(CFile& ,double ,double );
	void WriteKMLini(CFile&  ,double ,double );	
//	void WritePOIPath(CFile& file ,vector<LLA_T> *lst );
//	void close_nmea_file();
	void ConvertGpsTimeToUtc(S16, D64, UTC_T*);	    
//	void stop_write_nmea();
    void Initialization();
    void SetFacMsg(unsigned char*);	
    void continue_write_nmea();

private:
	
	HANDLE handle_version;
	HANDLE wait_version_complete;
	U08 Soft_Version;
	void Load_Menu();
	//void refresh_scatterplot();

	CDC bk_dc;
	CDC bar_dc;
	CDC earth_dc;
	CDC chart_dc;
	
	CButton m_rom_mode;
	int m_noisePower;

	CStatic m_lbl_download;
	CButton m_btn_browse;

public:
	int SetMessage(U08* msg, int len);
	int SetMessage2(U08* dst, U08* src, int srcLen);
	int GetCoordinateSel() { return m_coordinate.GetCurSel(); }
	int GetScaleSel() { return m_scale.GetCurSel(); }
	int GetMapScaleSel() { return m_mapscale.GetCurSel(); }
	bool IsFixed() { return ::IsFixed(m_gpggaMsg.GPSQualityIndicator); }
	void SetNmeaUpdated(bool b);
	bool SetFirstDataIn(bool b);
	void SendRestartCommand(int mode);		
//	void target_restart();
	void target_only_restart(int mode);
//	void Query_Target_Version(unsigned char* messages,int message_len);
	bool TIMEOUT_METHOD_QUICK(time_t start,time_t end);
	void WaitEvent();
//	void Datalog_sector_read(U08* message,U08 msgid);
//	int Datalog_read_one(U08*,int sector_id,int sector_count);
//	int check_msg_valid(U08 *buff);
	void add_msgtolist(LPCTSTR msg);
	bool SendToTargetBatch(U08* message,U16 length,char* Msg);
	int SendToTargetEph(U08* message,U16 length,char* Msg,bool quick);
	U32 get_register_zero();
	bool send_command_withackString(U08 *rs_buff,int size,char *res);
	U08 wait_res(char* res);
//	int Datalog_read_one_srec(U08* message,int sector_id,int sector_count);
	//void OnLogLogtest();
//	void writelog(char *msg);
	CEvent test_event;
//	int SendToTarget_datalog(U08* message,U16 length,char* Msg);
	void cancel_log_read();
	int cancel_readlog;
//	void UnlockSoarcomm();
//	void UnlockPolstar();
//	bool IsFileExist(const char *file_path);
//	void cartesian_to_geodetic( const POS_T* xyz_p, LLA_T* lla_p );
//	U08 com_get_register();


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
//	void UTC_convert_utc_to_gps_time( const UTC_TIME_T *utc_time_p, S16 *wn_p, D64 *tow_p );
	U08 CheckEphAndDownload();
//	void set_eph_onerow(U08 *buff,U16 svid);
//	U08 set_now_eph();
	void ShowColdStartEarth();
//	void change_agps_status(U08 agps);
	CButton m_bnt_warmstart;
	CButton m_btn_coldstart;

	CButton m_agps_enable;
	CButton m_agps_disable;

	double warmstart_latitude;
	double warmstart_longitude;
	double warmstart_altitude;
	void ConfigBaudrate(int baud, int attribute);
//	U08 set_now_eph_4();
//	void writettff_log(int ttff);
//	void check_fixed();
	void WaitReady();
//	void SetPort_noAck(U08 port,int mode);
	void Show_Noise();
	void QueryRtc();
	void GetAlmanac();
	//void ShowBinaryOutput(unsigned char *buff,int len);

	void close_minitor_1pps_window();
	//=============================================
	void GetGpsAlmanac(CString m_almanac_filename,U08 sv,U08 continues);
	void SetGpsAlmanac(U08 continues);

	void GetAlmanac_tmp();
	void activate_minihomer();
	void set_minihomerkey(U08* key,int len);
	void set_minihomerid(U08* id,int len);
	void Close_Open_Port(WPARAM wParam,CString port_name);
	U08 MinihomerQuerytag();
	void MinihomerSettagecco();

	void query_dr_info();
//	U08 set_clock_offset(S32 clk_offset);


	void Create_earth_pic(CDC *dc);
//	void Create_bar_pic(CDC *dc);
//	void RefreshBarChart(CDC *bar_dc);
//	bool is_Fixed(U16 gga_indicator);
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
	void SetGlonassEphms(U08 continues);
	bool IsGlonassEphmsEmpty(BYTE* buffer);
	
	
	void GetTimeCorrection(CString m_filename);
	void SetTimeCorrection(CString m_filename);
	U08 parse_psti_others(const char *buff, int psti_id);

	//For Common Download Clasases
public:
	enum BoostMode
	{
		ChangeToSram = 0,
		ChangeToFlashAndSram = 1,
		ChangeToTemp = 2,
	};

	enum MsgMode
	{
		Nooutput_Mode = 0,
		Binary_Mode,
		NMEA_Mode,
		Binary_Output,
		Binary_Datalog,
		Binary_Pacific,
	};


	enum CmdErrorCode {
		Ack = 0,
		NACK,
		Timeout,
	};

	enum CmdExeMode {
		Display = 0,
		Return,
		NoWait,
	};

	enum DownloadMode {
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
		CustomerDownload,
		InternalLoaderSpecial,
		GpsdoMasterSlave,

		HostBasedDownload,
		HostBasedCmdOnly,
		HostBasedBinOnly,
		ParallelDownloadType0,
		ParallelDownloadType1,
		RomExternalDownload,
	} m_DownloadMode;

	int m_nDownloadBaudIdx;
	int m_nDownloadBufferIdx;
	CString m_strDownloadImage;
	CString m_strDownloadImage2;

	void Download();
	void SetBaudrate(int b);
	BOOL GetShowBinaryCmdData() { return m_bShowBinaryCmdData; }
	void BoostBaudrate(BOOL bRestore, BoostMode mode = ChangeToTemp, bool isForce = false);
	BOOL OpenDataLogFile(UINT nOpenFlags);
	MsgMode GetMsgType() 
	{ 
		return m_msgType; 
	};
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
	void SetInputMode(U08 i) { m_inputMode = i; }
	void ExecuteConfigureCommand(U08 *cmd, int size, LPCSTR msg, bool restoreConnect = true);
	void LogReadBatchControl();
	void GetEphms(U08 SV, U08 continues = FALSE);	
	void GetGlonassEphms(U08 SV, U08 continues = FALSE);
	void GetBeidouEphms(U08 SV, U08 continues = FALSE);

	bool SAVE_EPHEMRIS(U08* buff,U08 id);
	bool SaveEphemeris2(U08* buff, WORD id);
	void Refresh_EarthChart(CDC *earth_dc);
	//void Refresh_ScatterChart(CDC *scatter_dc);
	int GetComPort() { return m_comPort; }
	int GetBaudrate() { return m_baudrate; }
	int GetCustomerID()
	{ return m_customerID; }

private:
	BOOL m_bShowBinaryCmdData;
	int downloadTotalSize;
	int downloadProgress;
	MsgMode m_msgType;
	U08 m_restartMode;
	U08 m_inputMode;
	CString datalogFilename;
	CFile dataLogFile;

	enum DataLogType
	{
		FIXNONE = 0,
		FIXFULL = 1,
		FIXINC = 2,
		FIXPOI = 3,
		FIXMULTI = 4,
		FIXPOIMULTI = 0x0C,

		FIXFULL2 = 11,
		FIXINC2 = 12,
		FIXPOI2 = 13,
	};

	UINT GetBinFromResource(int baud);
	U08 PlRomNoAlloc(const CString& prom_path);
	U08 PlRomNoAlloc2(const CString& prom_path);
	U08 PlRomNoAllocV8(const CString& prom_path);
	bool FirmwareUpdate(const CString& strFwPath);
	int SendRomBuffer3(const U08* sData, int sDataSize, FILE *f, int fbinSize, 
						bool needSleep, CWnd* notifyWnd);
	bool DownloadLoader();
	UINT GetSrecFromResource(int baud);
	bool QueryPassword();
	void GetLoaderDownloadCmd(char* msg, int size);
	//Data Log functions
	bool DatalogReadAll(int start_id,int offset, U08 *datalog,long size,long *receive_count );
	bool VerifyDataLogBuffer(U08 *buff, U08 *datalog, U08 *ptr_last, int size, long *receive_count);
	bool DataLogReadOffsetCtrl(int start_id, int total_sector, int offset, U08 *buff, long size , long *receive_count);
	bool QueryDataLogBoundary(U16 *end, U16 *total);
	void VerifyDataLogFormat(U08 *datalog,long *size);
	DataLogType GetDataLogType(U16 word);
	//For Common Binary Clasases
public:
	CmdErrorCode ExcuteBinaryCommand(int cmdIdx, BinaryCommand* cmd, BinaryData* ackCmd, DWORD timeOut = 3000, bool silent = false);
	CmdErrorCode ExcuteBinaryCommandNoWait(int cmdIdx, BinaryCommand* cmd);
	CGPSDlg::CmdErrorCode GetBinaryResponse(BinaryData* ackCmd, U08 cAck, U08 cAckSub, DWORD timeOut, bool silent, bool noWaitAck = false);

	typedef CmdErrorCode (CGPSDlg::*QueryFunction)(CmdExeMode, void*);
	void GenericQuery(QueryFunction pfn);
	//Query Functions
	enum { DefaultTimeOut = 3000 };
	int m_nDefaultTimeout;

	CmdErrorCode QueryPositionRate(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDatum(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySoftwareVersionRomCode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySha1String(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryConstellationCapability(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryVersionExtension(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySoftwareVersionSystemCode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySoftwareCrcRomCode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySoftwareCrcSystemCode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryWaasStatus(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPositionPinning(CmdExeMode nMode, void* outputData);
	CmdErrorCode Query1ppsMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPowerMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPowerSavingParameters(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryV8PowerSavingParameters(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryV8PowerSavingParametersRom(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryProprietaryMessage(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryTiming(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDopMask(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryElevationAndCnrMask(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryAntennaDetection(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNoisePower(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDrInfo(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDrHwParameter(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssSelectionForNavigationSystem(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssKnumberSlotCnr(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySbas(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySagps(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryQzss(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNoisePowerControl(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryInterferenceDetectControl(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNmeaBinaryOutputDestination(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryParameterSearchEngineNumber(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryAgpsStatus(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDatalogLogStatus(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRegister(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryPositionFixNavigationMask(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryChannelDoppler(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNavigationMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNmeaIntervalV8(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryNmeaInterval2V8(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryEricssonInterval(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryClockOffset(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryRefTimeSyncToGpsTime(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySearchEngineSleepCriteria(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDatumIndex(CmdExeMode nMode, void* outputData);

	CmdErrorCode QueryUartPass(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoResetSlave(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoEnterRom(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoLeaveRom(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoEnterDownload(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoLeaveDownload(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoEnterDownloadHigh(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoEnterUart(CmdExeMode nMode, void* outputData);
	CmdErrorCode GpsdoLeaveUart(CmdExeMode nMode, void* outputData);

	CmdErrorCode QueryNavigationModeV8(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssBootStatus(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDrMultiHz(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssKnumberSlotCnr2(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssSelectionForNavigationSystem2(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssNmeaTalkId(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGnssNavSol(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryCustomerID(CmdExeMode nMode, void* outputData);
	CmdErrorCode Query1ppsFreqencyOutput(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryBinaryMeasurementDataOut(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySerialNumber(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryDgps(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySmoothMode(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryTimeStamping(CmdExeMode nMode, void* outputData);
	CmdErrorCode QueryGpsTime(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySignalDisturbanceStatus(CmdExeMode nMode, void* outputData);
	CmdErrorCode QuerySignalDisturbanceData(CmdExeMode nMode, void* outputData);

//	CmdErrorCode ConfigureGpsdoMasterSerialPortHigh(CmdExeMode nMode, void* outputData);
private:
	afx_msg void OnQueryPositionRate()
	{ GenericQuery(&CGPSDlg::QueryPositionRate); }
	afx_msg void OnQueryDatum()
	{ GenericQuery(&CGPSDlg::QueryDatum); }
	afx_msg void OnQuerySoftwareVersionRomCode()
	{ GenericQuery(&CGPSDlg::QuerySoftwareVersionRomCode); }
	afx_msg void OnQuerySha1String()
	{ GenericQuery(&CGPSDlg::QuerySha1String); }
	afx_msg void OnQueryConstellationCapability()
	{ GenericQuery(&CGPSDlg::QueryConstellationCapability); }
	afx_msg void OnQueryVersionExtension()
	{ GenericQuery(&CGPSDlg::QueryVersionExtension); }
	afx_msg void OnQuerySoftwareVersionSystemCode()
	{ GenericQuery(&CGPSDlg::QuerySoftwareVersionSystemCode); }
	afx_msg void OnQuerySoftwareCrcRomCode()
	{ GenericQuery(&CGPSDlg::QuerySoftwareCrcRomCode); }
	afx_msg void OnQuerySoftwareCrcSystemCode()	
	{ GenericQuery(&CGPSDlg::QuerySoftwareCrcSystemCode); }
	afx_msg void OnQueryWaasStatus()
	{ GenericQuery(&CGPSDlg::QueryWaasStatus); }
	afx_msg void OnQueryPositionPinning()
	{ GenericQuery(&CGPSDlg::QueryPositionPinning); }
	afx_msg void OnQuery1ppsMode()
	{ GenericQuery(&CGPSDlg::Query1ppsMode); }
	afx_msg void OnQueryPowerMode()
	{ GenericQuery(&CGPSDlg::QueryPowerMode); }
	afx_msg void OnQueryPowerSavingParameters()
	{ GenericQuery(&CGPSDlg::QueryPowerSavingParameters); }
	afx_msg void OnQueryV8PowerSavingParameters()
	{ GenericQuery(&CGPSDlg::QueryV8PowerSavingParameters); }
	afx_msg void OnQueryV8PowerSavingParametersRom()
	{ GenericQuery(&CGPSDlg::QueryV8PowerSavingParametersRom); }
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
	afx_msg void OnQueryDrHwParameter()
	{ GenericQuery(&CGPSDlg::QueryDrHwParameter); }
	afx_msg void OnQueryGnssSelectionForNavigationSystem()
	{ GenericQuery(&CGPSDlg::QueryGnssSelectionForNavigationSystem); }
	afx_msg void OnQueryGnssKnumberSlotCnr()
	{ GenericQuery(&CGPSDlg::QueryGnssKnumberSlotCnr); }
	afx_msg void OnQuerySbas()
	{ GenericQuery(&CGPSDlg::QuerySbas); }
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
	afx_msg void OnQueryAgpsStatus()
	{ GenericQuery(&CGPSDlg::QueryAgpsStatus); }
	afx_msg void OnQueryDatalogLogStatus()
	{ GenericQuery(&CGPSDlg::QueryDatalogLogStatus); }
	afx_msg void OnQueryPositionFixNavigationMask()
	{ GenericQuery(&CGPSDlg::QueryPositionFixNavigationMask); }
	afx_msg void OnQueryNavigationMode()
	{ GenericQuery(&CGPSDlg::QueryNavigationMode); }
	afx_msg void OnQueryNmeaIntervalV8()
#if (CUSTOMER_ID==0x0001)	//SWID customize
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

	afx_msg void OnQueryNavigationModeV8()
	{ GenericQuery(&CGPSDlg::QueryNavigationModeV8); }
	afx_msg void OnQueryGnssBootStatus()
	{ GenericQuery(&CGPSDlg::QueryGnssBootStatus); }
	afx_msg void OnQueryDrMultiHz()
	{ GenericQuery(&CGPSDlg::QueryDrMultiHz); }
	afx_msg void OnQueryGnssKnumberSlotCnr2()
	{ GenericQuery(&CGPSDlg::QueryGnssKnumberSlotCnr2); }
	afx_msg void OnQueryGnssSelectionForNavigationSystem2()
	{ GenericQuery(&CGPSDlg::QueryGnssSelectionForNavigationSystem2); }
	afx_msg void OnQueryGnssNmeaTalkId()
	{ GenericQuery(&CGPSDlg::QueryGnssNmeaTalkId); }
	afx_msg void OnQueryGnssNavSol()
	{ GenericQuery(&CGPSDlg::QueryGnssNavSol); }
	afx_msg void OnQueryCustomerID()
	{ GenericQuery(&CGPSDlg::QueryCustomerID); }
	afx_msg void OnQuery1ppsFreqencyOutput()
	{ GenericQuery(&CGPSDlg::Query1ppsFreqencyOutput); }
	afx_msg void OnQueryBinaryMeasurementDataOut()
	{ GenericQuery(&CGPSDlg::QueryBinaryMeasurementDataOut); }
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

//	U32 GetClockOffsetByRegister();
//	U08 query_clock_offset(S32 *clock_offset);
	void Show_EarthChart(CDC *dc);
	void DrawGnssSatellite(CDC* dc, int id, int centerX, int centerY);
	void DrawBdSatellite(CDC* dc, int id, int centerX, int centerY);
	void DrawGaSatellite(CDC* dc, int id, int centerX, int centerY);

	void parse_sti_03_message(const char *buff,int len) /* for timing module */;
	void parse_sti_04_001_message(const char *buff, int len) /* for timing module */;
	void parse_sti_message(const char *buff,int len);
	void parse_sti_0_message(const char *buff,int len) /* for timing module */;
	void parse_rtoem_message(const char *buff, int len);
	void parse_psti_50(const char *buff);
	void parse_sti_20_message(const char *buff,int len) /* for timing module */;
//	void Config_silab_baudrate(HANDLE *m_DeviceHandle);
//	void Config_silab_baudrate_flash(HANDLE *m_DeviceHandle);
	void DoCommonConfig(CCommonConfigDlg* dlg);
	//Functions for combain GPS / GNSS Viewer UI Layout.
	int CreateSubMenu(const HMENU hMenu, const MenuItemEntry* menuItemTable, LPCSTR pszSubMenuText);
	PrnType GetPrnType(int id);
	void RescaleDialog();
	void SwitchToConnectedStatus(BOOL bSwitch);
	void SetFactoryDefault(bool isReboot);
	void GetGPSStatus();
	void SetConnectTitle(bool isInConnect);
//Test Clock Offser
	U08 QueryChanelFreq(int chanel, U16 *prn, double *freq);
	U08 PredictClockOffset(double *clk_offset);
//End

	int m_comPort;
	int m_baudrate;
	WPARAM plugin_wParam;
	CString plugin_port_name;
	U16 m_customerId;

	CLabel m_wgs84_x,m_wgs84_y,m_wgs84_z;
	CLabel m_enu_e,m_enu_n,m_enu_u;
	CSnrBarChartGpsGlonass* gpsSnrBar;
//	CSnrBarChartGlonass* gnssSnrBar;
	CSnrBarChartBeidou* bdSnrBar;
	CSnrBarChartGalileo* gaSnrBar;
	CPic_Scatter* pic_scatter;
	CPic_Earth* pic_earth;
	CLabel m_fixed_status;

	CEdit m_odo_meter;
	CEdit m_gyro_data;
	CEdit m_backward_indicator;
	CStatic m_lbl_odo_meter;
	CStatic m_lbl_gyro_data;
	CStatic m_lbl_backward_indicator;

	BinaryData m_versionInfo;
	BinaryData m_bootInfo;

	bool m_firstDataIn;
	U32 m_customerID;
	int m_dataLogDecompressMode;

	DECLARE_MESSAGE_MAP()

public:
	void NmeaOutput(LPCSTR pt, int len);
	int m_lastNmeaToken;
	UINT usedEarth;
};
