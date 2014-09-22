#pragma once

#include "Utility.h"
#include <vector>
#include "matrix.h"
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

class CWaitReadLog;
struct GPGSA;
typedef matrix<float> Matrix;


#include "Registry.h"
#define LogDefaultName		"Response.log"
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
		if(reg.SetKey("Software\\GNSSViewer\\GPS", false))
		{
			reg.WriteInt("setting_earthBitmap", earthBitmap);
			reg.WriteInt("setting_delayBeforeBinsize", delayBeforeBinsize);
			reg.WriteInt("setting_boostBaudrateIndex", boostBaudIndex);
			reg.WriteInt("setting_autoQueryVersion", autoQueryVersion);
			reg.WriteInt("setting_boostEphemeris", boostEphemeris);
			reg.WriteInt("setting_checkNmeaError", checkNmeaError);
			reg.WriteInt("setting_responseLog", responseLog);
			reg.WriteString("setting_responseLogPath", responseLogPath);

			reg.WriteInt("setting_specifyCenter", specifyCenter);
			reg.WriteFloat("setting_scatterCenterLon", scatterCenterLon);
			reg.WriteFloat("setting_scatterCenterLat", scatterCenterLat);
		}	
	}

	void Load()
	{
		CRegistry reg;
		reg.SetRootKey(HKEY_CURRENT_USER);
		const double defaultCenterLon = 121.008756203;
		const double defaultCenterLat = 24.784893606;

		if(reg.SetKey("Software\\GNSSViewer\\GPS", true))
		{
			earthBitmap = reg.ReadInt("setting_earthBitmap", 0);
			delayBeforeBinsize = reg.ReadInt("setting_delayBeforeBinsize", 0);
			boostBaudIndex = reg.ReadInt("setting_boostBaudrateIndex", BAUDRATE_DEFAULT);
			autoQueryVersion = reg.ReadInt("setting_autoQueryVersion", TRUE);
			boostEphemeris = reg.ReadInt("setting_boostEphemeris", FALSE);
			checkNmeaError = reg.ReadInt("setting_checkNmeaError", SHOW_ERROR_NMEA_NOTIFY);
			responseLog = reg.ReadInt("setting_responseLog", FALSE);
			responseLogPath = reg.ReadString("setting_responseLogPath", LogDefaultName);

			specifyCenter = reg.ReadInt("setting_specifyCenter", FALSE);
			scatterCenterLon = reg.ReadFloat("setting_scatterCenterLon", defaultCenterLon);
			scatterCenterLat = reg.ReadFloat("setting_scatterCenterLat", defaultCenterLat);
		}
		else
		{
			earthBitmap = 0;
			delayBeforeBinsize = 0;
			boostBaudIndex = BAUDRATE_DEFAULT;
			autoQueryVersion = TRUE;
			boostEphemeris = FALSE;
			checkNmeaError = SHOW_ERROR_NMEA_NOTIFY;
			responseLog = FALSE;
			responseLogPath = LogDefaultName;
			specifyCenter = FALSE;
			scatterCenterLon = defaultCenterLon;
			scatterCenterLat = defaultCenterLat;
		}
		downloadTesting = FALSE;
	}

	int earthBitmap;
	int delayBeforeBinsize;
	int boostBaudIndex;
	BOOL autoQueryVersion;
	BOOL boostEphemeris;
	BOOL checkNmeaError;
	BOOL downloadTesting;
	BOOL responseLog;
	CString responseLogPath;
	BOOL specifyCenter;
	double scatterCenterLon;
	double scatterCenterLat;
};

typedef struct {
  S16 year;
  S16 month;
  S16 day;
  U08 hour;
  U08 minute;
  F32 sec;
} UTC_T;

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

typedef struct {
	double lat;
	double lon;
	double speed;
	double alt;
	UTC_T utc;
} LL;

typedef struct UTC_TIME_T {
	S16 year;
	S16 month;
	S16 day;
	S16 day_of_year;
	S16 hour;
	S16 minute;
	F32 sec;
} UtcTime;
extern Setting g_setting;

#if GNSS_VIEWER
 #define MAX_SATELLITE				32
#else
 #define MAX_SATELLITE				16
#endif
#define MyMaxPath		(MAX_PATH * 8)
#define BINMSG_ERROR                0

extern const float R2D;
extern const COLORREF g_panelBkColor;

extern U08 type;
extern U08 attribute;
extern U08 msgid;

extern U08  slgga;	
extern U08  slgsa;
extern U08  slgsv;
extern U08  slgll;
extern U08  slrmc;
extern U08  slvtg;
extern U08  slzda;
extern U08  slgns;

extern HANDLE g_connectEvent;
extern HANDLE g_closeEvent;
extern CWaitReadLog* WRL;
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
void  convert_gps_time_to_utc( S16 wn, D64 tow, UTC_T* utc_p );

extern const char* DatumList[];
extern const int DatumListSize;

U08 Cal_Checksum(U08* pt);
UINT16 CalCheckSum2(U08* pt);
void UtcConvertGpsToUtcTime(S16 wn, D64 tow, UtcTime *utc_time_p);
void UtcConvertUtcToGpsTime(const UtcTime *utc_time_p, S16 *wn_p, D64 *tow_p);

//void UTC_convert_gps_to_utc_time_by_default_parameters( S16 wn, D64 tow, UtcTime *utc_time_p );

