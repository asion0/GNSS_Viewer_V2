#include "StdAfx.h"
#include "NMEA.h"
#include "GPSDlg.h"
using namespace std;

Satellite satellites_gps[MAX_SATELLITE];
Satellite satellites_gnss[MAX_SATELLITE];
Satellite satellites_bd[MAX_SATELLITE];
Satellite satellites_ga[MAX_SATELLITE];

GnssData NMEA::gnssData;
NMEA::NMEA_Type NMEA::nmeaType = NMEA::NtUnknown;
bool NMEA::firstGsaIn = false;

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
	else
	{
//		ASSERT(FALSE);
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
		first++;
		sign = -1;
	}
	int value = defaultValue;
	switch(second - first - 1)
	{
	case 0:
		return defaultValue;
		break;
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
	const double scaleTable[] = { 1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7};
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
	for(int i=6; i<len-3; ++i)
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
		rgga.AgeDGPSData = ParamFloat(pt, dot[12], dot[13], 0);
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

	rgga.Hour = ParamInt(pt, dot[0], dot[0]+3, 0);
	rgga.Min = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
	rgga.Sec = ParamFloat(pt, dot[0]+4, dot[1], 0);
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
	rgga.AgeDGPSData = ParamFloat(pt, dot[10], dot[11], 0);
	rgga.DiffRefStaID = (U16)ParamInt(pt, dot[11], dot[12], 0);
	return true;
}

bool NMEA::GSVProc(GPGSV& rgsv, LPCSTR pt, int len)
{
	int dot[MaxNmeaParam] = {0};	    
	int dotPos = ScanDot(pt, len, dot);

	if( (dotPos + 1) != 8 && (dotPos + 1) != 12 && 
		(dotPos + 1) != 16 && (dotPos + 1) != 20 ) 
	{
		return false;
	}
	if(dot[0] != 6 || pt[len - 3] != '*')
	{
		return 0;
	}

	rgsv.NumOfMessage = ParamInt(pt, dot[0], dot[1], 0);
	rgsv.SequenceNum = ParamInt(pt, dot[1], dot[2], 0);
	rgsv.NumOfSate = ParamInt(pt, dot[2], dot[3], 0);
    for(int i=3, groupIdx=0; i<dotPos; i+=4, ++groupIdx)
	{
		rgsv.sates[groupIdx].SatelliteID = ParamInt(pt, dot[i], dot[i+1], 0);
		rgsv.sates[groupIdx].Elevation = ParamInt(pt, dot[i+1], dot[i+2], 0);
		rgsv.sates[groupIdx].Azimuth = ParamInt(pt, dot[i+2], dot[i+3], 0);
		rgsv.sates[groupIdx].SNR = ParamInt(pt, dot[i+3], dot[i+4], INVALIDATE_SNR);
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
	for(int i=2; (i<2+GSA_MAX_SATELLITE) && (i+1<=dotPos); ++i)
	{
		rgsa.SatelliteID[idPos] = ParamInt(pt, dot[i], dot[i+1], 0);;
		++idPos;
	}   

	rgsa.PDOP = ParamFloat(pt, dot[14], dot[15], 0.0F);
	rgsa.HDOP = ParamFloat(pt, dot[15], dot[16], 0.0F);
	rgsa.VDOP = ParamFloat(pt, dot[16], dot[17], 0.0F);
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

	rrmc.Status = (U08)ParamChar(pt, dot[1], dot[2], 0);
	rrmc.Latitude = ParamDouble(pt, dot[2], dot[3], 0.0F);
	rrmc.Latitude_N_S = (U08)ParamChar(pt, dot[3], dot[4], 0);
	rrmc.Longitude = ParamDouble(pt, dot[4], dot[5], 0.0F);
	rrmc.Longitude_E_W = (U08)ParamChar(pt, dot[5], dot[6], 0);
	rrmc.SpeedKnots = ParamFloat(pt, dot[6], dot[7], 0.0F);
	rrmc.TrueCourse = ParamFloat(pt, dot[7], dot[8], 0.0F);

	rrmc.Day = ParamInt(pt, dot[8], dot[8]+3, 0);
	rrmc.Month = ParamInt(pt, dot[8]+2, dot[8]+5, 0);
	rrmc.Year = ParamInt(pt, dot[8]+4, dot[9], 0) + 2000;
	
	rrmc.MagVar = ParamFloat(pt, dot[9], dot[10], 0.0F);
	rrmc.MagVarDir = (U08)ParamChar(pt, dot[10], dot[11], 0);
	if(dotPos > 11)
	{	//Some customer's NMEA doesn't have this field.
		rrmc.ModeIndicator = (U08)ParamChar(pt, dot[11], dot[12], 0);
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

	rzda.Hour = ParamInt(pt, dot[0], dot[0]+3, 0);
	rzda.Min = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
	rzda.Sec = ParamFloat(pt, dot[0]+4, dot[1], 0.0F);

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
	rgll.Hour = ParamInt(pt, dot[4], dot[4]+3, 0);
	rgll.Min = ParamInt(pt, dot[4]+2, dot[4]+5, 0);
	rgll.Sec = ParamFloat(pt, dot[4]+4, dot[5], 0.0F);	
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
	if(NMEA_PRN_TYPE==0)
	{
		return GetGNSSSystem0(prn);
	}
	else if(NMEA_PRN_TYPE==1)
	{
		return GetGNSSSystem1(prn);
	}
	else if(NMEA_PRN_TYPE==2)
	{
		return GetGNSSSystem2(prn);
	}
	else if(NMEA_PRN_TYPE==3)
	{
		return GetGNSSSystem3(prn);
	}
}

NMEA::GNSS_System NMEA::GetGNSSSystem0(int prn)
{
	if(prn==0)
	{
		return GsUnknown;
	}
	if(prn >= 65 && prn <= 96)
	{
		return Glonass;
	}

	if( (prn >= 0 && prn <= 64) || (prn >= 193 && prn <= 197))
	{
		return Gps;
	}
	if(GPS_183_188 && prn>= 183 && prn <= 188)
	{
		return Gps;
	}

	return Beidou;
}

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

NMEA::GNSS_System NMEA::GetGNSSSystem3(int prn)
{
	if(prn==0)
	{
		return GsUnknown;
	}
	if(prn >= 65 && prn <= 96)
	{
		return Glonass;
	}

	if( (prn >= 0 && prn <= 64) || (prn >= 193 && prn <= 197))
	{
		return Gps;
	}
	if(prn>= 183 && prn <= 188)
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
	for(int i = offset-1 ; i >0 ; i-- )
	{
		if (buffer[i] == '*' )
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
		{ "$BDGGA,", MSG_GGA },
		{ "$GAGGA,", MSG_GGA },

		{ "$GPGSA,", MSG_GPGSA },
		{ "$GLGSA,", MSG_GLGSA },
		{ "$BDGSA,", MSG_BDGSA },
		{ "$GAGSA,", MSG_GAGSA },
		{ "$GNGSA,", MSG_GNGSA },

		{ "$GPGSV,", MSG_GPGSV },
		{ "$GLGSV,", MSG_GLGSV },
		{ "$BDGSV,", MSG_BDGSV },
		{ "$GAGSV,", MSG_GAGSV },

		{ "$GPRMC,", MSG_RMC },
		{ "$GNRMC,", MSG_RMC },
		{ "$BDRMC,", MSG_RMC },
		{ "$GARMC,", MSG_RMC },

		{ "$GPGNS,", MSG_GNS },
		{ "$GNGNS,", MSG_GNS },

		{ "$GPVTG,", MSG_VTG },
		{ "$GNVTG,", MSG_VTG },

		{ "$GPGLL,", MSG_GLL },
		{ "$GNGLL,", MSG_GLL },

		{ "$GPZDA,", MSG_ZDA },
		{ "$GNZDA,", MSG_ZDA },

		{ "$PSTI,", MSG_STI },
		{ "$SkyTraq,", MSG_REBOOT },
		{ "$OLinkStar,", MSG_REBOOT },
		{ NULL, MSG_Unknown }
	};

	int i = 0;
	NmeaType returnType = MSG_Unknown;
	while(nmeaTable[i].type)
	{
		if(0==StrHeaderCompare(pt, nmeaTable[i].subNmea, strlen(nmeaTable[i].subNmea)))
		//if(0==strncmp(pt, nmeaTable[i].subNmea, strlen(nmeaTable[i].subNmea)))
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
}

void NMEA::ShowGPGSAmsg(GPGSA& gpGsa, LPCSTR pt, int len)
{
	GSAProc(gpGsa, pt, len);
}
///*
void NMEA::ShowGNGSAmsg(GPGSA& rgpgsa, GPGSA& rglgsa, GPGSA& rbdgsa, GPGSA& rgagsa, LPCSTR pt, int len)
{
	if(firstGsaIn == false)
	{
		firstGsaIn = true;
		for(int i=0; i<MAX_SATELLITE; ++i)
		{
			rgpgsa.SatelliteID[i] = 0;
			rglgsa.SatelliteID[i] = 0;
			rbdgsa.SatelliteID[i] = 0;
			rgagsa.SatelliteID[i] = 0;
		}
	}
	GPGSA tmpGsa;
	GSAProc(tmpGsa, pt, len);
	
	rgpgsa.Auto_Manu_Mode = tmpGsa.Auto_Manu_Mode;
	rglgsa.Auto_Manu_Mode = tmpGsa.Auto_Manu_Mode;
	rbdgsa.Auto_Manu_Mode = tmpGsa.Auto_Manu_Mode;
	rgagsa.Auto_Manu_Mode = tmpGsa.Auto_Manu_Mode;
	rgpgsa.Mode = tmpGsa.Mode;
	rglgsa.Mode = tmpGsa.Mode;
	rbdgsa.Mode = tmpGsa.Mode;
	rgagsa.Mode = tmpGsa.Mode;
	rgpgsa.PDOP = tmpGsa.PDOP;
	rglgsa.PDOP = tmpGsa.PDOP;
	rbdgsa.PDOP = tmpGsa.PDOP;
	rgagsa.PDOP = tmpGsa.PDOP;
	rgpgsa.HDOP = tmpGsa.HDOP;
	rglgsa.HDOP = tmpGsa.HDOP;
	rbdgsa.HDOP = tmpGsa.HDOP;
	rgagsa.HDOP = tmpGsa.HDOP;
	rgpgsa.VDOP = tmpGsa.VDOP;
	rglgsa.VDOP = tmpGsa.VDOP;
	rbdgsa.VDOP = tmpGsa.VDOP;
	rgagsa.VDOP = tmpGsa.VDOP;

	int gpIndex = 0;
	int glIndex = 0;
	int bdIndex = 0;
	int gaIndex = 0;

	bool hasGpGsa = false;
	bool hasGlGsa = false;
	bool hasBdGsa = false;
	bool hasGaGsa = false;

	for(int i=0; i<MAX_SATELLITE; ++i)
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
		default:
			break;
		}
	}
}

void NMEA::ShowGLGSAmsg(GPGSA& rglgsa, LPCSTR pt, int len)
{
	GSAProc(rglgsa, pt, len);
}

void NMEA::ShowBDGSAmsg(GPGSA& rbdgsa, LPCSTR pt, int len)
{
	GSAProc(rbdgsa, pt, len);
}

void NMEA::ShowGAGSAmsg(GPGSA& rgagsa, LPCSTR pt, int len)
{
	GSAProc(rgagsa, pt, len);
}

void NMEA::ShowGPGGAmsg(GPGGA& gga, LPCSTR pt, int len)
{	
	firstGsaIn = false;
	GGAProc(gga, pt, len);
}

void NMEA::ShowGNSmsg(GPGGA& gga, LPCSTR pt, int len)
{	
	firstGsaIn = false;
	GNSProc(gga, pt, len);
}

void NMEA::ShowGPZDAmsg(GPZDA& zda, LPCSTR pt, int len)
{
	firstGsaIn = false;
	ZDAProc(zda,pt,len);
}

void NMEA::ShowGPRMCmsg(GPRMC& rmc, LPCSTR pt, int len)
{
	firstGsaIn = false;
	RMCProc(rmc,pt,len);
}

void NMEA::ShowGPVTGmsg(GPVTG& vtg, LPCSTR pt, int len)
{
	firstGsaIn = false;
	VTGProc(vtg,pt,len);
}

int gpgsv_counter = 0;
int glgsv_counter = 0;
int bdgsv_counter = 0;
int gagsv_counter = 0;

bool gpgsvHasGlonass = false;
bool gpgsvHasBeidou = false;
bool gpgsvHasGalileo = false;
bool gpgsvHasGps = false;
void NMEA::ShowGPGSVmsg2(GPGSV& rgpgsv, GPGSV& rglgsv, GPGSV& rbdgsv, GPGSV& rgagsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv = { 0 };
	GSVProc(tmpGsv, pt, len);

	if(1 == tmpGsv.SequenceNum)
	{
		gpgsvHasGlonass = false;
		gpgsvHasBeidou = false;
		gpgsvHasGalileo = false;
		gpgsvHasGps = false;
	}

	for(int i=0; i<4; ++i)
	{
		Satellite* s = NULL;
		int idx = 0;
		//int prn = tmpGsv.sates[i].SatelliteID;
		GNSS_System g = GetGNSSSystem(tmpGsv.sates[i].SatelliteID);

		if(GsUnknown==g)
		{
			continue;
		}

		if(Gps==g && !gpgsvHasGps)
		{	//first Gps prn in gsv message.
			gpgsvHasGps = true;
			gpgsv_counter = 0;
			memset(satellites_gps, 0, sizeof(satellites_gps));
		}
		else if(Glonass==g && !gpgsvHasGlonass)
		{	//first Gps prn in gsv message.
			gpgsvHasGlonass = true;
			glgsv_counter = 0;
			memset(satellites_gnss, 0, sizeof(satellites_gnss));
		}
		else if(Beidou==g && !gpgsvHasBeidou)
		{	//first Gps prn in gsv message.
			gpgsvHasBeidou = true;
			bdgsv_counter = 0;
			memset(satellites_bd, 0, sizeof(satellites_bd));
		}
		else if(Galileo==g && !gpgsvHasGalileo)
		{	//first Gps prn in gsv message.
			gpgsvHasGalileo = true;
			gagsv_counter = 0;
			memset(satellites_ga, 0, sizeof(satellites_ga));
		}

		if(Gps==g)
		{
			s = satellites_gps;
			idx = gpgsv_counter;
			++gpgsv_counter;
		}
		else if(Glonass==g)
		{
			s = satellites_gnss;
			idx = glgsv_counter;
			++glgsv_counter;
			nmeaType = MixGP;
		}
		else if(Beidou==g)
		{
			s = satellites_bd;
			idx = bdgsv_counter;
			++bdgsv_counter;
			nmeaType = MixGP;
		}
		else if(Galileo==g)
		{
			s = satellites_ga;
			idx = gagsv_counter;
			++gagsv_counter;
			nmeaType = MixGP;
		}

		if(idx >= MAX_SATELLITE)
		{
			continue;
		}
		s[idx].SatelliteID = tmpGsv.sates[i].SatelliteID;
		s[idx].Elevation   = tmpGsv.sates[i].Elevation;
		s[idx].Azimuth     = tmpGsv.sates[i].Azimuth;
		s[idx].SNR         = tmpGsv.sates[i].SNR;   
	}

	if(gpgsvHasGlonass)
	{
		rglgsv = tmpGsv;
	}
	if(gpgsvHasBeidou)
	{
		rbdgsv = tmpGsv;
	}
	if(gpgsvHasGalileo)
	{
		rgagsv = tmpGsv;
	}
	if(gpgsvHasGps)
	{
		rgpgsv = tmpGsv;
	}
}

void NMEA::ShowGLGSVmsg(GPGSV& glgsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv = { 0 };
	GSVProc(tmpGsv, pt, len);
	glgsv = tmpGsv;

	if(1 == glgsv.SequenceNum)
	{
		glgsv_counter = 0;
		memset(satellites_gnss, 0, sizeof(satellites_gnss));
	}

	for(int i=0; i<4; ++i)
	{
		if(0 == glgsv.sates[i].SatelliteID)
		{
			continue;
		}

		satellites_gnss[glgsv_counter].SatelliteID = glgsv.sates[i].SatelliteID;
		satellites_gnss[glgsv_counter].Elevation   = glgsv.sates[i].Elevation;
		satellites_gnss[glgsv_counter].Azimuth     = glgsv.sates[i].Azimuth;
		satellites_gnss[glgsv_counter].SNR         = glgsv.sates[i].SNR;   
		glgsv_counter++;
		if(glgsv_counter >= MAX_SATELLITE)
		{
			glgsv_counter=0;
		}
	}

	if(glgsv.NumOfMessage == glgsv.SequenceNum)
	{
		glgsv_counter = 0;
	}
}

void NMEA::ShowBDGSVmsg(GPGSV& bdgsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv = { 0 };
	GSVProc(tmpGsv, pt, len);
	bdgsv = tmpGsv;

	if(1 == bdgsv.SequenceNum)
	{
		bdgsv_counter = 0;
		memset(satellites_bd, 0, sizeof(satellites_bd));
	}

	for(int i=0; i<4; ++i)
	{
		if(0 == bdgsv.sates[i].SatelliteID)
		{
			continue;
		}

		satellites_bd[bdgsv_counter].SatelliteID = bdgsv.sates[i].SatelliteID;
		satellites_bd[bdgsv_counter].Elevation = bdgsv.sates[i].Elevation;
		satellites_bd[bdgsv_counter].Azimuth = bdgsv.sates[i].Azimuth;
		satellites_bd[bdgsv_counter].SNR = bdgsv.sates[i].SNR;   
		bdgsv_counter++;
		if(bdgsv_counter >= MAX_SATELLITE)
		{
			bdgsv_counter = 0;
		}
	}

	if(bdgsv.NumOfMessage == bdgsv.SequenceNum)
	{
		bdgsv_counter = 0;
	}
}

void NMEA::ShowGAGSVmsg(GPGSV& gagsv, LPCSTR pt, int len)
{
	firstGsaIn = false;
	GPGSV tmpGsv = { 0 };
	GSVProc(tmpGsv, pt, len);
	gagsv = tmpGsv;

	if(1 == gagsv.SequenceNum)
	{
		gagsv_counter = 0;
		memset(satellites_ga, 0, sizeof(satellites_ga));
	}

	for(int i=0; i<4; ++i)
	{
		if(0 == gagsv.sates[i].SatelliteID)
		{
			continue;
		}

		satellites_ga[gagsv_counter].SatelliteID = gagsv.sates[i].SatelliteID;
		satellites_ga[gagsv_counter].Elevation   = gagsv.sates[i].Elevation;
		satellites_ga[gagsv_counter].Azimuth     = gagsv.sates[i].Azimuth;
		satellites_ga[gagsv_counter].SNR         = gagsv.sates[i].SNR;   
		gagsv_counter++;
		if(gagsv_counter >= MAX_SATELLITE)
		{
			gagsv_counter=0;
		}
	}

	if(gagsv.NumOfMessage == gagsv.SequenceNum)
	{
		gagsv_counter=0;
	}
}