#pragma once
#include "stdafx.h"
#include "float.h"
#include <algorithm>    // std::find
#include <vector>       // std::vector

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
	MSG_QZGSA,

	MSG_GPGSV,
  MSG_GPGSV2,
	MSG_GLGSV,
	MSG_BDGSV,
	MSG_BDGSV2,
	MSG_GAGSV,
	MSG_GNGSV,
	MSG_GIGSV,
	MSG_QZGSV,

	MSG_RMC,
	MSG_GLL,
	MSG_ZDA,
	MSG_GNS,
	MSG_VTG,
	MSG_STI,
  MSG_PIRNSF,

	MSG_ERROR,
	MSG_REBOOT,
};

#define NonUseValue (-1)
//enum { NonUseValue = -1 };
enum { MaxNmeaParam = 30 };
enum { MaxSnrSize = 16 };
//enum { NonUsePrn = SHRT_MAX };

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
} GPVTG, *pGPVTG, &rGPVTG;

extern bool IsValidateValue(int snr);
extern bool IsValidateValue(D64 snr);

class SnrTable
{
public:
  SnrTable() {}
  SnrTable(F32 s) { SetSnr(0, s, NonUseValue); }
  SnrTable(int sg, F32 sn) { SetSnr(sg, sn, NonUseValue); }
  SnrTable(int sg, F32 sn, U16 ci) { SetSnr(sg, sn, ci); }
  SnrTable(const SnrTable &s) { Copy(s); }  // Copy constructor
  SnrTable& operator = (const SnrTable &s)
  {
    Clear();
    Copy(s);
    return *this;
  } 
  virtual ~SnrTable(void) {}
  enum { SnrPairSize = 5 };

  bool SetSnr(int sg, F32 sn, U16 ci) 
  {
    for(int i = 0; i < SnrPairSize; ++i)
    {
      if(snrData[i].sig == 0 && sg != 0 && snrData[i].snr == 0)
      { //Replace sig 0 by others value
        snrData[i].sig = sg;
        snrData[i].snr = sn;
        snrData[i].chInd = ci;
        return true;
      }
      else if(snrData[i].sig == sg)
      { //Replace exists
        snrData[i].snr = sn;
        snrData[i].chInd = ci;
        return true;
      }
      else if(snrData[i].sig == NonUseValue)
      { //Add new
        if(i > 0 && sg == 0)
        { //Do not add type 0 signal when already have others signal type
          return true;
        }
        snrData[i].sig = sg;
        snrData[i].snr = sn;
        snrData[i].chInd = ci;
        return false;
      }
    }
    return false;
  }

  F32 GetSnr(const int sg) const 
  {
    for(int i = 0; i < SnrPairSize; ++i)
    {
      if(snrData[i].sig == sg)
      {
        return snrData[i].snr;
      }
      else if(snrData[i].sig == NonUseValue)
      {
        return NonUseValue;
      }
    }
    return NonUseValue;
  }

  U16 GetChInd(const int sg) const 
  {
    for(int i = 0; i < SnrPairSize; ++i)
    {
      if(snrData[i].sig == sg)
      {
        if(snrData[i].chInd == (U16)NonUseValue)
          return 0;
        else
          return snrData[i].chInd;
      }
      else if(snrData[i].sig == NonUseValue)
      {
        return 0;
      }
    }
    return 0;
  }

  void Merge(const SnrTable& s)
  {
    for(int i = 0; i < SnrPairSize; ++i)
    {
      if(s.snrData[i].sig != NonUseValue)
      {
        SetSnr(s.snrData[i].sig, s.snrData[i].snr, s.snrData[i].chInd);
      }
      else
      {
        break;
      }
    }
  }

  void Clear() 
  {
    for(int i = 0; i < SnrPairSize; ++i)
    {
      snrData[i].sig = NonUseValue;
      snrData[i].snr = NonUseValue;
      snrData[i].chInd = NonUseValue;
    }
  }

protected:
  struct SnrPair
  {
    SnrPair() { sig = NonUseValue; snr = NonUseValue; chInd = NonUseValue; }
    S16 sig;
    F32 snr;
    U16 chInd;
  };
  SnrPair snrData[SnrPairSize];

  void Copy(const SnrTable& s)
  {
    for(int i = 0; i < SnrPairSize; ++i)
    {
      snrData[i] = s.snrData[i];
    }
  }
};

struct Satellite
{	
public:
  Satellite() { Clear(); }
  Satellite(const Satellite &s) // Copy constructor 
  {
    Clear();
    prn = s.prn; 
    ele = s.ele; 
    azi = s.azi; 
    snr = s.snr; 
  }
  Satellite& operator = (const Satellite &s)
  {
    Clear();
    prn = s.prn; 
    ele = s.ele; 
    azi = s.azi; 
    snr = s.snr; 
    return *this;
  } 
  virtual ~Satellite(void) {}

  void Set(U16 p, U16 e, U16 a, const SnrTable& s)
  {
    prn = p;
    if(e != (U16)NonUseValue && ele == 0)
    {
      ele = e;
    }
    if(a != (U16)NonUseValue && azi == 0)
    {
       azi = a;
    }    
    snr.Merge(s);
  }

  void Set(U16 p, U16 e, U16 a, int sigId, F32 s)
  {
    prn = p;
    ele = e;
    azi = a;
    //chInd = i;
    snr.SetSnr(sigId, s, NonUseValue);
  }

  void MergeSnr(const SnrTable& s) 
  { 
    snr.Merge(s); 
  }

  void Clear()
  {
    prn = NonUseValue;
    ele = azi = 0;
    snr.Clear();
  }

  void ClearSnr()
  {
    snr.Clear();
  }

  S16 GetPrn() const { return prn; }
  S16 GetEle() const { return ele; }
  S16 GetAzi() const { return azi; }
  //U16 GetChInd() const { return chInd; }
  const SnrTable& GetSnrTable() const { return snr; }
  bool IsInUsePrn() const { return prn != (U16)NonUseValue; }

protected:
  U16 prn;
  U16 ele;
  U16 azi;
  //U16 chInd;
  SnrTable snr;
};

class Satellites
{
public:
  Satellites(void) { Init(); }
  Satellites(const Satellites &ss) 
  {
    Init();
    index = ss.index; 
    inOrder = ss.inOrder; 
    inOrder = ss.inOrder; 
    signal = ss.signal; 
    for(int i = 0; i < MAX_SATELLITE; ++i)
    {
      sate[i] = ss.sate[i];
    }
  } 

  Satellites& operator = (const Satellites &ss)
  {
    index = ss.index; 
    inOrder= ss.inOrder; 
    inOrder= ss.inOrder; 
    signal= ss.signal; 
    for(int i = 0; i < MAX_SATELLITE; ++i)
    {
      sate[i] = ss.sate[i];
    }
    return *this;
  }
  virtual ~Satellites(void) {}

  void Clear() { Init(); }
  void SetSate(const Satellite& s)
  {
    SetSate2(s.GetPrn(), s.GetEle(), s.GetAzi(), s.GetSnrTable());
  }

  void SetSate2(int prn, int ele, int azi, const SnrTable& snr)
  {
    int idx = GetPrnIndex(prn);
    if(idx == -1)
    {
      idx = index++;
      inOrder = false;
    }
    sate[idx].Set(prn, ele, azi, snr);
  }

  void SetSate(int prn, int ele, int azi, const SnrTable& snr)
  {
    int idx = GetPrnIndex(prn);
    if(idx == -1)
    {
      idx = index++;
      inOrder = false;
    }
    sate[idx].Set(prn, ele, azi, snr);
  }

  int GetSateCount() { return index; }
  const Satellite* GetSateIndex(int idx) { return &sate[idx]; }

  void Sort()
  {
    if(inOrder)
    {
      return;
    }

    Satellite temp;
    for(int i = 0; i < index; ++i)
    {
      for(int j = i + 1; j < index; ++j)
      {
        if(sate[i].GetPrn() > sate[j].GetPrn())
        {
          temp = sate[i];
          sate[i] = sate[j];
          sate[j] = temp;
        }
      }
    }
    inOrder = true;
  }

  void ClearPrnInRange(int prnStart, int prnEnd)
  {
    int i, j;
    for(i = 0; i < index; ++i)
    {
      if(sate[i].GetPrn() >= prnStart && sate[i].GetPrn() <= prnEnd)
      {
        sate[i].Clear();
      }
    }

    int pos = -1;
    for(i = 0; i < index; ++i)
    {
      //if(sate[i].prn == 0)
      if(!sate[i].IsInUsePrn())
      {
        for(j = i; j < index; ++j)
        {
          //if(sate[j].prn != 0)
          if(sate[j].IsInUsePrn())
          {
            sate[i] = sate[j];
            //sate[j].prn = 0;
            sate[j].Clear();
            break;
          }
        }
        if(j == index)
        {
          break;
        }
      }
    }	
    index = i;
  }
  void ClearSnr() 
  {
    for(int i = 0; i < index; ++i)
    {
      sate[i].ClearSnr();
    }
    signal.clear();
  }

  int GetSnrIdSize()
  {
    return signal.size();
  }

  int GetSnrIdList(std::vector<int>& v)
  {
    v = signal;
    return v.size();
  }

  int GetSnrSigId(U16 idx)
  {
    if(signal.size() <= idx)
    {
      return NonUseValue;
    }
    return signal[idx];
  }

  bool AddSnrSigId(int sig)
  {
    std::vector<int>::iterator i;
    if(std::find(signal.begin(), signal.end(), sig) == signal.end())
    {
      signal.push_back(sig);
      return true;
    }
    return false;
  }
protected:
  Satellite sate[MAX_SATELLITE];
  std::vector<int> signal;
  int index;
  bool inOrder;

  void Init() 
  { 
    for(int i = 0; i < MAX_SATELLITE; ++i)
    {
      sate[i].Clear();
    }
    signal.clear();
    index = 0; 
    inOrder = true; 
  }
  int GetPrnIndex(int prn)
  {
    for(int i = 0; i < index; ++i)
    {
      if(sate[i].GetPrn() == prn)
        return i;
    }
    return -1;
  }
};

struct GPGSV
{
public:
  GPGSV() {};
  GPGSV(const GPGSV &v) 
  {
    NumOfMessage = v.NumOfMessage; 
    SequenceNum = v.SequenceNum; 
    NumOfSate = v.NumOfSate; 
    signalId = v.signalId; 
    sates[0] = v.sates[0]; 
    sates[1] = v.sates[1]; 
    sates[2] = v.sates[2]; 
    sates[3] = v.sates[3]; 
  } 
  GPGSV& operator = (const GPGSV &v) 
  { 
    NumOfMessage = v.NumOfMessage; 
    SequenceNum = v.SequenceNum; 
    NumOfSate = v.NumOfSate; 
    signalId = v.signalId; 
    return *this;
  } 

  void Clear()
  {
	  NumOfMessage = 0;
    SequenceNum = 0;
    NumOfSate = 0;
    signalId = 0;
    sates[0].Clear();
    sates[1].Clear();
    sates[2].Clear();
    sates[3].Clear();
  }

	U16     NumOfMessage;
  U16     SequenceNum;
  U16     NumOfSate;
  U08     signalId;
	Satellite    sates[4];
};

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
		UpdateGiSate = 1 << 13,
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
	Satellite giSate[MAX_SATELLITE];

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
		for(int i = 0; i < MAX_SATELLITE; ++i)
		{
      gpSate[i].Clear();
      glSate[i].Clear();
      bdSate[i].Clear();
      gaSate[i].Clear();
      giSate[i].Clear();
		}
	}

private:
	UINT m_updateEvent;
	CCriticalSection GnssDataCs;
};

class IntervalDetector
{
public:
  IntervalDetector() { }
  virtual ~IntervalDetector() {}

  void Arrived() 
  {
    if(timer.IsStarted())
      intervalRecord[recordIter++] = timer.Restart();
    else
      timer.Start();
  }

protected:
  enum { RecordCount = 5 };

protected:
  void Init() { memset(intervalRecord, 0, sizeof(intervalRecord)); recordIter = 0; }

protected:
  MyTimer  timer;
  DWORD intervalRecord[RecordCount];
  int recordIter;
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
		Navic,
	};

	enum NMEA_Type
	{
		NtUnknown,
		PureGP,
		MixGP,
		GN,
	};

  enum { NoSingalId = 0xFF };

protected:
	static int LSB(char lsb);
	static int MSB(char msb);
  static float ParamFloatOrInt(LPCSTR p, int first, int second, float defaultValue);
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
  static GNSS_System currentGsv;
public:
	static GnssData gnssData;
	static NMEA_Type nmeaType;

  static void SetCurrentGsv(GNSS_System gs);
  static GNSS_System GetCurrentGsv() { return currentGsv; };
	static GNSS_System GetGNSSSystem(int prn);
	static GNSS_System GetGNSSSystem0(int prn);
	static GNSS_System GetGNSSSystem1(int prn);
	static GNSS_System GetGNSSSystem2(int prn);
	static GNSS_System GetGNSSSystem3(int prn);

	static int TrimTail(const char* buffer, int offset);
	static NmeaType MessageType(LPCSTR pt, int len);
	static bool GetFirstGsaIn() { return firstGsaIn; }
  
  int ClearSatellitesInRange(GNSS_System gs, int prnStart, int prnEnd);
	Satellites satellites_gp;
	Satellites satellites_gl;
	Satellites satellites_bd;
	Satellites satellites_ga;
	Satellites satellites_gi;
  //For 0xDE and 0xE5 merger
	Satellites satellites2_gp;
	Satellites satellites2_gl;
	Satellites satellites2_bd;
	Satellites satellites2_ga;
	Satellites satellites2_gi;
  void CopySatellites()
  {
    satellites_gp = satellites2_gp;
    satellites_gl = satellites2_gl;
    satellites_bd = satellites2_bd;
    satellites_ga = satellites2_ga;
    satellites_gi = satellites2_gi;
  }
  MyTimer  gpgsvTimer;
  MyTimer  glgsvTimer;
  MyTimer  bdgsvTimer;
  MyTimer  gagsvTimer;
  MyTimer  gigsvTimer;

  DWORD  gpgsvInterval;
  DWORD  glgsvInterval;
  DWORD  bdgsvInterval;
  DWORD  gagsvInterval;
  DWORD  gigsvInterval;

#if(SUPPORT_BDL2_GSV2)
	Satellite satellites2_gp[MAX_SATELLITE];
	Satellite satellites2_gl[MAX_SATELLITE];
	Satellite satellites2_bd[MAX_SATELLITE];

	void ShowGPGSV2msg(GPGSV&, const char*, int);
	void ShowGLGSV2msg(GPGSV&, const char*, int);
	void ShowBDGSV2msg(GPGSV&, const char*, int);
#endif

	void ShowGPGLLmsg(GPGLL&, const char* ,int);
	void ShowGPZDAmsg(GPZDA&, const char* ,int);
	void ShowGPGGAmsg(GPGGA&, const char* ,int);
	void ShowGNSmsg(GPGGA&, const char* ,int);
	void ShowGPRMCmsg(GPRMC&, const char* ,int);
	void ShowGPVTGmsg(GPVTG&, const char* ,int);
	void ShowGPGSVmsg(GPGSV&, const char* ,int);
//#if(SUPPORT_BDL2_GSV2)
//  void ShowGPGSVmsg2(GPGSV& rgpgsv, GPGSV& rgpgsv2, GPGSV& rglgsv, GPGSV& rglgsv2, GPGSV& rbdgsv, GPGSV& rbdgsv2, GPGSV& rgagsv, LPCSTR pt, int len);
//#else
  void ShowGPGSVmsg2(GPGSV& rgpgsv, GPGSV& rglgsv, GPGSV& rbdgsv, GPGSV& rgagsv, GPGSV& rgigsv, LPCSTR pt, int len);
//#endif
	void ShowGLGSVmsg(GPGSV&, const char*, int);
	void ShowGIGSVmsg(GPGSV&, const char*, int);
	void ShowQZGSVmsg(GPGSV&, const char*, int);
	void ShowGAGSVmsg(GPGSV&, const char*, int);
	void ShowGNGSVmsg(GPGSV&, GPGSV&, GPGSV&, GPGSV&, GPGSV&, const char*, int);
	void ShowBDGSVmsg(GPGSV&, const char*, int);
	void ShowGLGSAmsg(GPGSA& msg_glgsa,const char* pt,int offset);
	void ShowGIGSAmsg(GPGSA& msg_gigsa,const char* pt,int offset);
	void ShowGPGSAmsg(GPGSA& msg_gpgsa,const char* pt,int offset);
	//void ShowGPGSAmsg(GPGSA& msg_gpgsa, GPGSA& msg_glgsa, GPGSA& msg_bdgsa, const char* pt, int offset);
	void ShowGNGSAmsg(GPGSA& msg_gpgsa, GPGSA& msg_glgsa, GPGSA& msg_bdgsa, GPGSA& msg_gagsa, GPGSA& msg_gigsa, const char* pt, int offset);
	void ShowBDGSAmsg(GPGSA& msg_bdgsa, const char* pt, int offset);
	void ShowGAGSAmsg(GPGSA& msg_gagsa, const char* pt, int offset);
	void ClearSatellites();
  void ClearSatellites2();
};
