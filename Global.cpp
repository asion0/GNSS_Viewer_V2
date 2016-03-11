#include "stdafx.h"
#include "Global.h"
#include "WaitReadLog.h"
#include "Monitor_1PPS.h"
#include "NMEA.h"
#include "GPSDlg.h"

const double R2D = 57.2957795131;
const COLORREF g_panelBkColor = RGB(250, 250, 250);
Setting g_setting;

U08 type;
U08 attribute;
U08 msgid;

U08  slgga;	
U08  slgsa;
U08  slgsv;
U08  slgll;
U08  slrmc;
U08  slvtg;
U08  slzda;
U08  slgns;

HANDLE g_connectEvent = NULL;
HANDLE g_closeEvent = NULL;
CWaitReadLog* WRL = NULL;
HANDLE waitlog = NULL;

double pos_lat, pos_lon;
int m_almanac_no;
CString m_almanac_filename;
CMonitor_1PPS *dia_monitor_1pps = NULL;

int FlashBytes[] = { 8*1024, 16*1024, 24*1024, 32*1024 };

int Setting::BaudrateTable[] = {4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
const int Setting::BaudrateTableSize = sizeof(Setting::BaudrateTable) / sizeof(Setting::BaudrateTable[0]);

void Setting::SetBaudrate(int b)
{
	for(int i = 0; i < BaudrateTableSize; ++i)
	{
		if(BaudrateTable[i] == b)
		{
			baudrate = i;
			return;
		}
	}
	baudrate = -1;
}

U32 ConvertCharToU32(const char *src)
{
	U32 resultValue = 0;
	U08 charIter;
	const char *ptr = src;
	int start = strlen(src);
	for (int i=start-1; i>=0 ;--i)
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
	if(WRL != NULL)
	{
		WRL->DoModal();
	}

	if(!ResetEvent(waitlog))  
	{
		DWORD error = GetLastError();
	}
	delete WRL; 
	WRL = NULL;
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
	return degree * (PI/180); 
}

double Rad2Deg(double rad)
{
	return rad * (180/PI); 
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

const S16 DefaultLeapSeconds = 17;	//Updated in 2016/02/03
//UtcConvertGpsToUtcTime
/*
typedef struct UTC_TIME_T {
	S16 year;
	S16 month;
	S16 day;
	S16 day_of_year;
	S16 hour;
	S16 minute;
	F32 sec;
} UtcTime;
typedef struct {
  S16 year;
  S16 month;
  S16 day;
  U08 hour;
  U08 minute;
  F32 sec;
} UTC_T;
*/

//void UtcConvertGpsToUtcTime(S16 wn, D64 tow, UtcTime *utc_time_p)
void  UtcConvertGpsToUtcTime(S16 wn, D64 tow, UtcTime* utc_p)
{
	// default leap secs has passed away
	// const S16 DEFAULT_PRIOR_LEAP_SECS = 17;
	// GPS Time start at 1980 Jan. 5/6 mid-night
	const S16 INIT_UTC_YEAR = 1980;     
	const S16 DAYS_PER_YEAR = 365;
	const S16 DAYS_PER_4_YEARS = (DAYS_PER_YEAR*4 + 1);  // plus one day for leap year
	const S16 day_of_year_month_table[] = 
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	const S16 day_of_leap_year_month_table[] = 
		{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

	//S16 i;
	//S32 tow_int;
	//D64 tow_frac;
	//S32 total_utc_sec, total_utc_day;
	//S32 passed_leap_days, passed_utc_years,
	//	day_of_utc_year, leap_days_of_prev_years;
	//S32 sec_of_day;
	const S16* month_tbl_p = day_of_year_month_table;

	S32 tow_int = (U32)floor(tow);
	D64 tow_frac = tow - (D64)tow_int;
	S32 total_utc_sec = 604800L*wn + tow_int - DefaultLeapSeconds;
	S32 total_utc_day = total_utc_sec / 86400L;
	S32 sec_of_day = total_utc_sec - 86400L * total_utc_day;
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

/*
void UtcConvertGpsToUtcTime(S16 wn, D64 tow, UtcTime *utc_time_p)
{
//	const S16 DEFAULT_LEAP_SECS = 15;           // 2006.01.01 : GPS-UTC = +14 seconds  
	const S16 INIT_UTC_YEAR = 1980;             // GPS Time start at 1980 Jan. 5/6 mid-night    
	const S16 DAYS_PER_YEAR = 365;    
	const S16 DAYS_PER_4_YEARS = ( DAYS_PER_YEAR*4 + 1 ); // plus one day for leap year
	const S16 day_of_year_table[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	const S16 day_of_leap_year_table[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };
	const S16* month_tbl_p = day_of_year_table;           // default is not the leap year              

	S16 i;
	S32 passed_leap_days, passed_utc_years, day_of_utc_year, leap_days_of_passed_utc_years;  
//	S32 ref_time ;              // ICD-200C : p122 ~ p124, decide situation a. or b. or c.             
	D64 tE = wn*604800 + tow;   // GPS time as estimated by the user.
	D64 delta_t_UTC = 0;        // leap sec calculated from sub4-p18
	D64 double_total_utc_sec;   
	S32 int_total_utc_sec;
	D64 tow_frac;
	S32 total_utc_day;
	S32 sec_of_day;  

	// ( ICD-200C p123, situation b. ) DN + 3/4 ~ DN +5/4
	// step 1 : decide leap sec, total_utc_day, sec of day 
	double_total_utc_sec = tE - DefaultLeapSeconds;
	int_total_utc_sec = (S32)double_total_utc_sec;
	tow_frac = double_total_utc_sec - (D64)int_total_utc_sec;
	total_utc_day = int_total_utc_sec / 86400L;
	sec_of_day = int_total_utc_sec - 86400L*total_utc_day;          

	// step 2 : calculate utc time    
	passed_leap_days = 1 + ( (total_utc_day + 5 - day_of_leap_year_table[2] ) / DAYS_PER_4_YEARS );
	passed_utc_years = (total_utc_day + 5 - passed_leap_days) / 365;
	leap_days_of_passed_utc_years = (passed_utc_years + 3) / 4;

	day_of_utc_year = total_utc_day + 5 - passed_utc_years*DAYS_PER_YEAR
		- leap_days_of_passed_utc_years;
	utc_time_p->day_of_year = (S16)day_of_utc_year + 1;                  
	utc_time_p->year = INIT_UTC_YEAR + (S16)passed_utc_years;

	if( (utc_time_p->year & 3) == 0 )        
		month_tbl_p = day_of_leap_year_table; // this year is leap year

	for( i = 1 ; i < 13 ; i++ )
		if( day_of_utc_year < month_tbl_p[i] )
			break;

	utc_time_p->month = i;
	utc_time_p->day   = (S16)day_of_utc_year - month_tbl_p[i-1] + 1; // + 1 : because of having sec_of_day

	utc_time_p->hour = (S16)(sec_of_day / 3600);
	if( utc_time_p->hour > 23 )
		utc_time_p->hour = 23;   

	utc_time_p->minute = (S16)((sec_of_day - utc_time_p->hour * 3600) / 60);
	if( utc_time_p->minute > 59 )
		utc_time_p->minute = 59;   

	utc_time_p->sec = (F32)(sec_of_day - utc_time_p->hour*3600L - utc_time_p->minute*60L) + (F32)tow_frac;
}
*/
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

void UtcConvertUtcToGpsTime( const UtcTime *utc_time_p, S16 *wn_p, D64 *tow_p )
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
	passed_leap_years = ( passed_years + 3 )/4 ;

	if( (utc_time_p->year & 0x3) == 0 )
		month_tbl_p = day_of_leap_year_table; // this year is leap year  

	total_int_sec = ( passed_leap_years*SECS_PER_LEAP_YEAR );
	total_int_sec += ( passed_years - passed_leap_years )*SECS_PER_YEAR;
	total_int_sec += ( month_tbl_p[ utc_time_p->month - 1 ] + utc_time_p->day - 1 )*86400;
	total_int_sec += ( utc_time_p->hour*3600 ) ;
	total_int_sec += ( utc_time_p->minute*60 ) ;
	total_int_sec += ( int_tow ) ;
	total_int_sec +=  DefaultLeapSeconds; // because GPS-UTC = +14 seconds    
	total_int_sec -= 86400*5;          // because from 1980.01.05/06 mid night
	*wn_p = (S16)(total_int_sec / 604800L);
	*tow_p = (D64)( total_int_sec - (*wn_p)*604800L + tow_frac );  
}

QualityMode GetGnssQualityMode(U32 qualityIndicator, U08 gpMode, U08 glMode, U08 gaMode, U08 bdMode)
{
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

	if(gpInd || glInd || gaInd || bdInd)
	{
		if(gpInd=='E' || glInd=='E'|| gaInd=='E'|| bdInd=='E')
		{
			mode = EstimatedMode;
		}
		else if(gpInd=='D' || glInd=='D' || gaInd=='D' || bdInd=='D')
		{
			mode = DgpsMode;
		}
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
		else if(gpInd == 'A' || glInd == 'A' || gaInd == 'A' || bdInd == 'A' || gpInd == '1')
		{
			if(gpMode==2 || glMode==2 || bdMode==2)
			{
				mode = PositionFix2d;
			}
			else if(gpMode==3 || glMode==3 || bdMode==3)
			{
				mode = PositionFix3d;
			}
			else if(gpMode==4 || glMode==4 || bdMode==4)
			{
				mode = SurveyIn;
			}
			else if(gpMode==5 || glMode==5 || bdMode==5)
			{
				mode = StaticMode;
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
	}
	return mode;
}

double ConvertLeonDouble(const U08* ptr)
{
	U08 temp[8] = {0};
	for(int i = 0; i < 8; ++i)
	{
		temp[7 - i] = *ptr++;
	}
	return *((double*)temp);
}

float ConvertLeonFloat(const U08* ptr)
{
	U08 temp[4] = {0};
	for(int i = 0; i < 4; ++i)
	{
		temp[3 - i] = *ptr++;
	}
	return *((float*)temp);
}