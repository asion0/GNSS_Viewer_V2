#include "StdAfx.h"
#include "NMEA.h"
#include "GPSDlg.h"
//using namespace std;

GnssData NMEA::gnssData;
NMEA::NMEA_Type NMEA::nmeaType = NMEA::NtUnknown;
NMEA::GNSS_System NMEA::currentGsv = NMEA::GsUnknown;
bool NMEA::firstGsaIn = false;

bool IsValidateValue(int snr) { return (snr != NonUseValue); }
bool IsValidateValue(D64 snr) { return (snr != NonUseValue); }

int NMEA::LSB(char lsb)
{
	if(lsb>='0' && lsb<='9')
	{
		return (lsb - '0');
	}
	else if(lsb>='A' && lsb<='F')
	{
		return (lsb - 'A' + 0xA);
	}
	else if(lsb>='a' && lsb<='f')
	{
		return (lsb - 'a' + 0xA);
	}
	return 0;
}

int NMEA::MSB(char msb)
{
	return LSB(msb) * 0x10;
}

int NMEA::ParamInt(LPCSTR p, int first, int second, int defaultValue)
{
	ASSERT(first <= second - 1);
	int sign = 1;
	if(p[first + 1]=='-')
	{
		++first;
		sign = -1;
	}
	int value = defaultValue;
	switch(second - first - 1)
	{
	case 0:
		return defaultValue;
	case 1:
		value = p[first + 1] - '0';
		break;
	case 2:
		value = (p[first + 1] - '0') * 10 + (p[first + 2] - '0');
		break;
	case 3:
		value = (p[first + 1] - '0') * 100 + (p[first + 2] - '0') * 10 + 
			(p[first + 3] - '0');
		break;
	case 4:
		value = (p[first + 1] - '0') * 1000 + (p[first + 2] - '0') * 100 + 
			(p[first + 3] - '0') * 10 + (p[first + 4] - '0');
		break;
	case 5:
		value = (p[first + 1] - '0') * 10000 + (p[first + 2] - '0') * 1000 + 
			(p[first + 3] - '0') * 100 + (p[first + 4] - '0') * 10 + 
			(p[first + 5] - '0');
		break;
	case 6:
		value = (p[first + 1] - '0') * 100000 + (p[first + 2] - '0') * 10000 + 
			(p[first + 3] - '0') * 1000 + (p[first + 4] - '0') * 100 + 
			(p[first + 5] - '0') * 10 + (p[first + 6] - '0');
		break;
	case 7:
		value = (p[first + 1] - '0') * 1000000 + (p[first + 2] - '0') * 100000 + 
			(p[first + 3] - '0') * 10000 + (p[first + 4] - '0') * 1000 + 
			(p[first + 5] - '0') * 100 + (p[first + 6] - '0') * 10 + 
			(p[first + 7] - '0');
		break;
	case 8:
		value = (p[first + 1] - '0') * 10000000 + (p[first + 2] - '0') * 1000000 + 
			(p[first + 3] - '0') * 100000 + (p[first + 4] - '0') * 10000 + 
			(p[first + 5] - '0') * 1000 + (p[first + 6] - '0') * 100 + 
			(p[first + 7] - '0') * 10 + (p[first + 8] - '0');
		break;
	default:
		ASSERT(FALSE);
	}
	return value * sign;
	return 0;
}

U32 NMEA::ParamHexInt(LPCSTR p, int first, int second, int defaultValue)
{
	ASSERT(first <= second - 1);
	int sign = 1;
	if(p[first + 1]=='-')
	{
		++first;
		sign = -1;
	}
	int value = defaultValue;
	switch(second - first - 1)
	{
	case 0:
		return defaultValue;
	case 1:
		value = Utility::LSB(p[first + 1]);
		break;
	case 2:
		value = Utility::LSB(p[first + 1]) * 0x10 + Utility::LSB(p[first + 2]);
		break;
	case 3:
		value = Utility::LSB(p[first + 1]) * 0x100 + Utility::LSB(p[first + 2]) * 0x10 + 
			Utility::LSB(p[first + 3]);
		break;
	case 4:
		value = Utility::LSB(p[first + 1]) * 0x1000 + Utility::LSB(p[first + 2]) * 0x100 + 
			Utility::LSB(p[first + 3]) * 0x10 + Utility::LSB(p[first + 4]);
		break;
	case 5:
		value = Utility::LSB(p[first + 1]) * 0x10000 + Utility::LSB(p[first + 2]) * 0x1000 + 
			Utility::LSB(p[first + 3]) * 0x100 + Utility::LSB(p[first + 4]) * 0x10 + 
			Utility::LSB(p[first + 5]);
		break;
	case 6:
		value = Utility::LSB(p[first + 1]) * 100000 + Utility::LSB(p[first + 2]) * 0x10000 + 
			Utility::LSB(p[first + 3]) * 0x1000 + Utility::LSB(p[first + 4]) * 0x100 + 
			Utility::LSB(p[first + 5]) * 0x10 + Utility::LSB(p[first + 6]);
		break;
	case 7:
		value = Utility::LSB(p[first + 1]) * 1000000 + Utility::LSB(p[first + 2]) * 0x100000 + 
			Utility::LSB(p[first + 3]) * 0x10000 + Utility::LSB(p[first + 4]) * 0x1000 + 
			Utility::LSB(p[first + 5]) * 0x100 + Utility::LSB(p[first + 6]) * 0x10 + 
			Utility::LSB(p[first + 7]);
		break;
	case 8:
		value = Utility::LSB(p[first + 1]) * 10000000 + Utility::LSB(p[first + 2]) * 0x1000000 + 
			Utility::LSB(p[first + 3]) * 0x100000 + Utility::LSB(p[first + 4]) * 0x10000 + 
			Utility::LSB(p[first + 5]) * 0x1000 + Utility::LSB(p[first + 6]) * 0x100 + 
			Utility::LSB(p[first + 7]) * 0x10 + Utility::LSB(p[first + 8]);
		break;
	default:
		ASSERT(FALSE);
	}
	return value * sign;
	return 0;
}

char NMEA::ParamChar(LPCSTR p, int first, int second, char defaultValue)
{
	ASSERT(first == second - 2 || first == second - 1);
	switch(second - first - 1)
	{
	case 0:
		return defaultValue;
	case 1:
		return p[first + 1];;
		break;
	default:
		ASSERT(FALSE);
	}
	return 0;
}

float NMEA::ParamFloatOrInt(LPCSTR p, int first, int second, float defaultValue)
{
	if(second - first == 1)
	{
		return defaultValue;
	}
  bool isFloat = false;
	for(int i = first + 1; i < second - 1; ++i)
	{
		if(p[i] == '.')
		{
			isFloat = true;
			break;
		}
	}
  if(isFloat)
  {
    return ParamFloat(p, first, second, defaultValue);
  }
  else
  {
    return (float)ParamInt(p, first, second, (int)defaultValue);
  }
}

float NMEA::ParamFloat(LPCSTR p, int first, int second, float defaultValue)
{
	if(second - first == 1)
	{
		return defaultValue;
	}
	int dotPos = 0;
	for(int i=first+1; i<second-1; ++i)
	{
		if(p[i]=='.')
		{
			dotPos = i;
			break;
		}
	}
	ASSERT(dotPos);
	const float scaleTable[] = { 1.0E-1F, 1.0E-2F, 1.0E-3F, 1.0E-4F, 1.0E-5F, 1.0E-6F, 1.0E-7F};
	int a = ParamInt(p, first, dotPos, 0);
	if(a < 0)
	{
		return  a - ParamInt(p, dotPos, second, 0) * scaleTable[second-dotPos-2];
	}
	return  a + ParamInt(p, dotPos, second, 0) * scaleTable[second-dotPos-2];
}

double NMEA::ParamDouble(LPCSTR p, int first, int second, float defaultValue)
{
	if(second - first == 1)
	{
		return defaultValue;
	}
	int dotPos = 0;
	for(int i=first+1; i<second-1; ++i)
	{
		if(p[i]=='.')
		{
			dotPos = i;
			break;
		}
	}
	const double scaleTable[] = { 1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8, 1.0E-9};
	int a = ParamInt(p, first, dotPos, 0);
	if(a < 0)
	{
		return  a - ParamInt(p, dotPos, second, 0) * scaleTable[second-dotPos-2];
	}
	return  a + ParamInt(p, dotPos, second, 0) * scaleTable[second-dotPos-2];
}

int NMEA::ScanDot(LPCSTR pt, int len, int dot[MaxNmeaParam])
{
	int dotPos = 0;
	//Pass the beginning "$GPGSV" and the ending "*nn"
	for(int i = 6; i < len - 3; ++i)
	{
		if(pt[i] == ',')
		{
			dot[dotPos] = i;
			++dotPos;
		}
	}
	dot[dotPos] = len - 3;
	return dotPos;
}

bool NMEA::GGAProc(GPGGA& rgga, LPCSTR pt, int len)
{	
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);
	if(dot[0] != 6 || pt[len - 3] != '*')
	{
		return false;
	}

  if((dot[1] - dot[0]) > 6)
  {
	  rgga.Hour = ParamInt(pt, dot[0], dot[0]+3, 0);
	  rgga.Min = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
	  if(dot[1] - dot[0] == 7)
	  {
		  rgga.Sec = (float)ParamInt(pt, dot[0]+4, dot[1], 0);
	  }
	  else
	  {
		  rgga.Sec = ParamFloat(pt, dot[0]+4, dot[1], 0);
	  }
  }
	rgga.Latitude = ParamDouble(pt, dot[1], dot[2], 0.0F);
	rgga.Latitude_N_S = (U08)ParamChar(pt, dot[2], dot[3], 0);
	rgga.Longitude = ParamDouble(pt, dot[3], dot[4], 0.0F);
	rgga.Longitude_E_W = (U08)ParamChar(pt, dot[4], dot[5], 0);
	rgga.GPSQualityIndicator = (U16)ParamChar(pt, dot[5], dot[6], 0);
	rgga.NumsOfSatellites = (U08)ParamInt(pt, dot[6], dot[7], 0);
	rgga.HDOP = ParamFloat(pt, dot[7], dot[8], 0);
	if(dot[9])
	{	//Some GGA doesn't have those data.
		rgga.Altitude = ParamFloat(pt, dot[8], dot[9], 0);
		rgga.Altitude_meters = (U08)ParamChar(pt, dot[9], dot[10], 0);
		rgga.GeoidalSeparation = ParamFloat(pt, dot[10], dot[11], 0);
		rgga.GeoidalSeparation_meters = (U08)ParamChar(pt, dot[11], dot[12], 0);
		//rgga.AgeDGPSData = ParamFloat(pt, dot[12], dot[13], 0);
		rgga.DiffRefStaID = (U16)ParamInt(pt, dot[13], dot[14], 0);
	}
	return true;
}

bool NMEA::GNSProc(GPGGA& rgga, LPCSTR pt, int len)
{	
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);
	if(dot[0] != 6 || pt[len - 3] != '*')
	{
		return false;
	}

  if((dot[1] - dot[0]) > 6)
  {
	  rgga.Hour = ParamInt(pt, dot[0], dot[0]+3, 0);
	  rgga.Min = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
	  rgga.Sec = ParamFloat(pt, dot[0]+4, dot[1], 0);
  }
	rgga.Latitude = ParamDouble(pt, dot[1], dot[2], 0.0F);
	rgga.Latitude_N_S = (U08)ParamChar(pt, dot[2], dot[3], 0);
	rgga.Longitude = ParamDouble(pt, dot[3], dot[4], 0.0F);
	rgga.Longitude_E_W = (U08)ParamChar(pt, dot[4], dot[5], 0);
	rgga.GPSQualityIndicator = (U16)ParamChar(pt, dot[5], dot[5]+2, 0) << 8;
	rgga.GPSQualityIndicator += (U16)ParamChar(pt, dot[5]+1, dot[5]+3, 0);

	rgga.NumsOfSatellites = ParamInt(pt, dot[6], dot[7], 0);
	rgga.HDOP = ParamFloat(pt, dot[7], dot[8], 0);
	rgga.Altitude = ParamFloat(pt, dot[8], dot[9], 0);
	rgga.GeoidalSeparation = ParamFloat(pt, dot[9], dot[10], 0);
	//rgga.AgeDGPSData = ParamFloat(pt, dot[10], dot[11], 0);
	rgga.DiffRefStaID = (U16)ParamInt(pt, dot[11], dot[12], 0);
	return true;
}

bool NMEA::GSVProc(GPGSV& rgsv, LPCSTR pt, int len)
{
  static bool noSigId = false;
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);

	if((dotPos + 1) != 8 && (dotPos + 1) != 12 && 
		(dotPos + 1) != 16 && (dotPos + 1) != 20 &&
    (dotPos + 1) != 9 && (dotPos + 1) != 13 &&  //NMEA 183 V4.0 Spec
		(dotPos + 1) != 17 && (dotPos + 1) != 21) 
	{
		return false;
	}
  //Modify for BDGSV2 20161017 Alex
	//if(dot[0] != 6 || pt[len - 3] != '*')
	if(!(dot[0] == 6 || dot[0] == 7 ) || pt[len - 3] != '*')
	{
		return 0;
	}

  if((dotPos + 1) == 9 || (dotPos + 1) == 13 ||  //NMEA 183 V4.1 Spec
		(dotPos + 1) == 17 || (dotPos + 1) == 21) 
	{
		dotPos -= 1;
    rgsv.signalId = (U08)ParamHexInt(pt, dot[dotPos], dot[dotPos + 1], 0);
	}
  else
  { //Old style
    rgsv.signalId = NoSingalId;
  }

	rgsv.NumOfMessage = ParamInt(pt, dot[0], dot[1], 0);
	rgsv.SequenceNum = ParamInt(pt, dot[1], dot[2], 0);
	rgsv.NumOfSate = ParamInt(pt, dot[2], dot[3], 0);
  int i = 3;

  for(int groupIdx = 0; i < dotPos; i += 4, ++groupIdx)
	{
    rgsv.sates[groupIdx].Set(
      ParamInt(pt, dot[i], dot[i+1], 0),
      ParamInt(pt, dot[i+1], dot[i+2], 0),
      ParamInt(pt, dot[i+2], dot[i+3], 0),
      (rgsv.signalId == NoSingalId) ? 0 : rgsv.signalId, 
      ParamFloatOrInt(pt, dot[i+3], dot[i+4], NonUseValue));
#ifdef _DEBUG
    if(pt[2] == 'B' && rgsv.signalId == 1 && groupIdx == 1)
      break;
#endif
	}
  return true;
}

bool NMEA::GSAProc(GPGSA& rgsa, LPCSTR pt, int len)
{	
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);
	if(dot[0] != 6 || pt[len - 3] != '*')
	{
		return false;
	}

	rgsa.Auto_Manu_Mode = ParamChar(pt, dot[0], dot[1], 0);
	rgsa.Mode = ParamInt(pt, dot[1], dot[2], 0);

	int idPos = 0;
	memset(rgsa.SatelliteID, 0, sizeof(rgsa.SatelliteID));
	for(int i = 2; (i < 2 + GSA_MAX_SATELLITE) && (i + 1 <= dotPos); ++i)
	{
		rgsa.SatelliteID[idPos] = ParamInt(pt, dot[i], dot[i+1], 0);;
		++idPos;
	}   

	rgsa.PDOP = ParamFloat(pt, dot[14], dot[15], 0.0F);
	rgsa.HDOP = ParamFloat(pt, dot[15], dot[16], 0.0F);
	rgsa.VDOP = ParamFloat(pt, dot[16], dot[17], 0.0F);
  if(dotPos >= 18)
  {
	  rgsa.SystemId = ParamInt(pt, dot[17], dot[18], 0);
  }
  else
  {  
    rgsa.SystemId = 0;
  }
	return true;
}

bool NMEA::RMCProc(GPRMC& rrmc, LPCSTR pt, int len)
{	
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);
	if(dot[0] != 6 || pt[len - 3] != '*' || dotPos < 11)
	{
		return false;
	}
	
  if((dot[1] - dot[0]) > 6)
  { //Has time
	  rrmc.Hour = ParamInt(pt, dot[0], dot[0]+3, 0);
	  rrmc.Min = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
	  if(dot[1] - dot[0] == 7)
	  {	//Short time format, no dot
		  rrmc.Sec = (float)ParamInt(pt, dot[0]+4, dot[1], 0);
	  }
	  else
	  {
		  rrmc.Sec = ParamFloat(pt, dot[0]+4, dot[1], 0.0F);
	  }
  }

	rrmc.Status = (U08)ParamChar(pt, dot[1], dot[2], 0);
	rrmc.Latitude = ParamDouble(pt, dot[2], dot[3], 0.0F);
	rrmc.Latitude_N_S = (U08)ParamChar(pt, dot[3], dot[4], 0);
	rrmc.Longitude = ParamDouble(pt, dot[4], dot[5], 0.0F);
	rrmc.Longitude_E_W = (U08)ParamChar(pt, dot[5], dot[6], 0);
	rrmc.SpeedKnots = ParamFloat(pt, dot[6], dot[7], 0.0F);
	rrmc.TrueCourse = ParamFloat(pt, dot[7], dot[8], 0.0F);

  if((dot[9] - dot[8]) > 6)
  { //Has date
	  rrmc.Day = ParamInt(pt, dot[8], dot[8]+3, 0);
	  rrmc.Month = ParamInt(pt, dot[8]+2, dot[8]+5, 0);
	  rrmc.Year = ParamInt(pt, dot[8]+4, dot[9], 0) + 2000;
  }
	rrmc.MagVar = ParamFloat(pt, dot[9], dot[10], 0.0F);
	rrmc.MagVarDir = (U08)ParamChar(pt, dot[10], dot[11], 0);
	if(dotPos >= 11)
	{	//Some customer's NMEA doesn't have this field.
		rrmc.ModeIndicator = (U08)ParamChar(pt, dot[11], dot[11] + 2, 0);
	}
	return true;
}

bool NMEA::ZDAProc(GPZDA& rzda, LPCSTR pt, int len)
{	
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);
	if(dot[0] != 6 || pt[len - 3] != '*' || (dotPos+1) != 7)
	{
		return false;
	}

  if((dot[1] - dot[0]) > 6)
  {
    rzda.Hour = ParamInt(pt, dot[0], dot[0]+3, 0);
	  rzda.Min = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
	  rzda.Sec = ParamFloat(pt, dot[0]+4, dot[1], 0.0F);
  }

  rzda.Day = ParamInt(pt, dot[1], dot[2], 0);
  rzda.Month = ParamInt(pt, dot[2], dot[3], 0);
  rzda.Year = ParamInt(pt, dot[3], dot[4], 0);
	rzda.LocalZoneHours = ParamInt(pt, dot[4], dot[5], 0);
	rzda.LocaZoneMinutes = ParamInt(pt, dot[5], dot[6], 0);
	return true;
}

bool NMEA::VTGProc(GPVTG& rvtg, LPCSTR pt, int len)
{	
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);
	if(dot[0] != 6 || pt[len - 3] != '*')
	{
		return false;
	}

	rvtg.TrueCourse = ParamFloat(pt, dot[0], dot[1], 0.0F);
	rvtg.MagneticCourse = ParamFloat(pt, dot[2], dot[3], 0.0F);
	rvtg.SpeedKnots = ParamFloat(pt, dot[4], dot[5], 0.0F);
	rvtg.SpeedKmPerHur = ParamFloat(pt, dot[6], dot[7], 0.0F);
	rvtg.Mode = (U08)ParamChar(pt, dot[8], dot[9], 0);
	return true;
}

bool NMEA::THSProc(F32& trueHeading, LPCSTR pt, int len)
{	
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);
	if(dot[0] != 6 || pt[len - 3] != '*')
	{
		return false;
	}

	trueHeading = ParamFloat(pt, dot[0], dot[1], 0.0F);
	return true;
}

bool NMEA::GLLProc(GPGLL& rgll, LPCSTR pt, int len)
{
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);
	if(dot[0] != 6 || pt[len - 3] != '*' || (dotPos+1) != 8)
	{
		return false;
	}
	rgll.Latitude = ParamDouble(pt, dot[0], dot[1], 0.0F);
	rgll.Latitude_N_S = (U08)ParamChar(pt, dot[1], dot[2], 0);
	rgll.Longitude = ParamDouble(pt, dot[2], dot[3], 0.0F);
	rgll.Longitude_E_W = (U08)ParamChar(pt, dot[3], dot[4], 0);
  if((dot[5] - dot[4]) > 6)
  {
	  rgll.Hour = ParamInt(pt, dot[4], dot[4]+3, 0);
	  rgll.Min = ParamInt(pt, dot[4]+2, dot[4]+5, 0);
	  rgll.Sec = ParamFloat(pt, dot[4]+4, dot[5], 0.0F);	
  }
	return true;
}

bool NMEA::VarifyNmeaChecksum(LPCSTR pt, int len)
{
	char checksum = 0;	
	for(int j=1; j<len-3; ++j)
	{		
		checksum ^= pt[j];
	}	
	return checksum == (char)(MSB(*(pt+len-2)) + LSB(*(pt+len-1)));
}

NMEA::GNSS_System NMEA::GetGNSSSystem(int prn)
{
	if(NMEA_PRN_TYPE == 0)
	{
		return GetGNSSSystem0(prn);
	}
	else if(NMEA_PRN_TYPE == 1)
	{
		return GetGNSSSystem1(prn);
	}
	else if(NMEA_PRN_TYPE == 2)
	{
		return GetGNSSSystem2(prn);
	}
	else if(NMEA_PRN_TYPE == 3)
	{
		return GetGNSSSystem3(prn);
	}
}
//GL: 65~96; GP: 1~64, 183~188, 193~197; BD: others
NMEA::GNSS_System NMEA::GetGNSSSystem0(int prn)
{
	if(prn == 0 || prn == NonUseValue)
	{
		return GsUnknown;
	}

	if(prn >= 65 && prn <= 96)
	{
		return Glonass;
	}

  //20181205 modify QZSS upper prn to 200, request from Kevin
	//if( (prn >= 0 && prn <= 64) || (prn >= 193 && prn <= 197))
	if( (prn >= 0 && prn <= 64) || (prn >= 193 && prn <= 200))
	{
		return Gps;
	}
	if(GPS_183_188 && prn >= 183 && prn <= 188)
	{
		return Gps;
	}
	if(prn >= 240 && prn <= 300)
	{
		return Navic;
	}
	return Beidou;
}

//GL: 201~300; GP: 101~200; BD: 1~100; GA: 301~400
NMEA::GNSS_System NMEA::GetGNSSSystem1(int prn)
{
	if(prn==0)
	{
		return GsUnknown;
	}
	if(prn >= 1 && prn <= 100)
	{
		return Beidou;
	}
	else if(prn >= 101 && prn <= 200)
	{
		return Gps;
	}
	else if(prn >= 201 && prn <= 300)
	{
		return Glonass;
	}
	else if(prn >= 301 && prn <= 400)
	{
		return Galileo;
	}
	return Beidou;
}

//GP: 1~85; BD: others
NMEA::GNSS_System NMEA::GetGNSSSystem2(int prn)
{
	if(prn==0)
	{
		return GsUnknown;
	}
	if(prn >= 1 && prn <= 85)
	{
		return Gps;
	}
	else if(prn >= 86)
	{
		return Beidou;
	}
	return Beidou;
}

//GL: 65~96; GP: 1~64, 183~188, 193~197; BD: others
NMEA::GNSS_System NMEA::GetGNSSSystem3(int prn)
{
	if(prn == 0)
	{
		return GsUnknown;
	}

	if(prn >= 65 && prn <= 96)
	{
		return Glonass;
	}

  //20181205 modify QZSS upper prn to 200, request from Kevin
	//if( (prn >= 0 && prn <= 64) || (prn >= 193 && prn <= 197))
	if( (prn >= 0 && prn <= 64) || (prn >= 193 && prn <= 200))
	{
		return Gps;
	}
	if(prn >= 183 && prn <= 188)
	{
		return Gps;
	}
	return Beidou;
}

int StrHeaderCompare(LPCSTR pt, LPCSTR header, int len)
{
	LPCSTR p1 = pt;
	LPCSTR p2 = header;
	int cl = 0;
	while(*p1 != 0 && *p2 != 0 && cl < len)
	{
		if(*p1 > *p2)
		{
			return 1;
		}
		else if(*p1 < *p2)
		{
			return -1;
		}
		++cl;
		++p1;
		++p2;
	}
	if(*p2==0)
	{
		return 0;
	}
	return -1;
}

int NMEA::TrimTail(const char* buffer, int offset)
{
	for(int i = offset - 1; i > 0; --i)
	{
		if (buffer[i]=='*')
		{
			offset = i + 3;
			break;
		}
	}
	return offset;
}

NmeaType NMEA::MessageType(LPCSTR pt, int len)
{
	struct NmeaTypeEntry
	{
		const char *subNmea;
		NmeaType type;
	};
	NmeaTypeEntry nmeaTable[] = {
		{ "$GPGGA,", MSG_GGA },
		{ "$GNGGA,", MSG_GGA },
		{ "$GIGGA,", MSG_GGA },
		{ "$BDGGA,", MSG_GGA },
		{ "$GBGGA,", MSG_GGA },
		{ "$GAGGA,", MSG_GGA },

		{ "$GPGSA,", MSG_GPGSA },
		{ "$GLGSA,", MSG_GLGSA },
		{ "$BDGSA,", MSG_BDGSA },
		{ "$GBGSA,", MSG_BDGSA },
		{ "$GAGSA,", MSG_GAGSA },
		{ "$GNGSA,", MSG_GNGSA },
		{ "$GIGSA,", MSG_GIGSA },
		{ "$NCGSA,", MSG_GIGSA },
		{ "$QZGSA,", MSG_QZGSA },

		{ "$GPGSV,", MSG_GPGSV },
		{ "$GPGSV2,", MSG_GPGSV2 },
		{ "$GLGSV,", MSG_GLGSV },
		{ "$BDGSV,", MSG_BDGSV },
		{ "$GBGSV,", MSG_BDGSV },
		{ "$GAGSV,", MSG_GAGSV },
		{ "$GNGSV,", MSG_GNGSV },
		{ "$BDGSV2,", MSG_BDGSV2 },
		{ "$GIGSV,", MSG_GIGSV },
		{ "$NCGSV,", MSG_GIGSV },
		{ "$QZGSV,", MSG_QZGSV },

		{ "$GPRMC,", MSG_RMC },
		{ "$GNRMC,", MSG_RMC },
		{ "$GIRMC,", MSG_RMC },
		{ "$BDRMC,", MSG_RMC },
		{ "$GBRMC,", MSG_RMC },
		{ "$GARMC,", MSG_RMC },

		{ "$GPGNS,", MSG_GNS },
		{ "$GNGNS,", MSG_GNS },

		{ "$GPVTG,", MSG_VTG },
		{ "$GNVTG,", MSG_VTG },
		{ "$GIVTG,", MSG_VTG },

		{ "$GPTHS,", MSG_THS },
		{ "$GNTHS,", MSG_THS },
		{ "$GITHS,", MSG_THS },

		{ "$GPGLL,", MSG_GLL },
		{ "$GNGLL,", MSG_GLL },
		{ "$GIGLL,", MSG_GLL },

		{ "$GPZDA,", MSG_ZDA },
		{ "$GNZDA,", MSG_ZDA },
		{ "$GIZDA,", MSG_ZDA },

		{ "$PSTI,", MSG_STI },
		{ "$SkyTraq,", MSG_REBOOT },
		//{ "$OLinkStar,", MSG_REBOOT },
		{ "$PIRNSF,", MSG_PIRNSF },
		{ NULL, MSG_Unknown }
	};

	int i = 0;
	NmeaType returnType = MSG_Unknown;
	while(nmeaTable[i].type)
	{
		if(0==StrHeaderCompare(pt, nmeaTable[i].subNmea, strlen(nmeaTable[i].subNmea)))
		{
			returnType = nmeaTable[i].type;
			break;
		}
		++i;
	}
	if(!VarifyNmeaChecksum(pt, len))
	{
		if(MSG_REBOOT==returnType)
			return MSG_REBOOT;
		else
			return MSG_ERROR;
	}
	return returnType;
}

void NMEA::ShowGPGLLmsg(GPGLL& rGll, LPCSTR pt, int len)
{	
	firstGsaIn = false;
	GLLProc(rGll, pt, len);
  currentGsv = GsUnknown;
}

void NMEA::ShowGPGSAmsg(GPGSA& gpGsa, LPCSTR pt, int len)
{
	GSAProc(gpGsa, pt, len);
}
///*
void NMEA::ShowGNGSAmsg(GPGSA& rgpgsa, GPGSA& rglgsa, GPGSA& rbdgsa, GPGSA& rgagsa, GPGSA& rgigsa, LPCSTR pt, int len)
{
	if(firstGsaIn == false)
	{
		firstGsaIn = true;
		for(int i=0; i < MAX_SATELLITE; ++i)
		{
			rgpgsa.SatelliteID[i] = 0;
			rglgsa.SatelliteID[i] = 0;
			rbdgsa.SatelliteID[i] = 0;
			rgagsa.SatelliteID[i] = 0;
			rgigsa.SatelliteID[i] = 0;
		}
	}
	GPGSA tmpGsa;
	GSAProc(tmpGsa, pt, len);

	int gpIndex = 0;
	int glIndex = 0;
	int bdIndex = 0;
	int gaIndex = 0;
	int giIndex = 0;

	bool hasGpGsa = false;
	bool hasGlGsa = false;
	bool hasBdGsa = false;
	bool hasGaGsa = false;
	bool hasGiGsa = false;

  for(int i = 0; i < MAX_SATELLITE; ++i)
  {
    if(tmpGsa.SystemId == 0)
    {
	      GNSS_System g = GetGNSSSystem(tmpGsa.SatelliteID[i]);
	      switch(g)
	      {
	      case Gps:
		      rgpgsa.SatelliteID[gpIndex++] = tmpGsa.SatelliteID[i];
		      hasGpGsa = true;
		      break;
	      case Glonass:
		      rglgsa.SatelliteID[glIndex++] = tmpGsa.SatelliteID[i];
		      hasGlGsa = true;
		      break;
	      case Beidou:
		      rbdgsa.SatelliteID[bdIndex++] = tmpGsa.SatelliteID[i];
		      hasBdGsa = true;
		      break;
	      case Galileo:
		      rgagsa.SatelliteID[gaIndex++] = tmpGsa.SatelliteID[i];
		      hasGaGsa = true;
		      break;
	      case Navic:
		      rgigsa.SatelliteID[giIndex++] = tmpGsa.SatelliteID[i];
		      hasGiGsa = true;
		      break;
        default:
		      break;
	      }
    }
    else
    {
	    switch(tmpGsa.SystemId)
      {
      case 1:
	      rgpgsa.SatelliteID[gpIndex++] = tmpGsa.SatelliteID[i];
	      hasGpGsa = true;
	      break;
      case 2:
	      rglgsa.SatelliteID[glIndex++] = tmpGsa.SatelliteID[i];
	      hasGlGsa = true;
	      break;
      case 3:
	      rgagsa.SatelliteID[gaIndex++] = tmpGsa.SatelliteID[i];
	      hasGaGsa = true;
	      break;
      case 4:
	      rbdgsa.SatelliteID[bdIndex++] = tmpGsa.SatelliteID[i];
	      hasBdGsa = true;
	      //rgigsa.SatelliteID[bdIndex++] = tmpGsa.SatelliteID[i];
	      //hasGiGsa = true;
      case 5: //NMEA 4.11 Id 5 is GQ Qzss system
	      rgigsa.SatelliteID[giIndex++] = tmpGsa.SatelliteID[i];
	      hasGiGsa = true;
	      break;
      case 6: //NMEA 4.11 Standard
	      rgigsa.SatelliteID[giIndex++] = tmpGsa.SatelliteID[i];
	      hasGiGsa = true;
	      break;      
      default:
	      //rbdgsa.SatelliteID[bdIndex++] = tmpGsa.SatelliteID[i];
	      //hasBdGsa = true;
        break;
      }
    }
  } //for(int i=0; i<MAX_SATELLITE; ++i)

  if(hasGpGsa)
  {
	  rgpgsa.Auto_Manu_Mode = tmpGsa.Auto_Manu_Mode;
	  rgpgsa.Mode = tmpGsa.Mode;
	  rgpgsa.PDOP = tmpGsa.PDOP;
	  rgpgsa.HDOP = tmpGsa.HDOP;
	  rgpgsa.VDOP = tmpGsa.VDOP;
  }
  if(hasGlGsa)
  {
	  rglgsa.Auto_Manu_Mode = tmpGsa.Auto_Manu_Mode;
	  rglgsa.Mode = tmpGsa.Mode;
	  rglgsa.PDOP = tmpGsa.PDOP;
	  rglgsa.HDOP = tmpGsa.HDOP;
	  rglgsa.VDOP = tmpGsa.VDOP;
  }
  if(hasBdGsa)
  {
	  rbdgsa.Auto_Manu_Mode = tmpGsa.Auto_Manu_Mode;
	  rbdgsa.Mode = tmpGsa.Mode;
	  rbdgsa.PDOP = tmpGsa.PDOP;
	  rbdgsa.HDOP = tmpGsa.HDOP;
	  rbdgsa.VDOP = tmpGsa.VDOP;
  }
  if(hasGaGsa)
  {
	  rgagsa.Auto_Manu_Mode = tmpGsa.Auto_Manu_Mode;
	  rgagsa.Mode = tmpGsa.Mode;
	  rgagsa.PDOP = tmpGsa.PDOP;
	  rgagsa.HDOP = tmpGsa.HDOP;
	  rgagsa.VDOP = tmpGsa.VDOP;
  }
  if(hasGiGsa)
  {
	  rgigsa.Auto_Manu_Mode = tmpGsa.Auto_Manu_Mode;
	  rgigsa.Mode = tmpGsa.Mode;
	  rgigsa.PDOP = tmpGsa.PDOP;
	  rgigsa.HDOP = tmpGsa.HDOP;
	  rgigsa.VDOP = tmpGsa.VDOP;  
  }
  currentGsv = GsUnknown;
}

void NMEA::ShowGLGSAmsg(GPGSA& rglgsa, LPCSTR pt, int len)
{
	GSAProc(rglgsa, pt, len);
  currentGsv = GsUnknown;
}

void NMEA::ShowGIGSAmsg(GPGSA& rgigsa, LPCSTR pt, int len)
{
	GSAProc(rgigsa, pt, len);
  currentGsv = GsUnknown;
}

void NMEA::ShowBDGSAmsg(GPGSA& rbdgsa, LPCSTR pt, int len)
{
	GSAProc(rbdgsa, pt, len);
  currentGsv = GsUnknown;
}

void NMEA::ShowGAGSAmsg(GPGSA& rgagsa, LPCSTR pt, int len)
{
	GSAProc(rgagsa, pt, len);
  currentGsv = GsUnknown;
}

void NMEA::ShowGPGGAmsg(GPGGA& gga, LPCSTR pt, int len)
{	
	firstGsaIn = false;
	GGAProc(gga, pt, len);
  currentGsv = GsUnknown;
}

void NMEA::ShowGNSmsg(GPGGA& gga, LPCSTR pt, int len)
{	
	firstGsaIn = false;
	GNSProc(gga, pt, len);
  currentGsv = GsUnknown;
}

void NMEA::ShowGPZDAmsg(GPZDA& zda, LPCSTR pt, int len)
{
	firstGsaIn = false;
	ZDAProc(zda,pt,len);
  currentGsv = GsUnknown;
}

void NMEA::ShowGPRMCmsg(GPRMC& rmc, LPCSTR pt, int len)
{
	firstGsaIn = false;
	RMCProc(rmc,pt,len);
  currentGsv = GsUnknown;
}

void NMEA::ShowGPVTGmsg(GPVTG& vtg, LPCSTR pt, int len)
{
	firstGsaIn = false;
	VTGProc(vtg,pt,len);
  currentGsv = GsUnknown;
}

void NMEA::ShowGPTHSmsg(F32& trueHeading, LPCSTR pt, int len)
{
	firstGsaIn = false;
	THSProc(trueHeading, pt, len);
  currentGsv = GsUnknown;
}

static int gpgsv_counter = 0;
static int glgsv_counter = 0;
static int bdgsv_counter = 0;
static int gagsv_counter = 0;
static int gigsv_counter = 0;

static bool gpgsvHasNavic = false;
static bool gpgsvHasGlonass = false;
static bool gpgsvHasBeidou = false;
static bool gpgsvHasGalileo = false;
static bool gpgsvHasGps = false;

void NMEA::ShowGPGSVmsg2(GPGSV& rgpgsv, GPGSV& rglgsv, GPGSV& rbdgsv, GPGSV& rgagsv, GPGSV& rgigsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv;
	GSVProc(tmpGsv, pt, len);
  
  if(tmpGsv.signalId != NoSingalId)
  { //NMEA 4.11
    rgpgsv = tmpGsv;
	  if(1 == tmpGsv.SequenceNum && currentGsv != Gps)
	  {
      currentGsv = Gps;
      gpgsv_counter = 0;
      satellites_gp.Clear();
	  }
	  for(int i = 0; i < 4; ++i)
	  {
		  if(!tmpGsv.sates[i].IsInUsePrn())
		  {
			  continue;
		  }
      satellites_gp.SetSate(tmpGsv.sates[i]);

      ++gpgsv_counter;
		  if(gpgsv_counter >= MAX_SATELLITE)
		  {
			  gpgsv_counter=0;
		  }
	  }

	  if(tmpGsv.NumOfMessage == tmpGsv.SequenceNum)
	  {
		  gpgsv_counter = 0;
      satellites_gp.AddSnrSigId(tmpGsv.signalId);
	  }
    return;
  }

  //Old style NMEA
  if(tmpGsv.signalId == NoSingalId)
  {
    tmpGsv.signalId = 0;
  }

	if(1 == tmpGsv.SequenceNum)
	{
		gpgsvHasNavic = false;
		gpgsvHasGlonass = false;
		gpgsvHasBeidou = false;
		gpgsvHasGalileo = false;
		gpgsvHasGps = false;
	}

  Satellites* s = NULL;
	for(int i = 0; i < 4; ++i)
	{
		int idx = 0;
	  GNSS_System g = GetGNSSSystem(tmpGsv.sates[i].GetPrn());
	  if(GsUnknown == g)
	  {
		  continue;
	  }

	  if(Gps == g && !gpgsvHasGps)
	  {	//first Gps prn in gsv message.
		  gpgsvHasGps = true;
		  gpgsv_counter = 0;
      if(tmpGsv.signalId == 0 || tmpGsv.signalId == 1)
      {
        satellites_gp.Clear();
      }
	  }
	  else if(Glonass == g && !gpgsvHasGlonass)
	  {	//first Gps prn in gsv message.
		  gpgsvHasGlonass = true;
		  glgsv_counter = 0;
      if(tmpGsv.signalId == 0 || tmpGsv.signalId == 1)
      {
        satellites_gl.Clear();
      }
	  }
	  else if(Beidou == g && !gpgsvHasBeidou)
	  {	//first Gps prn in gsv message.
		  gpgsvHasBeidou = true;
		  bdgsv_counter = 0;
      if(tmpGsv.signalId == 0 || tmpGsv.signalId == 1)
      {
        satellites_bd.Clear();
      }
	  }
	  else if(Galileo == g && !gpgsvHasGalileo)
	  {	//first Gps prn in gsv message.
		  gpgsvHasGalileo = true;
		  gagsv_counter = 0;
      satellites_ga.Clear();
	  }
	  else if(Navic == g && !gpgsvHasNavic)
	  {	//first Gps prn in gsv message.
		  gpgsvHasNavic = true;
		  gigsv_counter = 0;
      satellites_gi.Clear();
	  }

	  if(Gps == g)
	  {
      s = &satellites_gp;
      idx = gpgsv_counter;
		  ++gpgsv_counter;
	  }
	  else if(Glonass == g)
	  {
	    s = &satellites_gl;
		  idx = glgsv_counter;
		  ++glgsv_counter;
		  nmeaType = MixGP;
	  }
	  else if(Beidou == g)
	  {
	    s = &satellites_bd;

		  idx = bdgsv_counter;
		  ++bdgsv_counter;
		  nmeaType = MixGP;
	  }
	  else if(Galileo == g)
	  {
		  s = &satellites_ga;
		  idx = gagsv_counter;
		  ++gagsv_counter;
		  nmeaType = MixGP;
	  }

	  if(idx >= MAX_SATELLITE)
	  {
		  continue;
	  }
    s->SetSate(tmpGsv.sates[i]);
	} //for(int i = 0; i < 4; ++i)

	if(gpgsvHasNavic)
	{
    satellites_gi.AddSnrSigId(tmpGsv.signalId);
    rgigsv = tmpGsv;
	}
	if(gpgsvHasGlonass)
	{
    satellites_gl.AddSnrSigId(tmpGsv.signalId);
    rglgsv = tmpGsv;
	}
	if(gpgsvHasBeidou)
	{
    satellites_bd.AddSnrSigId(tmpGsv.signalId);
		rbdgsv = tmpGsv;
	}
	if(gpgsvHasGalileo)
	{
    satellites_ga.AddSnrSigId(tmpGsv.signalId);
		rgagsv = tmpGsv;
  }
	if(gpgsvHasGps)
	{
    satellites_gp.AddSnrSigId(tmpGsv.signalId);
		rgpgsv = tmpGsv;
	}
}

#if(SUPPORT_BDL2_GSV2)
void NMEA::ShowGPGSV2msg(GPGSV& gpgsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv = { 0 };
	GSVProc(tmpGsv, pt, len);
	gpgsv = tmpGsv;

	if(1 == gpgsv.SequenceNum)
	{
		gpgsv_counter = 0;
	}

	for(int i=0; i<4; ++i)
	{
		if(0 == gpgsv.sates[i].prn)
		{
			continue;
		}
    satellites_gp.SetSate(tmpGsv.sates[i].prn, tmpGsv.sates[i].ele, tmpGsv.sates[i].azi,
      -1, tmpGsv.sates[i].snr[0]);

		gpgsv_counter++;
		if(gpgsv_counter >= MAX_SATELLITE)
		{
			gpgsv_counter = 0;
		}
	}

	if(gpgsv.NumOfMessage == gpgsv.SequenceNum)
	{
		gpgsv_counter = 0;
    satellites_gp.AddSnrSigId(tmpGsv.signalId);
	}
}
#endif

void NMEA::ShowGLGSVmsg(GPGSV& glgsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv;
	GSVProc(tmpGsv, pt, len);
  glgsv = tmpGsv;

  //Old style NMEA
  if(tmpGsv.signalId == NoSingalId)
  {
    tmpGsv.signalId = 0;
  }

	if(1 == tmpGsv.SequenceNum && currentGsv != Glonass)
	{
    currentGsv = Glonass;
		glgsv_counter = 0;
    satellites_gl.Clear();
	}

	for(int i = 0; i < 4; ++i)
	{
		if(!tmpGsv.sates[i].IsInUsePrn())
		{
			continue;
		}
    satellites_gl.SetSate(tmpGsv.sates[i]);
		++glgsv_counter;
		if(glgsv_counter >= MAX_SATELLITE)
		{
			glgsv_counter = 0;
		}
	}

	if(tmpGsv.NumOfMessage == tmpGsv.SequenceNum)
	{
		glgsv_counter = 0;
    satellites_gl.AddSnrSigId(tmpGsv.signalId);
	}
}

void NMEA::ShowGIGSVmsg(GPGSV& gigsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv;
	GSVProc(tmpGsv, pt, len);
  gigsv = tmpGsv;
  //Old style NMEA
  if(tmpGsv.signalId == NoSingalId)
  {
    tmpGsv.signalId = 0;
  }

	if(1 == tmpGsv.SequenceNum && currentGsv != Navic)
	{
    currentGsv = Navic;
		gigsv_counter = 0;
    satellites_gi.Clear();
	}

	for(int i = 0; i < 4; ++i)
	{
		if(!tmpGsv.sates[i].IsInUsePrn())
		{
			continue;
		}
    satellites_gi.SetSate(tmpGsv.sates[i]);
		gigsv_counter++;
		if(gigsv_counter >= MAX_SATELLITE)
		{
			gigsv_counter = 0;
		}
	}

	if(tmpGsv.NumOfMessage == tmpGsv.SequenceNum)
	{
		gigsv_counter = 0;
    satellites_gi.AddSnrSigId(tmpGsv.signalId);
	}
}

void NMEA::ShowQZGSVmsg(GPGSV& gpgsv, LPCSTR pt, int len)
{
	//firstGsaIn = false;
	//GPGSV tmpGsv;
	//GSVProc(tmpGsv, pt, len);
 // gigsv = tmpGsv;
 // //Old style NMEA
 // if(tmpGsv.signalId == NoSingalId)
 // {
 //   tmpGsv.signalId = 0;
 // }

	//if(1 == tmpGsv.SequenceNum && currentGsv != Navic)
	//{
 //   currentGsv = Navic;
	//	gigsv_counter = 0;
 //   satellites_gi.Clear();
	//}

	//for(int i = 0; i < 4; ++i)
	//{
	//	if(!tmpGsv.sates[i].IsInUsePrn())
	//	{
	//		continue;
	//	}
 //   satellites_gi.SetSate(tmpGsv.sates[i]);
	//	gigsv_counter++;
	//	if(gigsv_counter >= MAX_SATELLITE)
	//	{
	//		gigsv_counter = 0;
	//	}
	//}

	//if(tmpGsv.NumOfMessage == tmpGsv.SequenceNum)
	//{
	//	gigsv_counter = 0;
 //   satellites_gi.AddSnrSigId(tmpGsv.signalId);
	//}
}

void NMEA::ShowBDGSVmsg(GPGSV& bdgsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv;
	GSVProc(tmpGsv, pt, len);
  bdgsv = tmpGsv;
  //Old style NMEA
  if(tmpGsv.signalId == NoSingalId)
  {
    tmpGsv.signalId = 0;
  }

	if(1 == tmpGsv.SequenceNum && currentGsv != Beidou)
	{
    currentGsv = Beidou;
		bdgsv_counter = 0;
    satellites_bd.Clear();
	}

	for(int i = 0; i < 4; ++i)
	{
		if(!tmpGsv.sates[i].IsInUsePrn())
		{
			continue;
		}
    satellites_bd.SetSate(tmpGsv.sates[i]);
		++bdgsv_counter;
		if(bdgsv_counter >= MAX_SATELLITE)
		{
			bdgsv_counter = 0;
		}
	}

	if(tmpGsv.NumOfMessage == tmpGsv.SequenceNum)
	{
		bdgsv_counter = 0;
    satellites_bd.AddSnrSigId(tmpGsv.signalId);
	}
}

#if(SUPPORT_BDL2_GSV2)
void NMEA::ShowBDGSV2msg(GPGSV& bdgsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv = { 0 };
	GSVProc(tmpGsv, pt, len);
	bdgsv = tmpGsv;

	if(1 == bdgsv.SequenceNum)
	{
		bdgsv_counter = 0;
	}

	for(int i=0; i<4; ++i)
	{
		if(0 == bdgsv.sates[i].prn)
		{
			continue;
		}

    satellites_gp.SetSate(tmpGsv.sates[i].prn, tmpGsv.sates[i].ele, tmpGsv.sates[i].azi,
      -1, tmpGsv.sates[i].snr[0]);

		++bdgsv_counter;
		if(bdgsv_counter >= MAX_SATELLITE)
		{
			bdgsv_counter = 0;
		}
	}

	if(bdgsv.NumOfMessage == bdgsv.SequenceNum)
	{
		bdgsv_counter = 0;
    satellites_bd.AddSnrSigId(tmpGsv.signalId);
	}
}
#endif

void NMEA::ShowGAGSVmsg(GPGSV& gagsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv;
	GSVProc(tmpGsv, pt, len);
	gagsv = tmpGsv;
  //Old style NMEA
  if(tmpGsv.signalId == NoSingalId)
  {
    tmpGsv.signalId = 0;
  }

	if(1 == tmpGsv.SequenceNum && currentGsv != Galileo)
	{
    currentGsv = Galileo;
    gagsv_counter = 0;
    satellites_ga.Clear();
	}

	for(int i = 0; i < 4; ++i)
	{
		if(!tmpGsv.sates[i].IsInUsePrn())
		{
			continue;
		}
    satellites_ga.SetSate(tmpGsv.sates[i]);
    ++gagsv_counter;
		if(gagsv_counter >= MAX_SATELLITE)
		{
			gagsv_counter=0;
		}
	}

	if(tmpGsv.NumOfMessage == tmpGsv.SequenceNum)
	{
		gagsv_counter = 0;
    satellites_ga.AddSnrSigId(tmpGsv.signalId);
	}
}

void NMEA::ShowGNGSVmsg(GPGSV& gpgsv, GPGSV& glgsv, GPGSV& bdgsv, GPGSV& gagsv, GPGSV& gigsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv;
	GSVProc(tmpGsv, pt, len);
  //Old style NMEA
  if(tmpGsv.signalId == NoSingalId)
  {
    tmpGsv.signalId = 0;
  }

	GPGSV* pGsv = NULL;
	int* pGsv_counter = NULL;
	Satellites* s = NULL;

	GNSS_System g = GetGNSSSystem(tmpGsv.sates[0].GetPrn());
	switch(g)
	{
	case Gps:
		gpgsv = tmpGsv;
		pGsv = &gpgsv;
		pGsv_counter = &gpgsv_counter;
		s = &satellites_gp;
		break;
	case Glonass:
		glgsv = tmpGsv;
		pGsv = &glgsv;
		pGsv_counter = &glgsv_counter;
		s = &satellites_gl;
		break;
	case Beidou:
		bdgsv = tmpGsv;
		pGsv = &bdgsv;
		pGsv_counter = &bdgsv_counter;
		s = &satellites_bd;
		break;
	case Galileo:
		gagsv = tmpGsv;
		pGsv = &gagsv;
		pGsv_counter = &gagsv_counter;
		s = &satellites_ga;
		break;
	case Navic:
		gigsv = tmpGsv;
		pGsv = &gigsv;
		pGsv_counter = &gigsv_counter;
		s = &satellites_gi;
		break;
	default:
		break;
	}

	if(1 == tmpGsv.SequenceNum)
	{
		*pGsv_counter = 0;
    s->Clear();
	}

	for(int i = 0; i < 4; ++i)
	{
		if(!tmpGsv.sates[i].IsInUsePrn())
		{
			continue;
		}
    s->SetSate(tmpGsv.sates[i]);

    ++(*pGsv_counter);
		if(*pGsv_counter >= MAX_SATELLITE)
		{
			*pGsv_counter = 0;
		}
	}

	if(pGsv->NumOfMessage == pGsv->SequenceNum)
	{
		*pGsv_counter=0;
	}
}


void NMEA::ClearSatellites()
{
  ClearSatellites2();
  satellites_gp.Clear();
  satellites_gl.Clear();
  satellites_bd.Clear();
  satellites_ga.Clear();
  satellites_gi.Clear();
}

void NMEA::ClearSatellites2()
{
  satellites2_gp.Clear();
  satellites2_gl.Clear();
  satellites2_bd.Clear();
  satellites2_ga.Clear();
  satellites2_gi.Clear();
}

int NMEA::ClearSatellitesInRange(GNSS_System gs, int prnStart, int prnEnd)
{
  Satellites* s = NULL;
  switch(gs)
  {
  case Gps:
	  s = &satellites_gp;
	  break;
  case Glonass:
	  s = &satellites_gl;
	  break;
  case Beidou:
	  s = &satellites_bd;
	  break;
  case Galileo:
	  s = &satellites_ga;
	  break;
  case Navic:
	  s = &satellites_gi;
	  break;
  }
  if(s == NULL || prnStart > prnEnd)
  {
    return -1;
  }

  s->ClearPrnInRange(prnStart, prnEnd);
  return s->GetSateCount();
}

void NMEA::SetCurrentGsv(GNSS_System gs)
{
  currentGsv = gs;
}