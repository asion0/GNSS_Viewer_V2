#include "stdafx.h"
#include "Global.h"
#include "WaitReadLog.h"
#include "Monitor_1PPS.h"
#include "NMEA.h"
#include "GPSDlg.h"
#include "Serial.h"

const double R2D = 57.2957795131;
const COLORREF g_panelBkColor = RGB(250, 250, 250);
Setting g_setting;

HANDLE g_connectEvent = NULL;
HANDLE g_closeEvent = NULL;
CWaitReadLog* g_waitReadDialog = NULL;
HANDLE waitlog = NULL;

double pos_lat, pos_lon;
int m_almanac_no;
CString m_almanac_filename;
CMonitor_1PPS *dia_monitor_1pps = NULL;

int FlashBytes[] = { 8*1024, 16*1024, 24*1024, 32*1024 };
#if(SPECIAL_BAUD_RATE)
int Setting::BaudrateTable[] = {7200, 14400, 28800, 57600, 86400, 172800, 345600, 691200, 1382400};
#elif (CUSTOMER_CWQX_160815)
int Setting::BaudrateTable[] = {4800, 9600, 19200, 38400, 57600, 115200, 0, 0, 0, 0, 0, 1200, 2400};
#else
int Setting::BaudrateTable[] = {4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
#endif

int Setting::GetBaudrateTableSize()
{
  return sizeof(Setting::BaudrateTable) / sizeof(Setting::BaudrateTable[0]);
}

int Setting::BaudrateIndex(int b)
{
  int idx = -1;
	for(int i = 0; i < GetBaudrateTableSize(); ++i)
	{
		if(BaudrateTable[i] == b)
		{
			idx = i;
			return idx;
		}
	}
  return idx;
}

int Setting::SetBaudrateByValue(int b)
{
	baudrate = BaudrateIndex(b);
  return baudrate;
}

void Setting::InitBaudrateCombo(CComboBox* c, bool addSPI, bool addI2C)
{
	c->ResetContent();
	for(int i=0; i<GetBaudrateTableSize(); ++i)
	{
		CString strIdx;
    if(Setting::BaudrateTable[i] != 0)
    {
		  strIdx.Format("%d", Setting::BaudrateTable[i]);
    }
    else
    {
      strIdx = "Reserve";
    }
		c->AddString(strIdx);
	}
  if(addSPI)
  {
	  c->AddString("SPI");
  }
	if(addI2C)
  {
    c->AddString("I2C");
  }
}

U32 ConvertCharToU32(const char *src)
{
	U32 resultValue = 0;
	U08 charIter;
	const char *ptr = src;
	int start = strlen(src);
	for (int i=start-1; i>=0;--i)
	{
		charIter = *ptr;
		if (charIter >= '0' && charIter <= '9')
		{
			resultValue += (*ptr - '0') << (i*4);
		}
		else if (charIter >= 'A' && charIter <= 'F')
		{
			resultValue += (*ptr - 'A' + 0xA) << (i*4);
		}
		else if (charIter >= 'a' && charIter <= 'f')
		{
			resultValue += (*ptr - 'a' + 0xA) << (i*4);
		}
		ptr++;
	}
	return resultValue;
}

UINT WaitLogRead(LPVOID pParam)
{
	if(g_waitReadDialog != NULL)
	{
		g_waitReadDialog->DoModal();
	}

	if(!ResetEvent(waitlog))  
	{
		DWORD error = GetLastError();
	}
	delete g_waitReadDialog; 
	g_waitReadDialog = NULL;
	return 0;
}

bool IsFixed(U16 ggaIndicator)
{
	U08 gpsInd, gnssInd;

	if(ggaIndicator > 0xFF)
	{
		gpsInd = ggaIndicator >> 8;
		gnssInd = ggaIndicator & 0xFF;
	}
	else
	{
		gpsInd = ggaIndicator & 0xFF;
		gnssInd = 0xFF;
	}

	return (gpsInd == '1' || gpsInd == '2' || gpsInd=='4' || gpsInd=='5' || gpsInd=='6' || 
			gpsInd == 'A' || gpsInd == 'D' || gpsInd == 'E' ||
			gnssInd == '1' || gnssInd == '2' || gnssInd=='6' || 
			gnssInd == 'A' || gnssInd == 'D' || gnssInd == 'F' || gnssInd == 'R' || gnssInd == 'E');
}

bool CheckInUse(int id, GPGSA* gsa)
{
	bool noUSed = true;
	for(int i=0; i<MAX_SATELLITE; ++i)
	{
		if(id == gsa->SatelliteID[i])
		{
			return false;
		}
	}
	return noUSed;
}

double Deg2Rad(double degree)
{
	return (degree * PI)/180; 
}

double Rad2Deg(double rad)
{
	return (rad * 180)/PI; 
}

float FixedPointToSingle(U32 FixedPointValue, int NumberOfFractionalBits)
{
	int i;
	int is_negative = 0;

	U32 Exponent;
	U32 Mantissa;
	float SingleFloatingPointValue;

	if (FixedPointValue == 0)
	{
		Mantissa = 0;
		SingleFloatingPointValue = 0;
	}
	else
	{
		if ((FixedPointValue & 0x80000000) > 0)
		{
			FixedPointValue = (U32)(-((int)FixedPointValue));
			is_negative = 1;
		}
		for (i = 31; i >= 0; i--)
		{
			if ((FixedPointValue & (0x1L << i)) != 0)
				break;
		}

		//find the shift bit
		int bitsToShift = (i - 23);
		if (bitsToShift > 0)
			FixedPointValue = FixedPointValue >> (bitsToShift);
		else
			FixedPointValue = FixedPointValue << (-bitsToShift);

		Exponent = (U32)(bitsToShift + (23 - NumberOfFractionalBits) + 127);

		Mantissa = (Exponent & 0xff) << 23;

		if (is_negative == 0)
		{
			Mantissa = Mantissa | (U32)(FixedPointValue & 0x7fffff);
		}
		else
		{
			Mantissa = Mantissa | (U32)((FixedPointValue & 0x7fffff) | 0x80000000);
		}
		
		SingleFloatingPointValue = *((float*)(&Mantissa)); 
	}


	return (SingleFloatingPointValue);
}

void COO_geodetic_to_cartesian( const LLA_T* lla_p, POS_T* xyz_p )
{
	D64 temp;
	D64 s_phi = sin(lla_p->lat);
	D64 c_phi = cos(lla_p->lat);

	// radius of curvature in prime vertical
	D64 N = WGS84_RA / sqrt( 1 - WGS84_E2 * s_phi * s_phi );

	xyz_p->px = (temp = (N + lla_p->alt)*c_phi)*cos(lla_p->lon);
	xyz_p->py = temp * sin(lla_p->lon);
	xyz_p->pz = (N * (1 - WGS84_E2) + lla_p->alt) * s_phi;	
}

const S16 DefaultLeapSeconds = 18;	//Updated in 2017/01/05
void  UtcConvertGpsToUtcTime(S16 wn, D64 tow, UtcTime* utc_p)
{
	// GPS Time start at 1980 Jan. 5/6 mid-night
	const S16 INIT_UTC_YEAR = 1980;     
	const S16 DAYS_PER_YEAR = 365;
	const S16 DAYS_PER_4_YEARS = (DAYS_PER_YEAR*4 + 1);  // plus one day for leap year
	const S16 day_of_year_month_table[] = 
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	const S16 day_of_leap_year_month_table[] = 
		{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };
	const S16* month_tbl_p = day_of_year_month_table;

	S32 tow_int = (U32)floor(tow);
	D64 tow_frac = tow - (D64)tow_int;
	S64 total_utc_sec = 604800L * wn + tow_int - DefaultLeapSeconds;
	S32 total_utc_day = (S32)(total_utc_sec / 86400L);
	S32 sec_of_day = (S32)(total_utc_sec - 86400L * total_utc_day);
  //S32 total_utc_day = 7 * wn + tow_int / 86400L;
  //S32 sec_of_day = tow_int -  86400L * (tow_int / 86400L);

	S32 passed_leap_days = (total_utc_day + DAYS_PER_4_YEARS 
			- day_of_leap_year_month_table[2] + 5) / DAYS_PER_4_YEARS;
	S32 passed_utc_years = (total_utc_day + 5 - passed_leap_days) / 365;
	S32 leap_days_of_prev_years = (passed_utc_years + 3) / 4;
	S32 day_of_utc_year = total_utc_day + 5 - passed_utc_years*DAYS_PER_YEAR
		- leap_days_of_prev_years;

	utc_p->year = 1980 + (S16)passed_utc_years;
	utc_p->day_of_year = (S16)day_of_utc_year + 1;
	if((utc_p->year & 0x3) == 0)  // utc->year % 4
		month_tbl_p = day_of_leap_year_month_table;  // this year is leap year
	
	int i = 1;
	for(i = 1; i < 13; ++i)
		if(day_of_utc_year < month_tbl_p[i])
			break;

	utc_p->month = i;
	utc_p->day = (S16)day_of_utc_year - month_tbl_p[i-1] + 1;

	utc_p->hour = (U08)(sec_of_day / 3600);
	if(utc_p->hour > 23)
		utc_p->hour = 23;

	utc_p->minute = (U08)((sec_of_day - utc_p->hour*3600) / 60);
	if(utc_p->minute > 59)
		utc_p->minute = 59;

	utc_p->sec = (F32)(sec_of_day - utc_p->hour*3600L - utc_p->minute*60L)
		+ (F32)tow_frac;
}

const char* DatumList[] = {
	"WGS-84, Global",
	"Adindan, Burkina Faso",
	"Adindan, Cameroon",
	"Adindan, Ethiopia",
	"Adindan, Mali",
	"Adindan, MEAN FOR Ethiopia, Sudan",
	"Adindan, Senegal",
	"Adindan, Sudan",
	"Afgooye, Somalia",
	"Ain el Abd 1970, Bahrain",
	"Ain el Abd 1970, Saudi Arabia",
	"American Samoa 1962, American Samoa Islands",
	"Anna 1 Astro 1965, Cocos Islands",
	"Antigua Island Astro 1943, Antigua (Leeward Islands)",
	"Arc 1950, Botswana",
	"Arc 1950,Burundi",
	"Arc 1950, Lesotho",
	"Arc 1950,Malawi",
	"Arc 1950, MEAN FOR Botswana, Lesotho, Malawi, Swaziland, Zaire, Zambia, Zimbabwe",
	"Arc 1950, Swaziland",
	"Arc 1950, Zaire",
	"Arc 1950, Zambia",
	"Arc 1950, Zimbabwe",
	"Arc 1960, MEAN FOR Kenya, Tanzania",
	"Arc 1960, Kenya",
	"Arc 1960, Taanzania",
	"Ascension Island 1958, Ascension Island",
	"Astro Beacon E 1945,Iwo Jima",
	"Astro DOS 71/4, St Helena Island",
	"Astro Tern Island (FRIG) 1961,Tern Island",
	"Astronomical Station 1952,Marcus Island",
	"Australian Geodetic 1966,Australia, Tasmania",
	"Australian Geodetic 1984,Australia, Tasmania",
	"Ayabelle Lighthouse,Djibouti",
	"Bellevue (IGN),Efate & Erromango Islands",
	"Bermuda 1957, Bermuda",
	"Bissau, Guinea-Bissau",
	"Bogota Observatory, Colombia",
	"Bukit Rimpah, Indonesia (Bangka & Belitung Ids)",
	"Camp Area Astro, Antarctica (McMurdo Camp Area)",
	"Campo Inchauspe,Argentina",
	"Canton Astro 1966, Phoenix Islands",
	"Cape,South Africa",
	"Cape Canaveral,Bahamas, Florida",
	"Carthage,Tunisia",
	"Chatham Island Astro 1971, New Zealand (Chatham Island)",
	"Chua Astro, Paraguay",
	"Corrego Alegre, Brazil",
	"Dabola, Guinea",
	"Deception Island, Deception Island, Antarctia",
	"Djakarta (Batavia),Indonesia (Sumatra)",
	"DOS 1968,New Georgia Islands (Gizo Island)",
	"Easter Island 1967,Easter Island",
	"Estonia, Coordinate System 1937, Estonia",
	"European 1950, Cyprus",
	"European 1950, Egypt",
	"European 1950, England, Channel Islands, Scotland, Shetland Islands",
	"European 1950, England, Ireland, Scotland, Shetland Islands",
	"European 1950, Finland, Norway",
	"European 1950, Greece",
	"European 1950, Iran",
	"European 1950, Italy (Sardinia)",
	"European 1950, Italy (Sicily)",
	"European 1950, Malta",
	"European 1950, MEAN FOR Austria, Belgium, Denmark, Finland, France, W Germany, Gibraltar, Greece, Italy, Luxembourg, Netherlands, Norway, Portugal, Spain, Sweden, Switzerland",
	"European 1950, MEAN FOR Austria, Denmark, France, W Germany, Netherlands, Switzerland",
	"European 1950, MEAN FOR Iraq, Israel, Jordan, Lebanon,Kuwait, Saudi Arabia, Syria",
	"European 1950, Portugal, Spain",
	"European 1950, Tunisia",
	"European 1979, MEAN FOR Austria, Finland, Netherlands, Norway, Spain, Sweden, Switzerland",
	"Fort Thomas 1955, Nevis, St. Kitts (Leeward Islands)",
	"Gan 1970, Republic of Maldives",
	"Geodetic Datum 1949, New Zealand",
	"Graciosa Base SW 1948, Azores (Faial, Graciosa, Pico, Sao Jorge, Terceira)",
	"Guam 1963, Guam",
	"Gunung Segara, Indonesia (Kalimantan)",
	"GUX 1 Astro, Guadalcanal Island",
	"Herat North, Afghanistan",
	"Hermannskogel Datum, Croatia -Serbia, Bosnia-Herzegovina",
	"Hjorsey 1955, Iceland",
	"Hong Kong 1963, Hong Kong",
	"Hu-Tzu-Shan, Taiwan",
	"Indian, Bangladesh",
	"Indian, India, Nepal",
	"Indian, Pakistan",
	"Indian 1954, Thailand",
	"Indian 1960,Vietnam (Con Son Island)",
	"Indian 1960,Vietnam (Near 16oN))",
	"Indian 1975,Thailand",
	"Indonesian 1974, Indonesia",
	"Ireland 1965,Ireland",
	"ISTS 061 Astro 1968,South Georgia Islands",
	"ISTS 073 Astro 1969,Diego Garcia",
	"Johnston Island 1961,Johnston Island",
	"Kandawala, Sri Lanka",
	"Kerguelen Island 1949, Kerguelen Island",
	"Kertau 1948, West Malaysia & Singapore",
	"Kusaie Astro 1951, Caroline Islands",
	"Korean Geodetic System, South Korea",
	"L. C. 5 Astro 1961, Cayman Brac Island",
	"Leigon, Ghana",
	"Liberia 1964, Liberia",
	"Luzon, Philippines (Excluding Mindanao)",
	"Luzon, Philippines (Mindanao)",
	"M'Poraloko, Gabon",
	"Mahe 1971, Mahe Island",
	"Massawa, Ethiopia (Eritrea)",
	"Merchich, Morocco",
	"Midway Astro 1961, Midway Islands",
	"Minna, Cameroon",
	"Minna, Nigeria",
	"Montserrat Island Astro 1958, Montserrat (Leeward Islands)",
	"Nahrwan, Oman (Masirah Island)",
	"Nahrwan, Saudi Arabia",
	"Nahrwan, United Arab Emirates",
	"Naparima BWI, Trinidad & Tobago",
	"North American 1927, Alaska (Excluding Aleutian Ids)",
	"North American 1927, Alaska (Aleutian Ids East of 180oW)",
	"North American 1927, Alaska (Aleutian Ids West of 180oW)",
	"North American 1927, Bahamas (Except San Salvador Id)",
	"North American 1927, Bahamas (San Salvador Island)",
	"North American 1927, Canada (Alberta, British Columbia)",
	"North American 1927, Canada (Manitoba, Ontario)",
	"North American 1927, Canada (New Brunswick, Newfoundland, Nova Scotia, Quebec)",
	"North American 1927, Canada (Northwest Territories, Saskatchewan)",
	"North American 1927, Canada (Yukon)",
	"North American 1927, Canal Zone",
	"North American 1927, Cuba",
	"North American 1927, Greenland (Hayes Peninsula)",
	"North American 1927, MEAN FOR Antigua, Barbados, Barbuda, Caicos Islands, Cuba, Dominican Republic, Grand Cayman, Jamaica, Turks Islands",
	"North American 1927, MEAN FOR Belize, Costa Rica, El Salvador, Guatemala, Honduras, Nicaragua",
	"North American 1927, MEAN FOR Canada",
	"North American 1927, MEAN FOR CONUS",
	"North American 1927, MEAN FOR CONUS (East of Mississippi, River Including Louisiana, Missouri, Minnesota)",
	"North American 1927, MEAN FOR CONUS (West of Mississippi, River Excluding Louisiana, Minnesota, Missouri)",
	"North American 1927, Mexico",
	"North American 1983, Alaska (Excluding Aleutian Ids)",
	"North American 1983, Aleutian Ids",
	"North American 1983, Canada",
	"North American 1983, CONUS",
	"North American 1983, Hawaii",
	"North American 1983, Mexico, Central America",
	"North Sahara 1959, Algeria",
	"Observatorio Meteorologico 1939, Azores (Corvo & Flores Islands)",
	"Old Egyptian 1907, Egypt",
	"Old Hawaiian, Hawaii",
	"Old Hawaiian, Kauai",
	"Old Hawaiian, Maui",
	"Old Hawaiian, MEAN FOR Hawaii, Kauai, Maui, Oahu",
	"Old Hawaiian, Oahu",
	"Oman, Oman",
	"Ordnance Survey Great Britain 1936, England",
	"Ordnance Survey Great Britain 1936, England, Isle of Man, Wales",
	"Ordnance Survey Great Britain 1936, MEAN FOR England, Isle of Man, Scotland, Shetland Islands, Wales",
	"Ordnance Survey Great Britain 1936, Scotland, Shetland Islands",
	"Ordnance Survey Great Britain 1936, Wales",
	"Pico de las Nieves, Canary Islands",
	"Pitcairn Astro 1967, Pitcairn Island",
	"Point, MEAN FOR Burkina Faso & Niger",
	"Pointe Noire 1948, Congo",
	"Porto Santo 1936, Porto Santo, Madeira Islands",
	"Provisional South American 1956, Bolivia",
	"Provisional South American 1956, Chile (Northern, Near 19 oS)",
	"Provisional South American 1956, Chile (Southern, Near 43 oS)",
	"Provisional South American 1956, Colombia",
	"Provisional South American 1956, Ecuador",
	"Provisional South American 1956, Guyana",
	"Provisional South American 1956, MEAN FOR Bolivia, Chile, Colombia, Ecuador, Guyana, Peru, Venezuela",
	"Provisional South American 1956, Peru",
	"Provisional South American 1956, Venezuela",
	"Provisional South Chilean 1963,	 Chile (Near 53 oS) (Hito XVIII)",
	"Puerto Rico, Puerto Rico, Virgin Islands",
	"Pulkovo 1942, Russia",
	"Qatar National, Qatar",
	"Qornoq, Greenland (South)",
	"Reunion, Mascarene Islands",
	"Rome 1940, Italy (Sardinia)",
	"S-42 (Pulkovo 1942), Hungary",
	"S-42 (Pulkovo 1942), Poland",
	"S-42 (Pulkovo 1942), Czechoslavakia",
	"S-42 (Pulkovo 1942), Latvia",
	"S-42 (Pulkovo 1942), Kazakhstan",
	"S-42 (Pulkovo 1942), Albania",
	"S-42 (Pulkovo 1942), Romania",
	"S-JTSK, Czechoslavakia (Prior 1 JAN 1993)",
	"Santo (DOS) 1965, Espirito Santo Island",
	"Sao Braz, Azores (Sao Miguel, Santa Maria Ids)",
	"Sapper Hill 1943, East Falkland Island",
	"Schwarzeck, Namibia",
	"Selvagem Grande 1938, Salvage Islands",
	"Sierra Leone 1960, Sierra Leone",
	"South American 1969, Argentina",
	"South American 1969, Bolivia",
	"South American 1969, Brazil",
	"South American 1969, Chile",
	"South American 1969, Colombia",
	"South American 1969, Ecuador",
	"South American 1969, Ecuador (Baltra, Galapagos)",
	"South American 1969, Guyana",
	"South American 1969, MEAN FOR Argentina, Bolivia, Brazil, Chile, Colombia, Ecuador, Guyana, Paraguay, Peru, Trinidad & Tobago, Venezuela",
	"South American 1969, Paraguay",
	"South American 1969, Peru",
	"South American 1969, Trinidad & Tobago",
	"South American 1969, Venezuela",
	"South Asia, Singapore",
	"Tananarive Observatory 1925, Madagascar",
	"Timbalai 1948, Brunei, E. Malaysia (Sabah Sarawak)",
	"Tokyo, Japan",
	"Tokyo, MEAN FOR Japan, South Korea, Okinawa",
	"Tokyo, Okinawa",
	"Tokyo, South Korea",
	"Tristan Astro 1968, Tristan da Cunha",
	"Viti Levu 1916, Fiji (Viti Levu Island)",
	"Voirol 1960, Algeria",
	"Wake Island Astro 1952, Wake Atoll",
	"Wake-Eniwetok 1960, Marshall Islands",
	"WGS 1972, Global Definition",
	"Yacare, Uruguay",
	"Zanderij, Suriname",
	"Pulkovo 1995, Russia",
	"PZ-90, Global"
};
const int DatumListSize = sizeof(DatumList) / sizeof(DatumList[0]);

U08 Cal_Checksum(U08* pt)
{
	U08 checkSum = 0;
	WORD len = MAKEWORD(pt[3], pt[2]);
	U08 id = pt[4];

	for(int i=0; i<len; ++i)
	{
		checkSum ^= pt[i + 4];
	}

	if(checkSum == pt[len + 4])
	{		
		return id;
	}
	return BINMSG_ERROR;
}

U16 CalCheckSum2(U08* pt)
{
	U08 checkSum = 0;
	WORD len = MAKEWORD(pt[3], pt[2]);
	WORD id = MAKEWORD(pt[4], pt[5]);

	for(int i=0; i<len; ++i)
	{
		checkSum ^= pt[i+4];
	}

	if(checkSum == pt[len + 4])
	{		
		return id;
	}

	return BINMSG_ERROR;
}

void UtcConvertUtcToGpsTime(const UtcTime *utc_time_p, S16 *wn_p, D64 *tow_p )
{
//	const S16 DEFAULT_LEAP_SECS = 14; // GPS-UTC = +14 seconds  
	const S32 SECS_PER_YEAR = 31536000;
	const S32 SECS_PER_LEAP_YEAR = 31622400;
	const S16 day_of_year_table[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	const S16 day_of_leap_year_table[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };
	const S16* month_tbl_p = day_of_year_table; // default is not the leap year              

	S32 int_tow = (S32)(utc_time_p->sec);
	F32 tow_frac = utc_time_p->sec - int_tow;
	S32 passed_years, passed_leap_years;
	S32 total_int_sec;

	passed_years = utc_time_p->year - 1980; // from 1980.01.01 
	passed_leap_years = ( passed_years + 3 )/4;

	if( (utc_time_p->year & 0x3) == 0 )
		month_tbl_p = day_of_leap_year_table; // this year is leap year  

	total_int_sec = (passed_leap_years * SECS_PER_LEAP_YEAR);
	total_int_sec += (passed_years - passed_leap_years) * SECS_PER_YEAR;
	total_int_sec += (month_tbl_p[utc_time_p->month - 1] + utc_time_p->day - 1) * 86400;
	total_int_sec += (utc_time_p->hour * 3600);
	total_int_sec += (utc_time_p->minute * 60);
	total_int_sec += (int_tow);
	total_int_sec += DefaultLeapSeconds; // because GPS-UTC = +14 seconds    
	total_int_sec -= 86400 * 5;          // because from 1980.01.05/06 mid night
	*wn_p = (S16)(total_int_sec / 604800L);
	*tow_p = (D64)(total_int_sec - (*wn_p) * 604800L + tow_frac);  
}

QualityMode GetGnssQualityMode(U32 qualityIndicator, U08 gpMode, U08 glMode, U08 gaMode, U08 bdMode, U08 giMode)
{
/*
  GNS & RMC Mode indicator
  A(1) - Autonomous. Satellites system used in non-differential mode in position fix.
  D(2) - Differential.
  E(6) - Estimated (dead reckoning) Mode.
  F(5) - Float RTK.
  M(7) - Manual Input Mode.
  N(0) - No fix.
  P(?) - Precise. Satellites system used in precision mode. Precision mode is defined as: no delibrate.
  R(4) - Real Time Kinematic. Satellites system used in RTK mode with fixed integers.
  S(8) - Simulator mode.

  In nmea_nostudio.c
  Nmea_qi[FIX_NMODE]={0, 1, 1, 1, 2, 5, 4};
  Nmea_mi[FIX_NMODE]={"N", "E", "A", "A", "D", "F", "R"};

  GGA Mode indicator
  0(N) - Fix not available or invalid. 
  1(A) - GPS SPS Mode, fix valid.
  2(D) - Differential GPS, SPS Mode, fix valid. 
  3(?) - GPS PPS Mode, fix valid.
  4(R) - Real Time Kinematic. Satellites system used in RTK mode with fixed integers.
  5(F) - Float RTK. Satellites system used in RTK mode, floating integers.
  6(E) - Estimated (dead recking) Mode.
  7(M) - Manual Input Mode.
  8(S) - Simulator Mode.

  // Quality Indicator used by GGA
  static const S08 Nmea_quality_indicator[FIX_NMODE] = {
                '0',   // FIX_NONE
                '6',   // FIX_PREDICTION
                '1',   // FIX_2D
                '1',   // FIX_3D
                '2',   // FIX_DIFF
                '5',   // FIX_RTK_FLOAT
                '4',   // FIX_RTK_FIX
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
*/
	QualityMode mode = Unlocated;
	U08 gpInd = 0, glInd = 0;
	U08 gaInd = 0, bdInd = 0;
	if(qualityIndicator > 0xFFFFFF)
	{
		gpInd = HIBYTE(HIWORD(qualityIndicator));
		glInd = LOBYTE(HIWORD(qualityIndicator));
		gaInd = HIBYTE(LOWORD(qualityIndicator));
		bdInd = LOBYTE(LOWORD(qualityIndicator));
	}
	else if(qualityIndicator > 0xFFFF)
	{
		gpInd = LOBYTE(HIWORD(qualityIndicator));
		glInd = HIBYTE(LOWORD(qualityIndicator));
		gaInd = LOBYTE(LOWORD(qualityIndicator));
		bdInd = 0;
	}
	else if(qualityIndicator > 0xFF)
	{
		gpInd = HIBYTE(LOWORD(qualityIndicator));
		glInd = LOBYTE(LOWORD(qualityIndicator));
		gaInd = 0;
		bdInd = 0;
	}
	else
	{
		gpInd = LOBYTE(LOWORD(qualityIndicator));
		glInd = 0;
		gaInd = 0;
		bdInd = 0;
	}

	if(gpInd==0 && glInd==0 && gaInd==0 && bdInd==0)
	{
		return mode;
	}

	if(gpInd=='E' || glInd=='E'|| gaInd=='E'|| bdInd=='E')
	{
		mode = EstimatedMode;
	}
	else if(gpInd=='D' || glInd=='D' || gaInd=='D' || bdInd=='D')
	{
		mode = DgpsMode;
	}
	//else if(gpInd=='1')
	//{
	//	return Unlocated;
	//}
	else if(gpInd=='2')
	{
		mode = DgpsMode;
	}
	else if(gpInd=='3')
	{
		mode = PpsMode;
	}
	else if(gpInd=='4')
	{
		mode = FixRTK;
	}
	else if(gpInd=='5')
	{
		mode = FloatRTK;
	}
	else if(gpInd=='6')
	{
		mode = EstimatedMode;
	}
	else if(gpInd == 'A' || glInd == 'A' || gaInd == 'A' || bdInd == 'A' || gpInd == '1')
	{
		if(gpMode==2 || glMode==2 || bdMode==2|| giMode==2)
		{
			mode = PositionFix2d;
		}
		else if(gpMode==3 || glMode==3 || bdMode==3 || giMode==3)
		{
			mode = PositionFix3d;
		}
		else if(gpMode==4 || glMode==4 || bdMode==4 || giMode==4)
		{
			mode = SurveyIn;
		}
		else if(gpMode==5 || glMode==5 || bdMode==5 || giMode==5)
		{
			mode = StaticMode;
		}
    else
    {
			mode = PositionFix2d;
    }
	}
	else if(gpInd=='N')
	{
		mode = DataNotValid;
	}
	else if(gpInd=='R')
	{
		mode = FixRTK;
	}
	else if(gpInd=='F')
	{
		mode = FloatRTK;
	}

	return mode;
}

D64 ConvertLeonDouble(const U08* ptr)
{
	U08 temp[8] = {0};
	for(int i = 0; i < 8; ++i)
	{
		temp[7 - i] = *ptr++;
	}
	return *((double*)temp);
}

F32 ConvertLeonFloat(const U08* ptr)
{
	U08 temp[4] = {0};
	for(int i = 0; i < 4; ++i)
	{
		temp[3 - i] = *ptr++;
	}
	return *((float*)temp);
}

U16 ConvertLeonU16(const U08* ptr)
{
	return MAKEWORD(ptr[1], ptr[0]);
}

S16 ConvertLeonS16(const U08* ptr)
{
	return (S16)MAKEWORD(ptr[1], ptr[0]);
}

U32 ConvertLeonU32(const U08* ptr)
{
	return MAKELONG(MAKEWORD(ptr[3], ptr[2]), MAKEWORD(ptr[1], ptr[0]));
}

S32 ConvertLeonS32(const U08* ptr)
{
	return (S32)MAKELONG(MAKEWORD(ptr[3], ptr[2]), MAKEWORD(ptr[1], ptr[0]));
}

WlfResult WaitingLoaderFeedback(CSerial* serial, int TimeoutLimit, CWnd* msgWnd)
{
	typedef struct _WlfEntry
	{
		WlfResult result;
		const char* string;
    int len;
	} WlfEntry;

	WlfEntry feedbackTable[] = {
		{ wlf_Ready, "READY", 5},
		{ wlf_Ready1, "READY1", 6},
		{ wlf_Ready2, "READY2", 6},
		{ wlf_error41, "Error41", 7},
		{ wlf_error42, "Error42", 7},
		{ wlf_error43, "Error43", 7},
		//^^^^^^^^^for Loader debug
		{ wlf_error5, "Error5", 6},
		{ wlf_error4, "Error4", 6},
		{ wlf_error3, "Error3", 6},
		{ wlf_error2, "Error2", 6},
		{ wlf_error1, "Error1", 6},
		{ wlf_resendbin, "Resendbin", 9},
		{ wlf_reset, "Reset", 5},
		{ wlf_resend, "RESEND", 6},
		{ wlf_end, "END", 3},
		{ wlf_ok, "OK", 2},
		{ wlf_None, "WAIT\n\r", 6},
		//{ wlf_None, ""},
	};

	WlfResult nReturn = wlf_ok;
	//CString strAckCmd;
	ScopeTimer t;
  U08 buffer[COM_BUFFER_SIZE] = { 0 };
	while(1)
	{
		if(t.GetDuration() > (DWORD)TimeoutLimit && msgWnd != NULL)
		{	//Time Out
			msgWnd->PostMessage(UWM_SETTIMEOUT, t.GetDuration(), 0);
		}

		if(t.GetDuration() > (DWORD)TimeoutLimit)
		{
			nReturn = wlf_timeout;
			break;
		}

		//strAckCmd.Empty();
		DWORD len = serial->GetString(buffer, COM_BUFFER_SIZE, TimeoutLimit - t.GetDuration());
		//strAckCmd.ReleaseBuffer();

		if(!ReadOK(len))
		{	
			continue;
		}

		if(len != 0)
		{
			nReturn = wlf_None;
			int tableSize = sizeof(feedbackTable) / sizeof(feedbackTable[0]);
      CString strAckCmd = (LPCSTR)buffer;
			while(tableSize--)
			{
				//if(0 == strAckCmd.Compare(feedbackTable[tableSize].string)) 
        int find = strAckCmd.Compare(feedbackTable[tableSize].string);
        if(find == 0 && find == strAckCmd.GetLength() - feedbackTable[tableSize].len)
        //if(find == 0)
				{
					nReturn = feedbackTable[tableSize].result;
					break;
				}
			}

			if(wlf_None != nReturn)
			{
				break;
			}
		}
		else
		{
			Sleep(20);
		}
	}

	if(nReturn > wlf_timeout)
	{
		//AfxMessageBox("Unknow Error!");
		Utility::Log(__FUNCTION__, "return", (int)nReturn);
	}
	Utility::Log(__FUNCTION__, "return", (int)nReturn);
	return nReturn;
}
 
bool PreprocessInputLine(U08 *buf, int& bufLen)
{
  U08 *start = NULL, *end = NULL;
  U08* iter = (U08*)(buf + bufLen - 1);
  //Find binary sentence
  do
  {
    if(*iter == 0x0D && *(iter + 1) == 0x0A)
    {
      end = iter + 1;
    }
  
    if(0xA0 == *iter && 0xA1 == *(iter + 1) && end != NULL)
    {
      start = iter;
      break;
    }
  } while(iter-- != buf);

  if(start != NULL && end != NULL)
  {
    memcpy(buf, start, end - start + 1);
    bufLen = end - start + 1;
    buf[bufLen] = 0;
    return true;
  }

  //Find text sentence
  start = NULL;
  end = NULL;
  iter = buf + bufLen - 1;
  do
  {
    if(*iter == 0x0D && *(iter + 1) == 0x0A)
    {
      end = iter + 1;
    }

    if(*iter == '$' && end != NULL)
    {
      start = iter;
      break;
    }
  } while(iter-- != buf);
  if(start != NULL && end != NULL && (end - start) > 7 && (end - start) < 128 && *(end - 4) != '*')
  {
    int a = 0;
  }
  if(start != NULL && end != NULL && (end - start) > 7 && (end - start) < 128 && *(end - 4) == '*')
  {
    memcpy(buf, start, end - start + 1);
    bufLen = end - start + 1;
    buf[bufLen] = 0;
    return true;
  }
  return false;
}

bool ReadOneLineInFile(CFile& f, char* buffer, int size)
{
  char* iter = buffer;
  memset(buffer, 0, size);
  
  if(1 != f.Read(iter++, 1))
  {
    return false;
  }

  while(iter - buffer < size)
  {
    if(1 != f.Read(iter, 1))
    { //End of file
      break;
    }
    if(*iter == 0x0A && *(iter - 1) == 0x0D)
    {
      return true;
    }
    ++iter;
  }
    
  return true;
}

bool IsPrintable(const char c)
{
  return ((c >= 0x20 && c <= 0x7E) || (c == 0x09) || (c == 0x0D) || (c == 0x0A));
}

bool AllPrintable(const char* buffer, int len)
{
  const char* ptr = buffer;
  int printableCount = 0;
  for(int i = 0; i < len; ++i)
  {
    if(IsPrintable(*ptr))
    {
      ++printableCount;
    }
    ++ptr;
  }
  if(printableCount >= (len * 3 / 4))
  {
    return true;
  }
  return false;
}

//---------------------------------------------------------
// GetBitData: get data from big-endian bits in U08 array
//
// Input:   buff - buffer array
//          pos  - bit position from start of data (bits)
//          len  - bit length (bits) (len<=32)
//          dataLen - length of output data

// Output:  data - output data pointr
// Return:  NONE
//---------------------------------------------------------
void GetBitData(U08 buff[], int pos, int len, U08* data, int dataLen)
{
  int i, o;
  if(len <= 0 || dataLen * 8 < len)
  {
    return;
  }

  U08* oPtr = data;
  memset(data, 0, dataLen);
  for(i = (pos + len - 1), o = 0; i >= pos; --i, ++o)
  {
    U08 imask = 1UL << (7 - i % 8);
    U08 omask = 1UL << (o % 8);
    if(buff[i/8] & imask)
    {
      *oPtr |= omask;
    }
    else
    {
      *oPtr &= ~omask;
    }
    if(o % 8 == 7)
    {
      ++oPtr;
    }
  }
}

//---------------------------------------------------------
// GetByteDataFromBE: get data from big-endian bytes in U08 array
//
// Input:   buff - buffer array
//          pos  - byte position from start of data (bytes)
//          len  - byte length (bytes) (len<=32)
//          dataLen - length of output data (bytes)

// Output:  data - output data pointr
// Return:  NONE
//---------------------------------------------------------
void GetByteDataFromBE(U08 buff[], int pos, int len, U08* data, int dataLen)
{
  if(len <= 0 || dataLen < len)
  {
    return;
  }

  memset(data, 0, dataLen);
  int i;
  for(i = pos + len - 1; i >= pos; --i)
  {
    *data = buff[i];
    ++data;
  }
}

//---------------------------------------------------------
// GetByteDataFromBE: get data from little-endian bytes in U08 array
//
// Input:   buff - buffer array
//          pos  - byte position from start of data (bytes)
//          len  - byte length (bytes) (len<=32)
//          dataLen - length of output data (bytes)

// Output:  data - output data pointr
// Return:  NONE
//---------------------------------------------------------
void GetByteDataFromLE(U08 buff[], int pos, int len, U08* data, int dataLen)
{
  int i;
  if(len <= 0 || dataLen < len)
  {
    return;
  }

  memset(data, 0, dataLen);
  for(i = pos; i < pos + len; ++i)
  {
    *data = buff[i];
    ++data;
  }
}

void ConvertInt38Sign(S64* d)
{
  U08* ptr = (U08*)d;
  if(ptr[4] & 0x20)
  {
    ptr[5] = 0xFF;
    ptr[6] = 0xFF;
    ptr[7] = 0xFF;
    ptr[4] |= 0xF0;
  }
}

int GetBitFlagCounts(U08* d, int size)
{  
  int count = 0;
  for(int i = 0; i < size * 8; ++i)
  {
    if((d[i / 8] >> (8 - (i % 8) - 1)) & 0x1)
    {
      ++count;
    }
  }
  return count;
}

void CooCartesianToGeodetic(const POS_T* xyz_p, LLA_T* lla_p)
{
	D64 p;
	D64 theta;
	D64 temp;
	D64 s_theta, c_theta, s_phi, c_phi;
	D64 phi; // latitude

	p = sqrt( (xyz_p->px)*(xyz_p->px) + (xyz_p->py)*(xyz_p->py) );

	if( p <= 0.01f )
	{
		lla_p->lat = (xyz_p->pz >= 0)?(PI/2.0):(-PI/2.0);
		lla_p->lon = 0.0;
		lla_p->alt = (F32)(fabs(xyz_p->pz) - WGS84_RB);
		return;
	}

	theta = atan2( ( xyz_p->pz*WGS84_RA ), ( p*WGS84_RB ) );
	s_theta = sin(theta);
	c_theta = cos(theta);    

	temp = ( xyz_p->pz + WGS84_E2P*WGS84_RB*s_theta*s_theta*s_theta ); 
	phi = atan2( temp, ( p - WGS84_E2*WGS84_RA*c_theta*c_theta*c_theta ) );

	s_phi = sin(phi);
	c_phi = cos(phi);

	lla_p->lat = phi;    
	lla_p->lon = atan2( xyz_p->py, xyz_p->px );
	lla_p->alt = (F32)( (p / c_phi) - ( WGS84_RA / sqrt(1.0 - WGS84_E2*s_phi*s_phi ) ) );
}

void ConvertEcefToLonLatAlt(D64 ecefX, D64 ecefY, D64 ecefZ, D64& latitude, D64& longitude, D64& altitude)
{
	POS_T pos;
	pos.px = (D64)ecefX;
	pos.py = (D64)ecefY;
	pos.pz = (D64)ecefZ;

	LLA_T lla;
	CooCartesianToGeodetic(&pos, &lla);

	lla.lat *= R2D;
	lla.lon *= R2D;
	double lat_d = (S16)fabs(lla.lat );
	double lon_d = (S16)fabs(lla.lon );
	double lat_m = fmod( fabs(lla.lat), 1.0) * 60.0;
	double lon_m = fmod( fabs(lla.lon), 1.0) * 60.0;

	latitude = lat_d * 100.0 + lat_m;
	longitude = lon_d * 100.0 + lon_m;
	altitude = lla.alt;
}

static const U16 crc16tab[256] = {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
    0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
    0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
    0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
    0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
    0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
    0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
    0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
    0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
    0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
    0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
    0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
    0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
    0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
    0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
    0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
    0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
    0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
    0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
    0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
    0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
    0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
    0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
    0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
    0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
    0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
    0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
    0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
    0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
    0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
    0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0 };

//---------------------------------------------------------
// crc16_wi : get the crc_16 value using ccitt standards. (with init input crc value)
//
// Input :
//       buf - a pointer point to the memory for calculate crc value(which type is char)
//       len - the length(bytes) of the memory( not include crc )
//       crc_init
//
// Return : the crc_16 ccitt standard value

U16 crc16_wi(const void *buf, U32 len, U16 crc_init)
{
  U32 i;
  U16 crc = crc_init;
  U08 *ptr;
  ptr = (U08 *)buf; // convert input memory type to char type
  for(i = 0 ; i < len ; ++i)
    crc = ( crc<<8 ) ^ crc16tab[ ( ( crc>>8 ) ^ *ptr++ ) & 0x00FF ];
  return crc;
}

//---------------------------------------------------------
// crc16 : get the crc_16 value using ccitt standards.
//
// Input :
//       buf - a pointer point to the memory for calculate crc value(which type is char)
//       len - the length(bytes) of the memory( not include crc )
// Return : the crc_16 ccitt standard value

U16 GetCrc16(const void *buf, U32 len)
{
  return crc16_wi(buf, len, 0); // initial crc = 0
}

LPCSTR GetU16HexString(const U16 d)
{
  const int DataLen = sizeof(d) * 2;
  static char output[DataLen + 1] = {0};
  for(int i = 0; i < DataLen; ++i)
  {
    int s = (DataLen - i - 1) * 4;
    int c = (d & (0xF << s)) >> s;
    output[i] = (c > 9) ? 'A' + c - 10 : '0' + c;
  }
  output[DataLen] = 0;
  return output;
}

LPCSTR GetBinString(const U32 d, int len)
{
  static char output[33] = {0};
  int i = len - 1;
  U32 mask = 1;
  for(; i >= 0; --i)
  {
    output[len - i - 1] = (d & (mask << i)) ? '1' : '0';
  }
  output[len] = 0;
  return output;
}