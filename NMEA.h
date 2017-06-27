#pragma once
#include "stdafx.h"
#include "float.h"

enum NmeaType
{
	MSG_Unknown = 0,

	MSG_GGA,

	MSG_GPGSA,
	MSG_GLGSA,
	MSG_BDGSA,
	MSG_GAGSA,
	MSG_GNGSA,
	MSG_GIGSA,

	MSG_GPGSV,
  MSG_GPGSV2,
	MSG_GLGSV,
	MSG_BDGSV,
	MSG_BDGSV2,
	MSG_GAGSV,
	MSG_GNGSV,
	MSG_GIGSV,

	MSG_RMC,
	MSG_GLL,
	MSG_ZDA,
	MSG_GNS,
	MSG_VTG,
	MSG_STI,

	MSG_ERROR,
	MSG_REBOOT,
};

typedef struct GPGGA
{
	U16     Hour;
	U16     Min;
	F32     Sec;
	D64     Latitude;
	U08     Latitude_N_S;
	D64     Longitude;
	U08     Longitude_E_W;
  U16     GPSQualityIndicator;
	U16     NumsOfSatellites;
	F32     HDOP;
	F32     Altitude;
	U08     Altitude_meters;
  F32     GeoidalSeparation;
	U08     GeoidalSeparation_meters;
	F32     AgeDGPSData;
	U16     DiffRefStaID;
}GPGGA, *pGPGGA, &rGPGGA;

typedef struct GPGLL
{
	D64     Latitude;
	U08     Latitude_N_S;
	D64     Longitude;
	U08     Longitude_E_W;
	U16     Hour;
	U16     Min;
	F32     Sec;
	U08     Status;
	U08     ModeIndicator;
}GPGLL, *pGPGLL, &rGPGLL;

typedef struct PSTI004001
{
	BOOL	Valide;
	D64     Pitch;
	D64     Roll;
	D64     Yaw;
	U32     Pressure;
	D64     Temperature;
}PSTI004001, *pPSTI004001, &rPSTI004001;

typedef struct GPGSA
{
	U08     Auto_Manu_Mode;//A=Automatic,M=Manual
	U16     Mode; //1=fix not available,2=2D,3=3D
	U16     SatelliteID[MAX_SATELLITE];
	F32     PDOP;
	F32     HDOP;
  F32     VDOP;
  U16     SystemId;
}GPGSA, *pGPGSA, &rGPGSA;

typedef struct GPVTG
{	
	F32     TrueCourse;     //true course over ground in degrees(000.0 ~ 359.9)	
	F32     MagneticCourse; //Magnetic course over ground in degrees(000.0 ~ 359.9)
	F32     SpeedKnots;
	F32     SpeedKmPerHur;
	U08     Mode;           //N=not valid, A=Auto, D=Diff, E=Estimated, M=Manual, S=Simulator
}GPVTG, *pGPVTG, &rGPVTG;

typedef struct Satellite
{	
	U16     SatelliteID;
	U16     Elevation;
	U16     Azimuth;
#if (FLOAT_SNR)
  F32     SNR;
#else
	U16     SNR;
#endif
}Satellite, *pSatellite, &rSatellite;

typedef struct GPGSV
{	
	U16     NumOfMessage;
  U16     SequenceNum;
  U16     NumOfSate;
	U08     have_gps;
	U08     have_gnss;
	Satellite    sates[4];
}GPGSV, *pGPGSV, &rGPGSV;

typedef struct GPRMC
{	
	U16     Hour;
	U16     Min;
	F32     Sec;
	U08     Status;
	D64     Latitude;
	U08     Latitude_N_S;
	D64     Longitude;
	U08     Longitude_E_W;
	F32     SpeedKnots;
	F32     TrueCourse;
	U16     Year;
	U16     Month;
	U16     Day;
	F32     MagVar;
	U08     MagVarDir;
	U08     ModeIndicator;
}GPRMC, *pGPRMC, &rGPRMC;

typedef struct GPZDA
{	
	U16     Hour;
	U16     Min;
	F32     Sec;
	U16     Day;
	U16     Month;
	U16     Year;
	U16     LocalZoneHours;
	U16     LocaZoneMinutes;
} GPZDA, *pGPZDA, &rGPZDA;

enum { INVALIDATE_SNR = 999 };
enum { MaxNmeaParam = 30 };
//#define UWM_GNSS_DATA_UPDATE	(WM_USER + 0x1379)
class GnssData
{
public:
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
	};

	enum UpdateNotify
	{
		UpdateData = 1 << 0,
		UpdateTime = 1 << 1,
		UpdateLatitude = 1 << 2,
		UpdateLongitude = 1 << 3,
		UpdateAltitude = 1 << 4,
		UpdateDirection = 1 << 5,
		UpdateSpeed = 1 << 6,
		UpdateHdop = 1 << 7,
		UpdateGpSate = 1 << 8,
		UpdateGlSate = 1 << 9,
		UpdateBdSate = 1 << 10,
		UpdateGaSate = 1 << 11,
		UpdateFixMode = 1 << 12,
	};

	GnssData()
	{
		notifyWnd = NULL;
		m_updateEvent = 0;
		Init();
	}

	void Notify(UINT_PTR f)
	{
		if(notifyWnd && m_updateEvent)
		{
			::PostMessage(notifyWnd, m_updateEvent, f, 0);
		}
	}

	void SetNotify(HWND h) { notifyWnd = h; }

	UINT RegisterEventMessage()
	{
		m_updateEvent = ::RegisterWindowMessage("GnssDataUpdate-Event");
		return m_updateEvent;
	}

	//Getter
	U16 GetYear() const { return year; }
	U16 GetMonth() const { return month; }
	U16 GetDay() const { return day; }
	U16 GetHour() const { return hour; }
	U16 GetMinute() const { return minute; }
	D64 GetSecond() const { return second; }
	D64 GetLatitude() const { return latitude; }
	U08 GetLatitudeNS() const { return latitudeNS; }
	D64 GetLongitude() const { return longitude; }
	U08 GetLongitudeEW() const { return longitudeEW; }
	D64 GetAltitude() const { return altitude; }
	D64 GetDirection() const { return trueCourse; }
	D64 GetSpeed() const { return speedKnots; }
	D64 GetHdop() const { return hdop; }
	QualityMode GetFixMode() const { return fixMode; }

	//Setter
	void SetDate(U16 y, U16 m, U16 d) 
	{ 
		GnssDataCs.Lock();
		year = y; 
		month = m; 
		day = d; 
		GnssDataCs.Unlock();
	}
	void SetTime(U16 h, U16 m, D64 s) 
	{ 
		GnssDataCs.Lock();
		hour = h; 
		minute = m; 
		second = s; 
		GnssDataCs.Unlock();
	}
	void SetLatitude(D64 lat, U08 latNS)
	{ 
		GnssDataCs.Lock();
		latitude = lat; 
		latitudeNS = latNS; 
		GnssDataCs.Unlock();
	}
	void SetLongitude(D64 lon, U08 lonEW)
	{ 
		GnssDataCs.Lock();
		longitude = lon; 
		longitudeEW = lonEW; 
		GnssDataCs.Unlock();
	}	
	void SetAltitude(D64 alt)
	{ 
		GnssDataCs.Lock();
		altitude = alt; 
		GnssDataCs.Unlock();
	}	
	void SetDirection(D64 d)
	{ 
		GnssDataCs.Lock();
		trueCourse = d; 
		GnssDataCs.Unlock();
	}
	void SetSpeed(D64 s)
	{ 
		GnssDataCs.Lock();
		speedKnots = s; 
		GnssDataCs.Unlock();
	}
	void SetHdop(D64 h)
	{ 
		GnssDataCs.Lock();
		hdop = h; 
		GnssDataCs.Unlock();
	}
	//void SetFixMode(U16 ggaQtyInd, U16 gpGsaMode, U16 glGsaMode, U16 bdGsaMode, U08 rmcModeInd);

	void ClearData() { Init(); }
protected:
	HWND	notifyWnd;
	//For date
	U16     year;
	U16     month;
	U16     day;
	//For time
	U16     hour;
	U16     minute;
	D64     second;
	//Latitude
	D64     latitude;
	U08     latitudeNS;
	//Longitude
	D64     longitude;
	U08     longitudeEW;
	//Altitude
	D64     altitude;
	//Direction
	D64     trueCourse;
	//Speed
	D64     speedKnots;
	//Hdop
	D64     hdop;
	//Fix Mode
	QualityMode fixMode;

	//Satellite sataus
	Satellite gpSate[MAX_SATELLITE];
	Satellite glSate[MAX_SATELLITE];
	Satellite bdSate[MAX_SATELLITE];
	Satellite gaSate[MAX_SATELLITE];

	void Init()
	{
		year = SHRT_MAX;
		month = SHRT_MAX;
		day = SHRT_MAX;
		//For time
		hour = SHRT_MAX;
		minute = SHRT_MAX;
		second = DBL_MAX;
		//Latitude
		latitude = DBL_MAX;
		latitudeNS = ' ';
		//Longitude
		longitude = DBL_MAX;
		longitudeEW = ' ';
		//Altitude
		altitude = DBL_MAX;
		//Direction
		trueCourse = DBL_MAX;
		//Speed
		speedKnots = DBL_MAX;
		//Hdop
		hdop = DBL_MAX;
		//Fix Mode
		fixMode = Uninitial;
		//Satellite sataus
		for(int i=0; i<MAX_SATELLITE; ++i)
		{
			gpSate[i].Azimuth = 0;
			gpSate[i].Elevation = 0;
			gpSate[i].SNR = SHRT_MAX;
			gpSate[i].SatelliteID = SHRT_MAX;
			glSate[i].Azimuth = 0;
			glSate[i].Elevation = 0;
			glSate[i].SNR = SHRT_MAX;
			glSate[i].SatelliteID = SHRT_MAX;
			bdSate[i].Azimuth = 0;
			bdSate[i].Elevation = 0;
			bdSate[i].SNR = SHRT_MAX;
			bdSate[i].SatelliteID = SHRT_MAX;
			gaSate[i].Azimuth = 0;
			gaSate[i].Elevation = 0;
			gaSate[i].SNR = SHRT_MAX;
			gaSate[i].SatelliteID = SHRT_MAX;
		}
	}

private:
	UINT m_updateEvent;
	CCriticalSection GnssDataCs;
};

class NMEA
{
public:
	enum GNSS_System
	{
		GsUnknown,
		Gps,
		Glonass,
		Beidou,
		Galileo,
	};

	enum NMEA_Type
	{
		NtUnknown,
		PureGP,
		MixGP,
		GN,
	};

protected:
	static int LSB(char lsb);
	static int MSB(char msb);
	static int ParamInt(LPCSTR p, int first, int second, int defaultValue);
	static char ParamChar(LPCSTR p, int first, int second, char defaultValue);
	static float ParamFloat(LPCSTR p, int first, int second, float defaultValue);
	static double ParamDouble(LPCSTR p, int first, int second, float defaultValue);
	static int ScanDot(LPCSTR pt, int offset, int dot[MaxNmeaParam]);
	static bool VarifyNmeaChecksum(LPCSTR pt, int len);

	static bool GLLProc(GPGLL& rgll, LPCSTR pt, int len);
	static bool GGAProc(GPGGA& rgga, LPCSTR pt, int len);
	static bool GNSProc(GPGGA& rgga, LPCSTR pt, int len);
	static bool GSVProc(GPGSV& rgsv, LPCSTR pt, int len);
	static bool GSAProc(GPGSA& rgsa, LPCSTR pt, int len);
	static bool RMCProc(GPRMC& rrmc, LPCSTR pt, int len);
	static bool ZDAProc(GPZDA& rzda, LPCSTR pt, int len);
	static bool VTGProc(GPVTG& rvtg, LPCSTR pt, int len);

	static bool firstGsaIn;
public:
	static GnssData gnssData;
	static NMEA_Type nmeaType;

	static GNSS_System GetGNSSSystem(int prn);
	static GNSS_System GetGNSSSystem0(int prn);
	static GNSS_System GetGNSSSystem1(int prn);
	static GNSS_System GetGNSSSystem2(int prn);
	static GNSS_System GetGNSSSystem3(int prn);

	static int TrimTail(const char* buffer, int offset);
	static NmeaType MessageType(LPCSTR pt, int len);
	static bool GetFirstGsaIn() { return firstGsaIn; }
  
  int ClearSatellitesInRange(GNSS_System gs, int prnStart, int prnEnd);
	Satellite satellites_gps[MAX_SATELLITE];
	Satellite satellites_gnss[MAX_SATELLITE];
	Satellite satellites_bd[MAX_SATELLITE];
	Satellite satellites_ga[MAX_SATELLITE];

#if(SUPPORT_BDL2_GSV2)
	Satellite satellites2_gps[MAX_SATELLITE];
	Satellite satellites2_bd[MAX_SATELLITE];
	void ShowGPGSV2msg(GPGSV&, const char*, int);
	void ShowBDGSV2msg(GPGSV&, const char*, int);
#endif

	void ShowGPGLLmsg(GPGLL&, const char* ,int);
	void ShowGPZDAmsg(GPZDA&, const char* ,int);
	void ShowGPGGAmsg(GPGGA&, const char* ,int);
	void ShowGNSmsg(GPGGA&, const char* ,int);
	void ShowGPRMCmsg(GPRMC&, const char* ,int);
	void ShowGPVTGmsg(GPVTG&, const char* ,int);
	void ShowGPGSVmsg(GPGSV&, const char* ,int);
	void ShowGPGSVmsg2(GPGSV&, GPGSV&, GPGSV&, GPGSV&, const char* ,int);
	void ShowGLGSVmsg(GPGSV&, const char*, int);
	void ShowGIGSVmsg(GPGSV&, const char*, int);
	void ShowGAGSVmsg(GPGSV&, const char*, int);
	void ShowGNGSVmsg(GPGSV&, GPGSV&, GPGSV&, GPGSV&, const char*, int);
	void ShowBDGSVmsg(GPGSV&, const char*, int);
	void ShowGLGSAmsg(GPGSA& msg_glgsa,const char* pt,int offset);
	void ShowGIGSAmsg(GPGSA& msg_glgsa,const char* pt,int offset);
	void ShowGPGSAmsg(GPGSA& msg_gpgsa,const char* pt,int offset);
	//void ShowGPGSAmsg(GPGSA& msg_gpgsa, GPGSA& msg_glgsa, GPGSA& msg_bdgsa, const char* pt, int offset);
	void ShowGNGSAmsg(GPGSA& msg_gpgsa, GPGSA& msg_glgsa, GPGSA& msg_bdgsa, GPGSA& msg_gagsa, const char* pt, int offset);
	void ShowBDGSAmsg(GPGSA& msg_bdgsa, const char* pt, int offset);
	void ShowGAGSAmsg(GPGSA& msg_gagsa, const char* pt, int offset);
	void ClearSatellites();
};
