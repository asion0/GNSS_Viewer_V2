#include "stdafx.h"
#include "GPS.h"
#include "BinaryMSG.h"
#include "GPSDlg.h"

extern void add2message(char* buffer, int offset);

static char g_msgBuff[512] = {0};

void ECEF_USER_PVT_PROC(ECEF_USER_PVT& ecef_user_pvt, U08* pt)
{
	memcpy(&ecef_user_pvt.MessageID    , &pt[4],  sizeof(U08));
	memcpy(&ecef_user_pvt.X            , &pt[5],  sizeof(S32));
	memcpy(&ecef_user_pvt.Y            , &pt[9],  sizeof(S32));
	memcpy(&ecef_user_pvt.Z            , &pt[13], sizeof(S32));
	memcpy(&ecef_user_pvt.XV           , &pt[17], sizeof(S16));
	memcpy(&ecef_user_pvt.YV           , &pt[19], sizeof(S16));
	memcpy(&ecef_user_pvt.ZV           , &pt[21], sizeof(S16));
	memcpy(&ecef_user_pvt.PositionMode , &pt[23], sizeof(U08));
	memcpy(&ecef_user_pvt.FixMode      , &pt[24], sizeof(U08));
	memcpy(&ecef_user_pvt.GPSWeek      , &pt[25], sizeof(U16));
	memcpy(&ecef_user_pvt.TOW          , &pt[27], sizeof(U32));
	memcpy(&ecef_user_pvt.Year         , &pt[31], sizeof(U16));
	memcpy(&ecef_user_pvt.Month        , &pt[33], sizeof(U08));
	memcpy(&ecef_user_pvt.Day          , &pt[34], sizeof(U08));
	memcpy(&ecef_user_pvt.Hour         , &pt[35], sizeof(U16));
	memcpy(&ecef_user_pvt.Min          , &pt[37], sizeof(U08));
	memcpy(&ecef_user_pvt.Sec          , &pt[38], sizeof(U08));
	memcpy(&ecef_user_pvt.NumOfSV      , &pt[39], sizeof(U08));
	memcpy(&ecef_user_pvt.NumOfSVinFix , &pt[41], sizeof(U08));
	memcpy(&ecef_user_pvt.GDOP         , &pt[41], sizeof(U08));
	memcpy(&ecef_user_pvt.PDOP         , &pt[42], sizeof(U08));
	memcpy(&ecef_user_pvt.HDOP         , &pt[43], sizeof(U08));
	memcpy(&ecef_user_pvt.VDOP         , &pt[44], sizeof(U08));
    memcpy(&ecef_user_pvt.TDOP         , &pt[45], sizeof(U08));    
}

void GEODETIC_USER_PVT_PROC(GEODETIC_USER_PVT& geod_user_pvt,U08* pt)
{
	memcpy(&geod_user_pvt.MessageID    , &pt[4],  sizeof(U08));
	memcpy(&geod_user_pvt.Latitude     , &pt[5],  sizeof(S32));	
	memcpy(&geod_user_pvt.Longitude    , &pt[9],  sizeof(S32));
	memcpy(&geod_user_pvt.Altitude     , &pt[13], sizeof(S16));
	memcpy(&geod_user_pvt.Heading      , &pt[15], sizeof(U16));
	memcpy(&geod_user_pvt.Speed        , &pt[17], sizeof(U16));
	memcpy(&geod_user_pvt.PositionMode , &pt[19], sizeof(U08));
	memcpy(&geod_user_pvt.FixMode      , &pt[20], sizeof(U08));
	memcpy(&geod_user_pvt.GPSWeek      , &pt[21], sizeof(U16));
	memcpy(&geod_user_pvt.TOW          , &pt[23], sizeof(U32));
	memcpy(&geod_user_pvt.Year         , &pt[27], sizeof(U16));
	memcpy(&geod_user_pvt.Month        , &pt[29], sizeof(U08));
	memcpy(&geod_user_pvt.Day          , &pt[30], sizeof(U08));
	memcpy(&geod_user_pvt.Hour         , &pt[31], sizeof(U16));
	memcpy(&geod_user_pvt.Min          , &pt[33], sizeof(U08));
	memcpy(&geod_user_pvt.Sec          , &pt[34], sizeof(U08));
	memcpy(&geod_user_pvt.NumOfSV      , &pt[35], sizeof(U08));
	memcpy(&geod_user_pvt.NumOfSVinFix , &pt[36], sizeof(U08));
	memcpy(&geod_user_pvt.GDOP         , &pt[37], sizeof(U08));
	memcpy(&geod_user_pvt.PDOP         , &pt[38], sizeof(U08));
	memcpy(&geod_user_pvt.HDOP         , &pt[39], sizeof(U08));
	memcpy(&geod_user_pvt.VDOP         , &pt[40], sizeof(U08));
	memcpy(&geod_user_pvt.TDOP         , &pt[41], sizeof(U08));   
	geod_user_pvt.Lat = (float)geod_user_pvt.Latitude/10000000;
	geod_user_pvt.Lon = (float)geod_user_pvt.Longitude /10000000;
}

void USER_SATELLITE_INFOMATION_PROC(USER_SATELLITE_INFOMATION& sv_info,U08* pt)
{
	memcpy(&sv_info.MessageID    , &pt[4],   sizeof(U08));
	int index=5;
	for(int i=0;i<12;i++)
	{
		memcpy(&sv_info.sv[i].prn    , &pt[index], sizeof(U08));
		index+=sizeof(U08);
	    memcpy(&sv_info.sv[i].health , &pt[index], sizeof(U08));
		index+=sizeof(U08);
	    memcpy(&sv_info.sv[i].azm    , &pt[index], sizeof(U16));
		index+=sizeof(U16);
	    memcpy(&sv_info.sv[i].elv    , &pt[index], sizeof(U08));
		index+=sizeof(U08);
	    memcpy(&sv_info.sv[i].cn0    , &pt[index], sizeof(U08));
		index+=sizeof(U08);
	    memcpy(&sv_info.sv[i].stu    , &pt[index], sizeof(U16));
		index+=sizeof(U16);	
	}	
	memcpy(&sv_info.GPSWeek       , &pt[101], sizeof(U16));
	memcpy(&sv_info.TOW           , &pt[103], sizeof(U32));
}

void SATELLITE_MEASUREMENT_DATA_PROC(SATELLITE_MEASUREMENT_DATA& sm_data,U08* pt)
{
	memcpy(&sm_data.MessageID     , &pt[4],    sizeof(U08));

	int index=5;
	for(int i=0;i<12;i++)
	{
		memcpy(&sm_data.smd[i].prn, &pt[index], sizeof(U16));
		index+=sizeof(U16);
	    memcpy(&sm_data.smd[i].PR,  &pt[index], sizeof(S32));
		index+=sizeof(S32);
	    memcpy(&sm_data.smd[i].DR,  &pt[index], sizeof(S32));
		index+=sizeof(S32);
	    memcpy(&sm_data.smd[i].Dp,  &pt[index], sizeof(U16));
		index+=sizeof(S16);
	}
	memcpy(&sm_data.GPSWeek       , &pt[149],  sizeof(U16));
	memcpy(&sm_data.TOW           , &pt[151],  sizeof(U32));
	memcpy(&sm_data.ClockOffset   , &pt[155],  sizeof(U16));
}

void SHOW_ECEF_USER_PVT(ECEF_USER_PVT& ecef_user_pvt,U08* pt,int len)
{
	ECEF_USER_PVT_PROC(ecef_user_pvt,pt);
}


void SHOW_GEODETIC_USER_PVT(GEODETIC_USER_PVT& geod_user_pvt,U08* pt,int len)
{	
	GEODETIC_USER_PVT_PROC(geod_user_pvt,pt);
}

void SHOW_USER_SATELLITE_INFOMATION(USER_SATELLITE_INFOMATION& sv_info,U08* pt,int len)
{   
	USER_SATELLITE_INFOMATION_PROC(sv_info,pt);
}

void SHOW_SATELLITE_MEASUREMENT_DATA(SATELLITE_MEASUREMENT_DATA& sm_data,U08* pt,int len)
{	
	SATELLITE_MEASUREMENT_DATA_PROC(sm_data,pt);
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
		lla_p->lat = (xyz_p->pz >= 0)?(ICD_PI/2.0):(-ICD_PI/2.0);
		lla_p->lon = 0.0;
		lla_p->alt = (F32)(fabs(xyz_p->pz) - RB);
		return;
	}

	theta = atan2( ( xyz_p->pz*RA ), ( p*RB ) );
	s_theta = sin(theta);
	c_theta = cos(theta);    

	temp = ( xyz_p->pz + E2P*RB*s_theta*s_theta*s_theta ); 
	phi = atan2( temp, ( p - E2*RA*c_theta*c_theta*c_theta ) );

	s_phi = sin(phi);
	c_phi = cos(phi);

	lla_p->lat = phi;    
	lla_p->lon = atan2( xyz_p->py, xyz_p->px );
	lla_p->alt = (F32)( (p / c_phi) - ( RA / sqrt(1.0 - E2*s_phi*s_phi ) ) );
}

U08 *decode_4bytes(U08 *src,U32 *dst)
{
	*dst = src[0]<<24 | src[1]<<16 | src[2]<<8 | src[3];
	src+=4;
	return src;
}

U08 *decode_2bytes(U08 *src,U16 *dst)
{
	*dst = src[0]<<8 | src[1];
	src+=2;
	return src;
}

U08 *decode_1bytes(U08 *src,U08 *dst)
{
	*dst = src[0];
	src+=1;
	return src;
}



extern Satellite satellites_gps[MAX_SATELLITE];
extern Satellite satellites_gnss[MAX_SATELLITE];

void ShowMeasurementChannel(U08* src, bool convertOnly, CString* pStr)
{
	U08 nmeas = src[6];
	U08* ptr = &src[7];
	CHANNEL_DATA_T channel = {0};
	U32 tmp[2];
	for (int i=0; i<nmeas; ++i)
	{
		ptr = decode_1bytes(ptr, &channel.prn);
		ptr = decode_1bytes(ptr, &channel.cn0);
		ptr = decode_4bytes(ptr, &tmp[1]);
		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.pseudo_range, tmp, sizeof(D64));

		ptr = decode_4bytes(ptr, &tmp[1]);
		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.carrier_cycle,tmp, sizeof(D64));

		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.doppler_freq, tmp, sizeof(F32));

		ptr = decode_1bytes(ptr, &channel.indicator);

		int len = sprintf_s(g_msgBuff, sizeof(g_msgBuff), 
			"$SVID=%d,cn0=%d,pseudo_range=%.3f,carrier_cycle=%.3f,doppler_freq=%.0f,indicator=%d",
			channel.prn, channel.cn0, channel.pseudo_range, channel.carrier_cycle, 
			channel.doppler_freq, channel.indicator);
		if(convertOnly && pStr)
		{
			*pStr += g_msgBuff;
			*pStr += "\r\n";
			continue;
		}
		add2message(g_msgBuff, len);
	}
	return;
}

void ShowSubframe(U08 *src, bool convertOnly, CString* pStr)
{
	U08 msgType = src[4];
	LPCSTR subFrameType = NULL;
	if(msgType==0xE0)
	{
		subFrameType = "GPS_SUBFRAME";
	}
	else if(msgType==0xE1)
	{
		subFrameType = "GLONASS_STRING";
	}	
	else if(msgType==0xE2)
	{
		subFrameType = "BEIDOU2_D1_SUBFRAME";
	}	
	else if(msgType==0xE3)
	{
		subFrameType = "BEIDOU2_D2_SUBFRAME";
	}	
	else
	{
		subFrameType = "Unknown_SUBFRAME";
	}

	U08 prn = src[5];
	U08 subFrmId = src[6];
	sprintf_s(g_msgBuff, sizeof(g_msgBuff), 
		"$%s,SVID=%d, SFID=%d, data=",
		subFrameType, prn, subFrmId);

	WORD packetLen = MAKEWORD(src[3], src[2]);
	for(int i=3; i<packetLen; ++i)
	{
		char tmpBuff[10];
		sprintf_s(tmpBuff, sizeof(tmpBuff), "%02X ", src[4+i]);
		strcat_s(g_msgBuff, sizeof(g_msgBuff), tmpBuff);
	}

	int len = strlen(g_msgBuff);
	if(convertOnly)
	{
		if(pStr)
		{
			*pStr += g_msgBuff;
			*pStr += "\r\n";
		}
		return;
	}
	add2message(g_msgBuff, len);
}

void ShowMeasurementTime(U08 *src, bool convertOnly, CString* pStr)
{
	U16 wn;
	U32 tow;
	U16 period;
	U08 *ptr = &src[6];
	ptr = decode_2bytes(ptr, &wn);
	ptr = decode_4bytes(ptr, &tow);
	ptr = decode_2bytes(ptr, &period);
	
	int len = sprintf_s(g_msgBuff, sizeof(g_msgBuff),
		"$wn=%d,tow=%d,period=%d", 
		wn, tow, period);

	if(convertOnly && pStr)
	{
		*pStr = g_msgBuff;
		*pStr += "\r\n";
		return;
	}

	add2message(g_msgBuff, len);

	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, (tow / 1000.0), &utc);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;
	return;
}

void ShowMeasurementSv(U08 *src, bool convertOnly, CString* pStr)
{
	int fixed_gps_c = 0, fixed_glonass_c = 0, fixed_beidou_c = 0, fixed_galileo_c = 0;
	int gps_c = 0, glonass_c = 0, beidou_c = 0, galileo_c = 0;

	U08 svs = src[6];
	U08 *ptr = &src[7];
	SV_CH_DATA_T sv;

	if(!convertOnly)
	{
		memset(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID));
	}

	for (int i=0; i<svs; ++i)
	{
		ptr = decode_1bytes(ptr, &sv.channel_id);
		ptr = decode_1bytes(ptr, &sv.prn);
		ptr = decode_1bytes(ptr, &sv.SV_status);
		ptr = decode_1bytes(ptr, &sv.URA);
		ptr = decode_1bytes(ptr, (U08*)&sv.cn0);
		ptr = decode_2bytes(ptr, (U16*)&sv.elevation);
		ptr = decode_2bytes(ptr, (U16*)&sv.azimuth);
		ptr = decode_1bytes(ptr, &sv.channel_status);

		int len = sprintf_s(g_msgBuff, sizeof(g_msgBuff), 
			"$channel_id=%d,SVID=%d,SV_status=%d,URA=%d,cn0=%d,elevation=%d,azimuth=%d,channel_status=%d",
			sv.channel_id, sv.prn, sv.SV_status, sv.URA,sv.cn0, 
			sv.elevation, sv.azimuth, sv.channel_status);

		if(convertOnly)
		{
			if(pStr)
			{
				*pStr += g_msgBuff;
				*pStr += "\r\n";
			}
			continue;
		}

		if(convertOnly)
		{
			return;
		}

		add2message(g_msgBuff, len);
		if(NMEA::Glonass == NMEA::GetGNSSSystem(sv.prn))
		{
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID[fixed_glonass_c] = sv.prn;
				fixed_glonass_c++;
			}
			satellites_gnss[glonass_c].SatelliteID = sv.prn;
			satellites_gnss[glonass_c].SNR = sv.cn0;
			satellites_gnss[glonass_c].Elevation = sv.elevation;
			satellites_gnss[glonass_c].Azimuth = sv.azimuth;
			glonass_c++;
		}
		else if(NMEA::Gps == NMEA::GetGNSSSystem(sv.prn))
		{
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID[fixed_gps_c] = sv.prn;
				fixed_gps_c++;
			}
			satellites_gps[gps_c].SatelliteID = sv.prn;
			satellites_gps[gps_c].SNR = sv.cn0;
			satellites_gps[gps_c].Elevation = sv.elevation;
			satellites_gps[gps_c].Azimuth = sv.azimuth;
			gps_c++;
		}
		else if(NMEA::Beidou == NMEA::GetGNSSSystem(sv.prn))
		{
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID[fixed_beidou_c] = sv.prn;
				fixed_beidou_c++;
			}
			satellites_bd[beidou_c].SatelliteID = sv.prn;
			satellites_bd[beidou_c].SNR = sv.cn0;
			satellites_bd[beidou_c].Elevation = sv.elevation;
			satellites_bd[beidou_c].Azimuth = sv.azimuth;
			beidou_c++;
		}
		else if(NMEA::Galileo == NMEA::GetGNSSSystem(sv.prn))
		{
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID[fixed_galileo_c] = sv.prn;
				fixed_galileo_c++;
			}
			satellites_bd[galileo_c].SatelliteID = sv.prn;
			satellites_bd[galileo_c].SNR = sv.cn0;
			satellites_bd[galileo_c].Elevation = sv.elevation;
			satellites_bd[galileo_c].Azimuth = sv.azimuth;
			galileo_c++;
		}
	}
	
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = gps_c;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = (gps_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = (gps_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = fixed_gps_c + fixed_glonass_c;

	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfSate = glonass_c;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfMessage = (glonass_c + 3) / 4;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.SequenceNum = (glonass_c + 3) / 4;

	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfSate = beidou_c;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfMessage = (beidou_c + 3) / 4;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.SequenceNum = (beidou_c + 3) / 4;
}

void ShowReceiverNav(U08 *src, bool convertOnly, CString* pStr)
{
	U08 *ptr = &src[6];
	U32 tmp[2];
	RECEIVER_NAV_DATA_T receiver;

	ptr = decode_1bytes(ptr, &receiver.nav_status);
	ptr = decode_2bytes(ptr, &receiver.wn);
	
	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.tow, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_x, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_y, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_z, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_vx, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_vy, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_vz, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.clock_bias, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.clock_drift, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.gdop, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.pdop, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.hdop, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.vdop, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.tdop, tmp, sizeof(F32));


	int len = sprintf_s(g_msgBuff, sizeof(g_msgBuff), 
		"$navigation_status=%d,wn=%d,tow=%f,ecef_x=%f,ecef_y=%f,ecef_z=%f,ecef_vx=%f,ecef_vy=%f,ecef_vz=%f,clock_bias=%f,clock_drift=%f,gdop=%f,pdop=%f,hdop=%f,vdop=%f,tdop=%f",
		receiver.nav_status, receiver.wn, receiver.tow, 
		receiver.ecef_x, receiver.ecef_y, receiver.ecef_z,
		receiver.ecef_vx, receiver.ecef_vy, receiver.ecef_vz,
		receiver.clock_bias, receiver.clock_drift,
		receiver.gdop, receiver.pdop, receiver.hdop, 
		receiver.vdop, receiver.tdop);

	if(convertOnly)
	{
		if(pStr)
		{
			*pStr += g_msgBuff;
			*pStr += "\r\n";
		}
		return;
	}

	add2message(g_msgBuff, len);

	switch(receiver.nav_status)
	{
	case 0:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '0';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 1;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'V';
		break;
	case 1:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 2;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
		break;
	case 2:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 2;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
		break;
	case 3:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
		break;
	case 4:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '2';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'D';
		break;
	}

	POS_T pos;
	pos.px = receiver.ecef_x;
	pos.py = receiver.ecef_y;
	pos.pz = receiver.ecef_z;

	LLA_T lla;
	CooCartesianToGeodetic(&pos, &lla);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = lla.lat * R2D * 100;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = (lla.lat >= 0) ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = lla.lon * R2D * 100;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = (lla.lon >= 0) ? 'E' : 'W';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = lla.alt;

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = receiver.pdop;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.HDOP = receiver.hdop;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = receiver.vdop;
}

void ShowBinaryOutput(U08* src, bool convertOnly, CString* pStr)
{
	U16 wn = src[7] << 8 | src[8];
	U32 tow = src[9]<<24 | src[10]<<16 | src[11]<<8 | src[12];
	D64 ftow = (double)tow / 100.0;

	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, ftow, &utc);


	double lat = src[13]<<24 | src[14]<<16 | src[15]<<8 | src[16];
	double lon = src[17]<<24 | src[18]<<16 | src[19]<<8 | src[20];

	D64 flat = lat ;
	flat /= 10000000;
	flat =((int)flat)*100 +(double)(flat - (int)flat)*60;

	D64 flon = lon;
	flon /= 10000000;
	flon = ((int)flon)*100 +(double)(flon - (int)flon)*60;

	U32 alt_t =src[21]<<24 | src[22]<<16 | src[23]<<8 | src[24];
	U32 alt = src[25]<<24 | src[26]<<16 | src[27]<<8 | src[28];


	U32 gdop = src[29]<<8 | src[30];
	U32 pdop = src[31]<<8 | src[32];
	U32 hdop = src[33]<<8 | src[34];
	U32 vdop = src[35]<<8 | src[36];
	U32 tdop = src[37]<<8 | src[38];

	S32 ecefx = src[39]<<24 | src[40]<<16 | src[41]<<8 | src[42];
	S32 ecefy = src[43]<<24 | src[44]<<16 | src[45]<<8 | src[46];
	S32 ecefz = src[47]<<24 | src[48]<<16 | src[49]<<8 | src[50];

	S32 vx = src[51]<<24 | src[52]<<16 | src[53]<<8 | src[54];
	S32 vy = src[55]<<24 | src[56]<<16 | src[57]<<8 | src[58];
	S32 vz = src[59]<<24 | src[60]<<16 | src[61]<<8 | src[62];

	D64 fx = vx / 100;
	D64 fy = vy / 100;
	D64 fz = vz / 100;

	D64 fv = sqrt(fx * fx + fy * fy + fz * fz);

	char m_buff[512];
	int msg_len;

	msg_len = sprintf_s(m_buff, "$fix mode=%d", src[5]);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff, "$number if sv in fix=%d", src[6]);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff, "$GPS Week=%d", wn);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff,"$GPS TOW=%.2f",ftow);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}

	msg_len = sprintf_s(m_buff,"$latitude=%.6f",flat);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}

	msg_len = sprintf_s(m_buff,"$longutide=%.6f",flon);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}

	msg_len = sprintf_s(m_buff,"$ellipsoid altitude=%.2f", (F32)alt_t / 100);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}

	msg_len = sprintf_s(m_buff,"$sea level altitude=%.2f", (F32)alt);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}

	msg_len = sprintf_s(m_buff,"$gdop=%.2f",(F32)gdop/100);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff,"$pdop=%.2f",(F32)pdop/100);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff,"$hdop=%.2f",(F32)hdop/100);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff,"$tdop=%.2f",(F32)tdop/100);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}

	msg_len = sprintf_s(m_buff,"$ecef-x=%.2f",(double)ecefx/100);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff,"$ecef-y=%.2f",(double)ecefy/100);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff,"$ecef-z=%.2f",(double)ecefz/100);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}

	msg_len = sprintf_s(m_buff,"$ecef-vx=%.2f",fx);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff,"$ecef-vy=%.2f",fy);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}
	msg_len = sprintf_s(m_buff,"$ecef-vz=%.2f",fz);
	if(convertOnly)
	{
		*pStr += m_buff;
		*pStr += "\r\n";
	}
	else
	{
		add2message(m_buff, msg_len);
	}

	if(convertOnly)
	{
		return;
	}

	if(src[5]==0)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '0';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 1;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'V';
	}
	else if(src[5] ==1)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 2;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[5] == 2)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[5] == 3)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '2';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = src[6];

	volatile U16 i_sec;
	i_sec = (U16)(utc.sec * 1000);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = i_sec*0.001f;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = i_sec * 0.001f;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = flat;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = flat >= 0 ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = flon;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = flon >= 0 ? 'E' : 'W';

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Latitude = flat;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Latitude_N_S = flat >= 0 ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Longitude = flon;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Longitude_E_W = flon >= 0 ? 'E' : 'W';

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = (F32)alt;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude /= 100.0F;

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = (F32)pdop/100;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.HDOP = (F32)hdop/100;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = (F32)vdop/100;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.SpeedKnots = (F32)(fv / 1.85567F);
}

