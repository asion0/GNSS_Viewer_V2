#pragma once

#include <vector>
#include <list>
#include "Utility.h"
#include "matrix.h"
#include "Registry.h"

using namespace math;
using namespace std;

#include "CxImage/ximage.h"
#if defined(_DEBUG)
 #pragma comment( lib, "CxImage/DebugLib/cximage.lib" )
 #pragma comment( lib, "CxImage/DebugLib/png.lib" )
 #pragma comment( lib, "CxImage/DebugLib/zlib.lib" )
#else
 #pragma comment( lib, "CxImage/ReleaseLib/cximage.lib" )
 #pragma comment( lib, "CxImage/ReleaseLib/png.lib" )
 #pragma comment( lib, "CxImage/ReleaseLib/zlib.lib" )
#endif
#pragma comment( lib, "Ws2_32.lib" )

class CWaitReadLog;
class CSerial;
struct GPGSA;
typedef matrix<float> Matrix;

#define MAKEU64(hi, lo) ((ULONGLONG(DWORD(hi) & 0xFFFFFFFF) << 32) | ULONGLONG(DWORD(lo) & 0xFFFFFFFF))
#define LogDefaultName		        "Response.log"
#define DefaultTimeout		        2000

#define MAX_WAIT_TIME		          INFINITE
#define TIME_OUT_MS               10000
#define TIME_OUT_QUICK_MS         1000
#define SCAN_TIME_OUT_MS          500
#define DOWNLOAD_TIME_OUT_LOOP    10
#define BUF_SIZE 4096
#define BOOTLOADER_SIZE		        0x10000

#define WGS84_RA    (6378137.0)                   // semi-major earth axis(ellipsoid equatorial radius)
#define WGS84_INV_F (298.257223563)               // inverse flattening of WGS-84
#define WGS84_F     (1.0/WGS84_INV_F)             // inverse flattening of WGS-84
#define WGS84_RB    (WGS84_RA*(1.0-WGS84_F))      // semi-major earth axis(ellipsoid polar radius)
#define WGS84_E2    (2.0*WGS84_F-WGS84_F*WGS84_F) // eccentricity squared: (RA*RA-RB*RB)/RA*RA
#define WGS84_E2P   (WGS84_E2/(1.0-WGS84_E2))     // eccentricity squared: (RA*RA-RB*RB)/RB*RB

#define COM_BUFFER_SIZE   (16 * 1024)              // 
#define KNOTS2KMHR        1.852F
#define MS2KMHR           3.6F
#define VIEWER_REG_PATH   "Software\\GNSSViewer\\GPS"

// Bit constants
#define BIT0        (0x01)
#define BIT1        (0x02)
#define BIT2        (0x04)
#define BIT3        (0x08)
#define BIT4        (0x10)
#define BIT5        (0x20)
#define BIT6        (0x40)
#define BIT7        (0x80)
#define BIT8        (0x0100)
#define BIT9        (0x0200)
#define BIT10       (0x0400)
#define BIT11       (0x0800)
#define BIT12       (0x1000)
#define BIT13       (0x2000)
#define BIT14       (0x4000)
#define BIT15       (0x8000)
#define BIT16       (0x00010000L)
#define BIT17       (0x00020000L)
#define BIT18       (0x00040000L)
#define BIT19       (0x00080000L)
#define BIT20       (0x00100000L)
#define BIT21       (0x00200000L)
#define BIT22       (0x00400000L)
#define BIT23       (0x00800000L)
#define BIT24       (0x01000000L)
#define BIT25       (0x02000000L)
#define BIT26       (0x04000000L)
#define BIT27       (0x08000000L)
#define BIT28       (0x10000000L)
#define BIT29       (0x20000000L)
#define BIT30       (0x40000000L)
#define BIT31       (0x80000000L)

struct Setting
{
	Setting()
	{
		Load();
	}

	void Save()
	{
		CRegistry reg;
		reg.SetRootKey(HKEY_CURRENT_USER);

		if(reg.SetKey(VIEWER_REG_PATH, false))
		{
			reg.WriteInt("comport", comPort);
			reg.WriteInt("baudrate", baudrate);
			reg.WriteInt("tcpip_type", tcpType);
			reg.WriteString("tcpip_host", tcpHost);
			reg.WriteInt("tcpip_port", tcpPort);
			reg.WriteString("firmware", mainFwPath);
			reg.WriteInt("setting_earthBitmap", earthBitmap);
			reg.WriteInt("configrfic_type", configRfIcType);

      CString txt;
      for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
      {
        txt.Format("configrfic_reg%d", i);
        reg.WriteInt(txt, configRfIcReg[i]);
      }
		}

		if(IS_DEBUG && reg.SetKey(VIEWER_REG_PATH, false))
		{
			reg.WriteInt("setting_delayBeforeBinsize", delayBeforeBinsize);
			reg.WriteInt("setting_boostBaudrateIndex", boostBaudIndex);
			reg.WriteInt("setting_autoQueryVersion", autoQueryVersion);
			reg.WriteInt("setting_boostEphemeris", boostEphemeris);
			reg.WriteInt("setting_checkNmeaError", checkNmeaError);
			reg.WriteInt("setting_checkTimeError", checkTimeError);
			reg.WriteInt("setting_responseLog", responseLog);
			reg.WriteString("setting_responseLogPath", responseLogPath);

			reg.WriteInt("setting_specifyCenter", specifyCenter);
			reg.WriteFloat("setting_scatterCenterLon", scatterCenterLon);
			reg.WriteFloat("setting_scatterCenterLat", scatterCenterLat);
			reg.WriteInt("setting_specifyCenterAlt", specifyCenterAlt);
			reg.WriteFloat("setting_scatterCenterAlt", scatterCenterAlt);
			reg.WriteInt("setting_defaultTimeout", defaultTimeout);
			reg.WriteInt("setting_scatterCount", scatterCount);
			reg.WriteInt("setting_downloadRomInternal", downloadRomInternal);
			reg.WriteInt("setting_downloadUseBinExternal", downloadUseBinExternal);
			reg.WriteInt("setting_showAllUnknownBinary", showAllUnknownBinary);
			reg.WriteInt("setting_autoAgpsAfterColdStart", autoAgpsAfterColdStart);
			reg.WriteInt("setting_rtkStatusChanged", rtkStatusChanged);
			reg.WriteInt("setting_weekNumberCycle", weekNumberCycle);
      
			reg.WriteInt("recentScatterCenterCount", recentScatterCenter.GetCount());
			for(int i = 0; i < recentScatterCenter.GetCount(); ++i)
			{
				CString key;
				key.Format("recentScatterCenter%d", i);
				reg.WriteString(key, recentScatterCenter[i]);
			}
		}	
	}

	void Load()
	{
		CRegistry reg;
		reg.SetRootKey(HKEY_CURRENT_USER);
		const double defaultCenterLon = 121.008756203;
		const double defaultCenterLat = 24.784893606;
		const double defaultCenterAlt = 100.0;

		if(IS_DEBUG && reg.SetKey(VIEWER_REG_PATH, true))
		{
			delayBeforeBinsize = reg.ReadInt("setting_delayBeforeBinsize", 0);
			boostBaudIndex = reg.ReadInt("setting_boostBaudrateIndex", BAUDRATE_DEFAULT);
			autoQueryVersion = reg.ReadInt("setting_autoQueryVersion", TRUE);
			boostEphemeris = reg.ReadInt("setting_boostEphemeris", FALSE);
			checkNmeaError = reg.ReadInt("setting_checkNmeaError", SHOW_ERROR_NMEA_NOTIFY);
			checkTimeError = reg.ReadInt("setting_checkTimeError", SHOW_ERROR_TIME_NOTIFY);
			responseLog = reg.ReadInt("setting_responseLog", FALSE);
			responseLogPath = reg.ReadString("setting_responseLogPath", LogDefaultName);
			specifyCenter = reg.ReadInt("setting_specifyCenter", FALSE);
			scatterCenterLon = reg.ReadFloat("setting_scatterCenterLon", defaultCenterLon);
			scatterCenterLat = reg.ReadFloat("setting_scatterCenterLat", defaultCenterLat);
			specifyCenterAlt = reg.ReadInt("setting_specifyCenterAlt", FALSE);
			scatterCenterAlt = reg.ReadFloat("setting_scatterCenterAlt", defaultCenterAlt);
			defaultTimeout = reg.ReadInt("setting_defaultTimeout", DefaultTimeout);
			scatterCount = reg.ReadInt("setting_scatterCount", MAX_SCATTER_COUNT);
			downloadRomInternal = reg.ReadInt("setting_downloadRomInternal", FALSE);
			downloadUseBinExternal = reg.ReadInt("setting_downloadUseBinExternal", FALSE);
			showAllUnknownBinary = reg.ReadInt("setting_showAllUnknownBinary", FALSE);
			autoAgpsAfterColdStart = reg.ReadInt("setting_autoAgpsAfterColdStart", FALSE);
			rtkStatusChanged = reg.ReadInt("setting_rtkStatusChanged", FALSE);
			weekNumberCycle = reg.ReadInt("setting_weekNumberCycle", 2);

			int recentCount = reg.ReadInt("recentScatterCenterCount", 0);
			for(int i = 0; i < recentCount; ++i)
			{
				CString key, data;
				key.Format("recentScatterCenter%d", i);
				data = reg.ReadString(key, "");
				recentScatterCenter.Add(data);
			}
		}
		else
		{
			delayBeforeBinsize = 0;
			boostBaudIndex = BAUDRATE_DEFAULT;
      if(CUSTOMER_ZENLANE_160808)
      {
			  autoQueryVersion = FALSE;
      }
      else
      {
			  autoQueryVersion = TRUE;
      }

			boostEphemeris = FALSE;
			checkNmeaError = SHOW_ERROR_NMEA_NOTIFY;
			checkTimeError = SHOW_ERROR_TIME_NOTIFY;
			responseLog = FALSE;
			responseLogPath = LogDefaultName;
			specifyCenter = FALSE;
			scatterCenterLon = defaultCenterLon;
			scatterCenterLat = defaultCenterLat;
			specifyCenterAlt = FALSE;
			scatterCenterAlt = defaultCenterAlt;
			defaultTimeout = DefaultTimeout;
			scatterCount = MAX_SCATTER_COUNT;
      downloadRomInternal = FALSE;
      downloadUseBinExternal = FALSE;
      showAllUnknownBinary = FALSE;
      autoAgpsAfterColdStart = FALSE;
      rtkStatusChanged = FALSE;
			weekNumberCycle = 2;
		}

		if(reg.SetKey(VIEWER_REG_PATH, true))
		{
			comPort = reg.ReadInt("comport", 0);
			baudrate = reg.ReadInt("baudrate", 1);
			tcpType = reg.ReadInt("tcpip_type", 0);
			tcpHost = reg.ReadString("tcpip_host", "");
			tcpPort = reg.ReadInt("tcpip_port", 20000);
			mainFwPath = reg.ReadString("firmware", "");
			earthBitmap = reg.ReadInt("setting_earthBitmap", 0);
			configRfIcType = reg.ReadInt("configrfic_type", 2);

      CString txt;
      for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
      {
        txt.Format("configrfic_reg%d", i);
			  configRfIcReg[i] = reg.ReadInt(txt, 0);
      }
		}
		else
		{
			comPort = 0;
			baudrate = 1;
      tcpType = 0;
			tcpHost = "";
			tcpPort = 20000;
			mainFwPath = "";
			earthBitmap = 0;
      configRfIcType = 0;
      CString txt;
      for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
      {
			  configRfIcReg[i] = 0;
      }
		}
		downloadTesting = FALSE;
		if(scatterCount <= 0)
		{
			scatterCount = MAX_SCATTER_COUNT;
		}
	}

	void AddRecentScatterCenter(LPCSTR r)
	{
		for(int i = 0; i < recentScatterCenter.GetCount(); ++i)
		{
			if(recentScatterCenter[i] == r)
			{
				recentScatterCenter.RemoveAt(i);
			}
		}
		if(recentScatterCenter.GetCount() > 9)
			recentScatterCenter.RemoveAt(0);
		recentScatterCenter.Add(r);
	}
  enum { RFIC_W_REG_SIZE = 23 };
  enum { RFIC_R_REG_SIZE = 3 };

	int BaudrateIndex(int b);
	int BaudrateValue(int i){ return BaudrateTable[i]; }
	int GetBaudrateIndex() { return baudrate; }
  int GetShortTimeout() { return (baudrate > 0) ? 1000 : 2000; }
  int GetLongTimeout() { return (baudrate > 0) ? 3000 : 5000; }
	int GetBaudrate() { return BaudrateTable[baudrate]; }
	void SetBaudrateIndex(int b) { baudrate = b; }
	int SetBaudrateByValue(int b);
	int GetComPortIndex() { return comPort - 1; }
	int GetComPort() { return comPort; }
  int GetTcpipType() { return tcpType; }
	LPCSTR GetTcpipHost() { return tcpHost; }	
  int GetTcpipPort() { return tcpPort; }
	void SetComPortIndex(int c) { comPort = c + 1; }
	void SetComPort(int c) { comPort = c; }
	void SetTcpipType(int p) { tcpType = p; }
	void SetTcpipHost(LPCSTR h) { tcpHost = h; }
	void SetTcpipPort(int p) { tcpPort = p; }
  void InitBaudrateCombo(CComboBox* c, bool addSPI = false, bool addI2C = false);
	int GetBaudrateTableSize();
  bool IsValidBaudrateIndex(int i) { return (i >= GetBaudrateTableSize()) ? false : (0 != BaudrateTable[i]); };
	//General use
	CString mainFwPath;
	int earthBitmap;

	//Internal Use only
	int delayBeforeBinsize;
	int boostBaudIndex;
	BOOL autoQueryVersion;
	BOOL boostEphemeris;
	BOOL checkNmeaError;
	BOOL checkTimeError;
	BOOL downloadTesting;
	BOOL downloadRomInternal;
  BOOL downloadUseBinExternal;
  BOOL showAllUnknownBinary;
  BOOL autoAgpsAfterColdStart;
  BOOL rtkStatusChanged;
	int weekNumberCycle;

	BOOL responseLog;
	CString responseLogPath;
	BOOL specifyCenter;
	double scatterCenterLon;
	double scatterCenterLat;
	BOOL specifyCenterAlt;
	double scatterCenterAlt;
	int defaultTimeout;
	int scatterCount;
	CStringArray recentScatterCenter;
	U08 configRfIcType;
	U32 configRfIcReg[RFIC_W_REG_SIZE];

protected:
	static int BaudrateTable[];
	//General use
	int comPort;
	int baudrate;
  int tcpType;
  CString tcpHost;
  int tcpPort;
};

typedef struct UTC_TIME_T {
	S16 year;
	S16 month;
	S16 day;
	S16 day_of_year;
	S16 hour;
	S16 minute;
	F32 sec;
} UtcTime;

struct POS_T {
  D64 px;
  D64 py;
  D64 pz;
};

struct LLA_T {
	D64 lat;
	D64 lon;
	F32 alt;
};

struct LL {
	double lat;
	double lon;
	double speed;
	double alt;
	UtcTime utc;
};

enum QualityMode {
	Uninitial = 0,
	Unlocated,
	EstimatedMode,
	DgpsMode,
	PpsMode,
	PositionFix2d,
	PositionFix3d,

	SurveyIn,
	StaticMode,
	DataNotValid,
	AutonomousMode,
	DgpsMode2,
	FixRTK,
	FloatRTK,
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
	wlf_error6,
	wlf_error7,
	//wlf_resend,
	//wlf_reset,
	//wlf_resendbin,
	wlf_timeout,
	//for Loader debug
	//wlf_error41,
	//wlf_error42,
	//wlf_error43,
	//wlf_Ready,
	//wlf_Ready1,
	//wlf_Ready2,
};

extern Setting g_setting;

#define MAX_SATELLITE				32
#define MyMaxPath		(MAX_PATH * 8)
#define BINMSG_ERROR                0

extern const double R2D;
extern const COLORREF g_panelBkColor;

extern HANDLE g_connectEvent;
extern HANDLE g_closeEvent;
extern CWaitReadLog* g_waitReadDialog;
extern HANDLE	waitlog;
extern const S16 DefaultLeapSeconds;

extern double pos_lat, pos_lon;
extern int m_almanac_no;
extern CString m_almanac_filename;

class CMonitor_1PPS;
extern CMonitor_1PPS *dia_monitor_1pps;
//extern bool g_doesNotChangeBaudrate;
extern int FlashBytes[];

U32 ConvertCharToU32(const char *src);
UINT WaitLogRead(LPVOID pParam);
bool IsFixed(U16 gga_indicator);
bool CheckInUse(int id, GPGSA* gsa);
double Deg2Rad(double degree);
double Rad2Deg(double rad);
float FixedPointToSingle(U32 FixedPointValue, int NumberOfFractionalBits);
void COO_geodetic_to_cartesian( const LLA_T* lla_p, POS_T* xyz_p );
//void  convert_gps_time_to_utc( S16 wn, D64 tow, UTC_T* utc_p );

extern const char* DatumList[];
extern const int DatumListSize;

U08 GetCheckSum(const U08* pt, int len);
U08 Cal_Checksum(U08* pt);
UINT16 CalCheckSum2(U08* pt);
void UtcConvertGpsToUtcTime(S16 wn, D64 tow, UtcTime *utc_time_p);
void UtcConvertUtcToGpsTime(const UtcTime *utc_time_p, S16 *wn_p, D64 *tow_p);
QualityMode GetGnssQualityMode(U32 qualityIndicator, U08 gpMode = 0, U08 glMode = 0, U08 gaMode = 0, U08 bdMode = 0, U08 giMode = 0);
//QualityMode GetGnssQualityMode(const U32 qualityIndicator, const U08 gpMode = 0, const U08 glMode = 0, const  gaMode = 0, const U08 bdMode = 0);
D64 ConvertLeonDouble(const U08* ptr);
F32 ConvertLeonFloat(const U08* ptr);
U16 ConvertLeonU16(const U08* ptr);
U16 ConvertLeU16(const U08* ptr);
S16 ConvertLeonS16(const U08* ptr);
U32 ConvertLeonU32(const U08* ptr);
S32 ConvertLeonS32(const U08* ptr);
U32 ConvertLeU32(const U08* ptr);
S64 ConvertLeonU64(const U08* ptr);
S64 ConvertLeonS64(const U08* ptr);
WlfResult WaitingLoaderFeedback(CSerial* serial, int TimeoutLimit, CWnd* msgWnd);

bool ReadOneLineInFile(CFile& f, char* buffer, int size);
bool PreprocessInputLine(U08 *buf, int& bufLen);
bool IsPrintable(const char c);
bool AllPrintable(const char* buffer, int len);
void GetBitData(U08 buff[], int pos, int len, U08* data, int dataLen);
void GetByteDataFromLE(U08 buff[], int pos, int len, U08* data, int dataLen);
void GetByteDataFromBE(U08 buff[], int pos, int len, U08* data, int dataLen);
void ConvertInt38Sign(S64* d);
int GetBitFlagCounts(U08* d, int size);
void CooCartesianToGeodetic(const double x, const double y, const double z, double &lat, double &lon, float &alt);
void CooCartesianToGeodetic(const POS_T* xyz_p, LLA_T* lla_p);
void ConvertEcefToLonLatAlt(D64 ecefX, D64 ecefY, D64 ecefZ, D64& latitude, D64& longitude, D64& altitude);

template <class T>
void DisplayStatic(CDialog* pDlg, UINT uid, LPCSTR format, T data)
{
  CString txt;
  txt.Format(format, data);
  pDlg->GetDlgItem(uid)->SetWindowText(txt);
}

U16 GetCrc16(const void *buf, U32 len);
LPCSTR GetU16HexString(const U16 d);
LPCSTR GetBinString(const U32 d, int len);

