#include "StdAfx.h"
#include "log2nmea.h"

#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

const int rmcBufferSize = 1024;
log2nmea::log2nmea(void)
{
}

log2nmea::~log2nmea(void)
{
}

void log2nmea::set_LoggFile(const char *file)
{
	m_filename = file;
}

void log2nmea::parse_symbol(char *src,int src_len,char *dst,int *len_dst,char symbol)
{
	char *pt_line = src;
	int line_size = 0;	
	for(int i = 0 ; i < src_len;i++)
	{
		if (*src != symbol)
		{
			src++;
			line_size++;
		}
		else
		{
			memcpy(dst,pt_line,line_size);
			dst[line_size] = 0;
			//src++;
			//pt_line = src;
			//line_size=0;
			break;
		}
	}
	*len_dst = line_size;
}

void log2nmea::convert2nmea()
{
	//ifstream fsrc;
	//fsrc.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );
	FILE *f = NULL;
	fopen_s(&f, m_filename, "r");

	fseek(f,0,SEEK_END);
	long fsize = ftell(f);
	rewind(f);
	char *buff;
	buff = new char[fsize];
	fread(buff,1,fsize,f);
	fclose(f);

	FILE *fdst = NULL;
	fopen_s(&fdst, "rmc.nmea","w+");

	//char *pt_buf = buff;
	//char *pt_line = buff;
	//int line_size = 0;
	char *ptr_start = &buff[0];
	long bytes = fsize;

	poi_c = 0;
	while(bytes>0)
	{
		char line[1024];
		int read_size=0;
		parse_symbol(ptr_start,bytes,line,&read_size,'\r');
		ptr_start+=(read_size+1);
		bytes-=(read_size+1);
		if (read_size > 0)
		{
			char rmc[rmcBufferSize] = {0};
			if (conv2rmc(line, rmc, rmcBufferSize))
			{
				fwrite(rmc, 1, strlen(rmc), fdst);
			}
		}
		else
		break;
	}

	delete [] buff;
	fclose(fdst);
}

bool log2nmea::conv2rmc(char *src, char *dst, int dstSize)
{
	if ( *src != 'W')
	{
		rmc_nmea rmc;
		parse_rmc(src,&rmc);
		//$GPRMC,104255.001,A,2447.0945,N,12100.5234,E,000.0,000.0,010807,,,A*69
		if (rmc.mode == 3)			// poi
		{
			poi_c++;
			sprintf_s(dst, dstSize,"$GPWPL,%s,%c,%s,%c,%05d*",rmc.lat,rmc.n_s,rmc.lon,rmc.e_w,poi_c);
		}
		else
			sprintf_s(dst, dstSize,"$GPRMC,%s.000,A,%s,%c,%s,%c,%03d.%01d,000.0,%s,,,A*",
				rmc.utctime,rmc.lat,rmc.n_s,rmc.lon,rmc.e_w,
				(int)rmc.speed,int((rmc.speed*10)-(((int)rmc.speed)*10)), rmc.utcdate);
		add_chksum(dst, dstSize);
		return true;
	}
	return false;
}

void log2nmea::parse_rmc(char *src,rmc_nmea *rmc)
{
	/*
	WNO TOW Time Date DECEF_X DECEF_Y DECEF_Z ECEF_X ECEF_Y ECEF_Z Speed Longitude Latitude Altitude
	1438 282101 6:21:28 8/1/2007 0 0 0 -2984962 4966100 2657513 0 121.008714 24.784930 116.575805
	*/

	int index = 0;
	char *ptr = src;
	size_t len = strlen(src);
	
	for(size_t i = 0 ; i < len;i++)
	{
		if(*src != ' ' && i != len-1)
		{
			src++;
		}
		else
		{
			index++;
			src++;
			switch(index)
			{
			case 1:			//WNO
				break;
			case 2:			//TOW
				break;
			case 3:			//Time
				parse_time(ptr, rmc->utctime, sizeof(rmc->utctime));
				break;
			case 4:			//date
				parse_date(ptr,rmc->utcdate, sizeof(rmc->utcdate));
				break;
			case 5:			//DECEF_X
				break;
			case 6:			//DECEF_Y
				break;
			case 7:			//DECEF_Z
				break;
			case 8:			//ECEF_X
				break;
			case 9:			//ECEF_Y
				break;	
			case 10:		//ECEF_Z
				break;
			case 11:		//Speed
				rmc->speed = (float)atof(ptr);
				rmc->speed = (float)(rmc->speed / 1.852);
				break;
			case 12:		//Longitude
				bool ew;
				ew = parse_lonlat(atof(ptr), rmc->lon, sizeof(rmc->lon));
				rmc->e_w = (ew) ? 'E':'W';
				break;
			case 13:		//Latitude
				bool ns;
				ns = parse_lonlat(atof(ptr), rmc->lat, sizeof(rmc->lat));
				rmc->n_s = (ns) ? 'N':'S';
				break;
			case 14:		//Altitude
				break;
			case 15:
				rmc->mode = atoi(ptr);
			}
			ptr = src;
		}
	}
}

void log2nmea::parse_date(char* src, char *dst, int dstSize)
{
	//8/1/2007
	int index = 0;
	char *ptr = src;
	size_t len = strlen(src);
	int MM,DD,YY;
	for(size_t i = 0 ; i < len;i++)
	{
		if(*src != '/' && *src != ' ' )
		{
			src++;
		}
		else
		{
			src++;
			index++;
			switch(index)
			{
			case 1:			//MM
				MM = atoi(ptr);
				break;
			case 2:			//DD
				DD = atoi(ptr);
				break;
			case 3:			//YYYY
				YY = atoi(ptr+2);
				break;
			}
			ptr=src;
		}
		
	}	
	sprintf_s(dst, dstSize,"%02d%02d%02d\0",DD,MM,YY);
}

void log2nmea::parse_time(char* src,char *dst, int dstSize)
{
	//6:21:28
	int index = 0;
	char *ptr = src;
	int len = strlen(src);
	int hh,mm,ss;


	for(int i = 0 ; i < len;i++)
	{
		if(*src != ':' && *src != ' ' )
		{
			src++;
		}
		else
		{
			src++;
			index++;
			switch(index)
			{
			case 1:			//hh
				hh = atoi(ptr);
				break;
			case 2:			//mm
				mm = atoi(ptr);
				break;
			case 3:			//ss
				ss = atoi(ptr);
				break;
			}
			ptr=src;
		}
		
	}	
	sprintf_s(dst, dstSize, "%02d%02d%02d\0",hh,mm,ss);
}

//121.008714 24.784930
bool log2nmea::parse_lonlat(double src, char *dst, int dstSize)
{

	int d = (int)src;
	double m = fmod( src, 1.0 ) * 60.0;
	double tmp = 100*d + m;
	sprintf_s(dst, dstSize, "%.4f\0",abs(tmp));

	return (tmp>=0);
}

void log2nmea::add_chksum(char *src, int size)
{
	int len = strlen(src);
	U08 chk_sum = 0;

	for(int j=1;j<len-1;j++)
	{		
	    //TRACE("%c",*(src+j));
	 	chk_sum^=*(src+j);
	}	
	
	sprintf_s(src, size, "%s%x\r\n\0", src, chk_sum);
}