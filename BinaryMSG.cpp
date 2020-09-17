#include "stdafx.h"
#include "GPS.h"
#include "BinaryMSG.h"
#include "GPSDlg.h"

struct CHANNEL_DATA
{
	U08 prn;
	U08 cn0;
	D64 pseudo_range;
	D64 carrier_cycle;
	F32 doppler_freq;
	U08 indicator;
};

struct ExtMeasChannelData
{
	U08 typeNsingel;
	U08 svid;
	U08 freqIdNlockTimeInd;
	U08 cn0;
  D64 pseduRange;
  D64 accCarrierCycle;
	F32 dopplerFreq;
	U08 prStdDeviation;
	U08 accStdDeviation;
	U08 dfDeviation;
	U16 chInd;
};

struct SV_CH_DATA
{
	U08 channel_id;
	U08 prn;
	U08 SV_status;
	U08 URA;
	S08 cn0;
	S16 elevation;
	S16 azimuth;
	U08 channel_status;
};

struct GnssSvChannelData
{
	U08 channelId;
	U08 gnssTypeSignal;
  U08 svid;
	U08 svStatusInd;
	U08 ura;
	S08 cn0;
	U08 chStatusInd;
};

struct RECEIVER_NAV_DATA
{
	U08 nav_status;
	U16 wn;
	D64 tow;
	D64 ecef_x;
	D64 ecef_y;
	D64 ecef_z;
	F32 ecef_vx;
	F32 ecef_vy;
	F32 ecef_vz;
	D64 clock_bias;
	F32 clock_drift;
	F32 gdop;
	F32 pdop;
	F32 hdop;
	F32 vdop;
	F32 tdop;
};

extern void add2message(const char* buffer, int offset);
extern void SetListCtrlRedraw(BOOL b);

static CString strBuffer("", 512);

static void DisplayAndSave(bool convertOnly, CString* pStr, const char* buf, int len)
{
  const char eol[] = "\r\n";
	if(convertOnly)
	{
    if(pStr)
    {
		  *pStr += buf;
		  *pStr += eol;
    }
	}
  else
  {
    add2message(buf, len);
    CSaveNmea::SaveText(buf, len);
    CSaveNmea::SaveText(eol, 2);
  }
}

static U08* decode_4bytes(U08* src, U32* dst)
{
	*dst = src[0]<<24 | src[1]<<16 | src[2]<<8 | src[3];
	src += 4;
	return src;
}

static U08* decode_2bytes(U08* src, U16* dst)
{
	*dst = src[0]<<8 | src[1];
	src += 2;
	return src;
}

static U08* decode_1bytes(U08* src, U08* dst)
{
	*dst = src[0];
	src += 1;
	return src;
}

void GetGnssTypeSignalTypeString(U08 typeSig, CString& typeTxt)
{
  switch(typeSig) 
  {
  case 0x00:
    typeTxt = "GPS-L1 C/A";
    break;
  case 0x10:
    typeTxt = "GPS-L1C";
    break;
  case 0x20:
    typeTxt = "GPS-L2C";
    break;
  case 0x40:
    typeTxt = "GPS-L5";
    break;
  case 0x01:
    typeTxt = "SBAS-L1";
    break;
  case 0x02:
    typeTxt = "GLONASS-L1";
    break;
  case 0x22:
    typeTxt = "GLONASS-L2";
    break;
  case 0x42:
    typeTxt = "GLONASS-L3";
    break;
  case 0x03:
    typeTxt = "Galileo-E1";
    break;
  case 0x43:
    typeTxt = "Galileo-E5a";
    break;
  case 0x53:
    typeTxt = "Galileo-E5b";
    break;
  case 0x63:
    typeTxt = "Galileo-E6";
    break;
  case 0x04:
    typeTxt = "QZSS-L1 C/A";
    break;
  case 0x14:
    typeTxt = "QZSS-L1C";
    break;
  case 0x24:
    typeTxt = "QZSS-L2C";
    break;
  case 0x44:
    typeTxt = "QZSS-L5";
    break;
  case 0x64:
    typeTxt = "QZSS-LEX";
    break;
  case 0x05:
    typeTxt = "BeiDou-B1I";
    break;
  case 0x15:
    typeTxt = "BeiDou-B1C";
    break;
  case 0x45:
    typeTxt = "BeiDou-B2A";
    break;
  case 0x55:
    typeTxt = "BeiDou-B2I";
    break;
  case 0x75:
    typeTxt = "BeiDou-B3I";
    break;
  case 0x46:
    typeTxt = "NavIC-L5";
    break;
  default:
    typeTxt.Format("Unknown(%02X)", typeSig);
    break;
  }
}

void GetGnssTypeAndSignalId(U08 typeSig, NMEA::GNSS_System& gnss, U08& sigId)
{
  gnss = NMEA::GsUnknown;
  switch(typeSig) 
  {
  case 0x00:
    sigId = 1;  //"GPS-L1 C/A";
    gnss = NMEA::Gps;
    break;
  case 0x10:
    sigId = 2;  //"GPS-L1C";
    gnss = NMEA::Gps;
    break;
  case 0x20:
    sigId = 5;  //"GPS-L2C";
    gnss = NMEA::Gps;
    break;
  case 0x40:
    sigId = 7;  //"GPS-L5";
    gnss = NMEA::Gps;
    break;
  case 0x01:
    sigId = 1;  //"SBAS-L1";
    gnss = NMEA::Gps;
    break;
  case 0x02:
    sigId = 1;  //"GLONASS-L1";
    gnss = NMEA::Glonass;
    break;
  case 0x22:
    sigId = 3;  //"GLONASS-L2";
    gnss = NMEA::Glonass;
    break;
  case 0x42:
    sigId = 4;  //"GLONASS-L3";
    gnss = NMEA::Glonass;
    break;
  case 0x03:
    sigId = 7;  //"Galileo-E1";
    gnss = NMEA::Galileo;
    break;
  case 0x43:
    sigId = 1;  //"Galileo-E5a";
    gnss = NMEA::Galileo;
    break;
  case 0x53:
    sigId = 2;  //"Galileo-E5b";
    gnss = NMEA::Galileo;
    break;
  case 0x63:
    sigId = 4;  //"Galileo-E6";
    gnss = NMEA::Galileo;
    break;
  case 0x04:
    sigId = 1;  //"QZSS-L1 C/A";
    gnss = NMEA::Gps;
    break;
  case 0x14:
    sigId = 2;  //"QZSS-L1C";
    gnss = NMEA::Gps;
    break;
  case 0x24:
    sigId = 5;  //"QZSS-L2C";
    gnss = NMEA::Gps;
    break;
  case 0x44:
    sigId = 7;  //"QZSS-L5";
    gnss = NMEA::Gps;
    break;
  case 0x64:
    sigId = 8;  //"QZSS-LEX";
    gnss = NMEA::Gps;
    break;
  case 0x05:
    sigId = 1;  //"BeiDou-B1I";
    gnss = NMEA::Beidou;
    break;
  case 0x15:
    sigId = 3;  //"BeiDou-B1C";
    gnss = NMEA::Beidou;
    break;
  case 0x45:
    sigId = 5;  //"BeiDou-B2A";
    gnss = NMEA::Beidou;
    break;
  case 0x55:
    sigId = 11;  //"BeiDou-B2I";
    gnss = NMEA::Beidou;
    break;
  case 0x75:
    sigId = 8;  //"BeiDou-B3I";
    gnss = NMEA::Beidou;
    break;
  case 0x46:
    sigId = 1;  //"NavIC-L5";
    gnss = NMEA::Navic;
    break;
  default:
    sigId = 0;
    break;
  }
}

void ShowMeasurementChannel(U08* src, bool convertOnly, CString* pStr)
{
	U08* ptr = &src[5];
  U08 iod = 0;
  U08 nmeas = 0;

	ptr = decode_1bytes(ptr, &iod);
	ptr = decode_1bytes(ptr, &nmeas);

  //SetListCtrlRedraw(FALSE);
  strBuffer.Format("$RAW_MEAS(0xDD),IOD=%d,NMEAS=%d", iod, nmeas);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nmeas == 0)
  {
    //SetListCtrlRedraw(TRUE);
    return;
  }

#ifdef _LOCKHEED_MARTIN_
  strBuffer.Format(" SVID|CN0|  Pseudo Range|DeltaRange|DopFreq|Indicator|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" ----+---+--------------+----------+-------+---------+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
#else
  strBuffer.Format(" SVID|CN0|   PseudoRange|    CarrierCycle|DopFreq|Indicator|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" ----+---+--------------+----------------+-------+---------+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
#endif


	CHANNEL_DATA channel = { 0 };
	U32 tmp[2];
#ifdef _LOCKHEED_MARTIN_
	if(!convertOnly)
	{
		memset(CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID));
	}
	int gps_c = 0, glonass_c = 0, beidou_c = 0, galileo_c = 0, navic_c = 0;
#endif
	for (int i=0; i<nmeas; ++i)
	{
		ptr = decode_1bytes(ptr, &channel.prn);
		ptr = decode_1bytes(ptr, &channel.cn0);
#ifdef _LOCKHEED_MARTIN_
    S32 pseudoRange, deltaRange;
		ptr = decode_4bytes(ptr, (U32*)&pseudoRange);
    //channel.pseudo_range = (D64)pseudoRange / 10;
    channel.pseudo_range = (D64)pseudoRange;
    ptr = decode_4bytes(ptr, (U32*)&deltaRange);
		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.doppler_freq, tmp, sizeof(F32));
    ptr = decode_1bytes(ptr, &channel.indicator);
    strBuffer.Format("  %3d|%3d|%14.3lf|%10d| %6.0f|     0x%02X|",
      channel.prn, channel.cn0, channel.pseudo_range, deltaRange, 
			channel.doppler_freq, channel.indicator);
#else
		ptr = decode_4bytes(ptr, &tmp[1]);
		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.pseudo_range, tmp, sizeof(D64));
		ptr = decode_4bytes(ptr, &tmp[1]);
		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.carrier_cycle,tmp, sizeof(D64));
		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.doppler_freq, tmp, sizeof(F32));
		ptr = decode_1bytes(ptr, &channel.indicator);
    strBuffer.Format("  %3d|%3d| %13.3lf| % 15.3lf| %6.0f|     0x%02X|",
      channel.prn, channel.cn0, channel.pseudo_range, channel.carrier_cycle, 
			channel.doppler_freq, channel.indicator);

#endif
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
	  if(convertOnly)
	  {
		  continue;
	  }
#ifdef _LOCKHEED_MARTIN_
		if(NMEA::Glonass == NMEA::GetGNSSSystem(channel.prn))
		{
			if(glonass_c == 0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gl.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_gl.AddSnrSigId(0);
			}
      SnrTable snr(channel.cn0);
      CGPSDlg::gpsDlg->nmea.satellites_gl.SetSate(channel.prn, 0, 0, snr);
			glonass_c++;
		}
		else if(NMEA::Gps == NMEA::GetGNSSSystem(channel.prn))
		{
			if(gps_c == 0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gp.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_gp.AddSnrSigId(0);
			}
      SnrTable snr(channel.cn0);
      CGPSDlg::gpsDlg->nmea.satellites_gp.SetSate(channel.prn, 0, 0, snr);
			gps_c++;
		}
		else if(NMEA::Beidou == NMEA::GetGNSSSystem(channel.prn))
		{
			if(beidou_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_bd.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_bd.AddSnrSigId(0);
			}
      SnrTable snr(channel.cn0);
      CGPSDlg::gpsDlg->nmea.satellites_bd.SetSate(channel.prn, 0, 0, snr);
			beidou_c++;
		}
		else if(NMEA::Galileo == NMEA::GetGNSSSystem(channel.prn))
		{
			if(galileo_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_ga.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_ga.AddSnrSigId(0);
			}
      SnrTable snr(channel.cn0);
      CGPSDlg::gpsDlg->nmea.satellites_ga.SetSate(channel.prn, 0, 0, snr);
			galileo_c++;
		}
		else if(NMEA::Navic == NMEA::GetGNSSSystem(channel.prn))
		{
			if(navic_c == 0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gi.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_gi.AddSnrSigId(0);
			}
      SnrTable snr(channel.cn0);
      CGPSDlg::gpsDlg->nmea.satellites_gi.SetSate(channel.prn - 240, 0, 0, snr);
			navic_c++;
		}
#endif
	}
#ifdef _LOCKHEED_MARTIN_
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = gps_c;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = (gps_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = (gps_c + 3) / 4;
	//CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = fixed_gps_c + fixed_glonass_c;

	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfSate = glonass_c;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfMessage = (glonass_c + 3) / 4;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.SequenceNum = (glonass_c + 3) / 4;

	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfSate = beidou_c;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfMessage = (beidou_c + 3) / 4;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.SequenceNum = (beidou_c + 3) / 4;

	CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfSate = navic_c;
	CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfMessage = (navic_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gigsvMsgCopy.SequenceNum = (navic_c + 3) / 4;
#endif
  //SetListCtrlRedraw(TRUE);
	return;
}

void ExtRawMeas(U08* src, bool convertOnly, CString* pStr)
{
	U08* ptr = &src[5];
  U08 ver = 0;
  U08 iod = 0;
  U16 weeks = 0;
  U32 tow = 0;
  U16 measPeriod = 0;
  U08 measIndFlag = 0;
  U08 reserve = 0;
  U08 nmeas = 0;

	ptr = decode_1bytes(ptr, &ver);
	ptr = decode_1bytes(ptr, &iod);
	ptr = decode_2bytes(ptr, &weeks);
	ptr = decode_4bytes(ptr, &tow);
	ptr = decode_2bytes(ptr, &measPeriod);
	ptr = decode_1bytes(ptr, &measIndFlag);
	ptr = decode_1bytes(ptr, &reserve);
	ptr = decode_1bytes(ptr, &nmeas);

  strBuffer.Format("$EXT_RAW_MEAS(0xE5),V%d,IOD=%d,WN=%d,TOW=%d,MeasPeriod=%d,MeasIndicator=0x%02X,NMEAS=%d",
    ver, iod, weeks, tow, measPeriod, measIndFlag, nmeas);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  bool hasGp = false;
  bool hasGl = false;
  bool hasGa = false;
  bool hasBd = false;
  bool hasGi = false;

  if(nmeas != 0)
  {
    ExtMeasChannelData channel = { 0 };
    U32 tmp[2] = { 0 };

    strBuffer.Format("Gnss Signal|SVID|FrqID|LTInd|CN0|   PseudoRange|       AccCrrCyc|DopFreq|PRSdDv|ACCSdDv|DFSdDv| ChInd|");
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

    strBuffer.Format("-----------+----+-----+-----+---+--------------+----------------+-------+------+-------+------+------+");
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
    CGPSDlg::gpsDlg->m_psti033B.numCycleSlippedTotal = 0;

	  for (int i = 0; i < nmeas; ++i)
	  {
		  ptr = decode_1bytes(ptr, &channel.typeNsingel);
		  ptr = decode_1bytes(ptr, &channel.svid);
		  ptr = decode_1bytes(ptr, &channel.freqIdNlockTimeInd);
		  ptr = decode_1bytes(ptr, &channel.cn0);
		  ptr = decode_4bytes(ptr, &tmp[1]);
		  ptr = decode_4bytes(ptr, &tmp[0]);
		  memcpy(&channel.pseduRange, tmp, sizeof(D64));
		  ptr = decode_4bytes(ptr, &tmp[1]);
		  ptr = decode_4bytes(ptr, &tmp[0]);
		  memcpy(&channel.accCarrierCycle, tmp, sizeof(D64));
		  ptr = decode_4bytes(ptr, &tmp[0]);
		  memcpy(&channel.dopplerFreq, tmp, sizeof(F32));
		  ptr = decode_1bytes(ptr, &channel.prStdDeviation);
		  ptr = decode_1bytes(ptr, &channel.accStdDeviation);
		  ptr = decode_1bytes(ptr, &channel.dfDeviation);
	    ptr = decode_2bytes(ptr, &channel.chInd);
	    ptr = decode_1bytes(ptr, &reserve);
	    ptr = decode_1bytes(ptr, &reserve);

      CString typeTxt;
      GetGnssTypeSignalTypeString(channel.typeNsingel, typeTxt);
    //strBuffer.Format("Gnss Signal|SVID|FrqID|LTInd|CN0|   PseudoRange|       AccCrrCyc|DopFreq|PRSdDv|ACCSdDv|DFSdDv| ChInd|");
    //strBuffer.Format("-----------+----+-----+-----+---+--------------+----------------+-------+------+-------+------+------+");
      strBuffer.Format("%11s|  %2d|%5d|%5d|%3d| %13.3lf| % 15.3lf| % 6.0f|%6d|%7d|%6d|0x%04X|",
          typeTxt, channel.svid, channel.freqIdNlockTimeInd & 0x0f, channel.freqIdNlockTimeInd >> 4, 
          channel.cn0, channel.pseduRange, channel.accCarrierCycle, channel.dopplerFreq, 
          channel.prStdDeviation, channel.accStdDeviation, channel.dfDeviation, channel.chInd);
      DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

      if(channel.chInd & 0x08)
      {
        ++CGPSDlg::gpsDlg->m_psti033B.numCycleSlippedTotal;
      }

	    if(convertOnly)
	    {
		    continue;
	    }
      //For multi-Hz raw data output display issue, support 0xE5 will display error in multi-hz
return;
      U08 sigId;
      NMEA::GNSS_System gs = NMEA::GsUnknown;
      GetGnssTypeAndSignalId(channel.typeNsingel, gs, sigId);
/* 
      sigId = channel.typeNsingel >> 4;
      switch(channel.typeNsingel & 0x0F)
      {
      case 0:
       gs = NMEA::Gps;
       if(sigId == 0)
         sigId = 1;
       else if(sigId == 1)
         sigId = 3;
       else if(sigId == 2)
         sigId = 4;
       else 
         sigId = 8;
       break;
      case 1:
       gs = NMEA::Gps;
       sigId = 1;
       break;
      case 2:
       gs = NMEA::Glonass;
       //channel.svid += 64;
       if(sigId == 0)
         sigId = 1;
       else if(sigId == 2)
         sigId = 3;
       else if(sigId == 4)
         sigId = 4; //?
       break;
      case 3:
       gs = NMEA::Galileo;
       if(sigId == 0)
         sigId = 7;
       else if(sigId == 4)
         sigId = 1;
       else if(sigId == 5)
         sigId = 2;
       else 
         sigId = 5;
       break;
      case 4: //QZSS
       gs = NMEA::Gps;
       if(sigId == 0)
         sigId = 1;
       else if(sigId == 1)
         sigId = 3;
       else if(sigId == 2)
         sigId = 4;
       else  if(sigId == 4)
         sigId = 8;
       else
         sigId = 7; //?
       break;
      case 5:
       gs = NMEA::Beidou;
       //channel.svid += 200;
       if(sigId == 0)
         sigId = 1;
       else if(sigId == 1)
         sigId = 5;
       else if(sigId == 4)
         sigId = 2;
       else  if(sigId == 5)
         sigId = 3;
       else
         sigId = 4; //?
       break;
      case 6:
       gs = NMEA::Navic;
       sigId = 4;
       break;
      default:
       gs = NMEA::Gps;
       break;
      }
*/
      if(gs == NMEA::Gps)
      {
        if(hasGp == false)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_gp.ClearSnr();
          CGPSDlg::gpsDlg->nmea.satellites2_gp.ClearChannelInd();
          hasGp = true;
        }
        SnrTable snr(sigId, channel.cn0, channel.chInd);
        if(channel.chInd & 0x08)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_gp.AddChannelInd(channel.svid);
        }
        CGPSDlg::gpsDlg->nmea.satellites2_gp.SetSate2(channel.svid, NonUseValue, NonUseValue, snr);
        CGPSDlg::gpsDlg->nmea.satellites2_gp.AddSnrSigId(sigId);

      }
      if(gs == NMEA::Beidou)
      {
        if(hasBd == false)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_bd.ClearSnr();
          CGPSDlg::gpsDlg->nmea.satellites2_bd.ClearChannelInd();
          hasBd = true;
        }
        SnrTable snr(sigId, channel.cn0, channel.chInd);
        if(channel.chInd & 0x08)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_bd.AddChannelInd(channel.svid);
        }
        CGPSDlg::gpsDlg->nmea.satellites2_bd.SetSate2(channel.svid, NonUseValue, NonUseValue, snr);
        CGPSDlg::gpsDlg->nmea.satellites2_bd.AddSnrSigId(sigId);
      }
      if(gs == NMEA::Glonass)
      {
        if(hasGl == false)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_gl.ClearSnr();
          CGPSDlg::gpsDlg->nmea.satellites2_gl.ClearChannelInd();
          hasGl = true;
        }
        SnrTable snr(sigId, channel.cn0, channel.chInd);
        if(channel.chInd & 0x08)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_gl.AddChannelInd(channel.svid);
        }
        CGPSDlg::gpsDlg->nmea.satellites2_gl.SetSate2(channel.svid + 64, NonUseValue, NonUseValue, snr);
        CGPSDlg::gpsDlg->nmea.satellites2_gl.AddSnrSigId(sigId);
      }
      if(gs == NMEA::Galileo)
      {
        if(hasGa == false)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_ga.ClearSnr();
          CGPSDlg::gpsDlg->nmea.satellites2_ga.ClearChannelInd();
          hasGa = true;
        }
        SnrTable snr(sigId, channel.cn0, channel.chInd);
        if(channel.chInd & 0x08)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_ga.AddChannelInd(channel.svid);
        }
        CGPSDlg::gpsDlg->nmea.satellites2_ga.SetSate2(channel.svid, NonUseValue, NonUseValue,  snr);
        CGPSDlg::gpsDlg->nmea.satellites2_ga.AddSnrSigId(sigId);
      }
      if(gs == NMEA::Navic)
      {
        if(hasGi == false)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_gi.ClearSnr();
          CGPSDlg::gpsDlg->nmea.satellites2_gi.ClearChannelInd();
          hasGi = true;
        }
        SnrTable snr(sigId, channel.cn0, channel.chInd);
        if(channel.chInd & 0x08)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_gi.AddChannelInd(channel.svid);
        }
        CGPSDlg::gpsDlg->nmea.satellites2_gi.SetSate2(channel.svid, NonUseValue, NonUseValue,  snr);
        CGPSDlg::gpsDlg->nmea.satellites2_gi.AddSnrSigId(sigId);
      }
	  } //for (int i = 0; i < nmeas; ++i)
  }

	if(convertOnly)
	{
		return;
	}

	UtcTime utc;
	UtcConvertGpsToUtcTime(weeks, (tow / 1000.0), &utc);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;

  if(hasGp)
  {
    CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = 3;
  }
  if(hasBd)
  {
    CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_bdgsvMsgCopy.SequenceNum = 3;
  }
  if(hasGa)
  {
    CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_gagsvMsgCopy.SequenceNum = 3;
  }
  if(hasGl)
  {
    CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_glgsvMsgCopy.SequenceNum = 3;
  }
  if(hasGi)
  {
    CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_gigsvMsgCopy.SequenceNum = 3;
  }
  //for (int i = 0; i < nmeas; ++i)
  //{
	 // ptr = decode_1bytes(ptr, &channel.typeNsingel);
	 // ptr = decode_1bytes(ptr, &channel.svid);
	 // ptr = decode_1bytes(ptr, &channel.freqIdNlockTimeInd);
	 // ptr = decode_1bytes(ptr, &channel.cn0);
	 // ptr = decode_4bytes(ptr, &tmp[1]);
  //}
	return;
}

void ExtRawMeasEf(U08* src, bool convertOnly, CString* pStr)
{
	src += 3;	//pass id and sid
  U08 fixMode = *(src + 2);
  U08 nSv = *(src + 2);
	U16 wn = ConvertLeonU16(src + 4);
	U32 tow = ConvertLeonU32(src + 6);

	S32 ecefx = ConvertLeonS32(src + 10);
	S32 ecefy = ConvertLeonS32(src + 14);
	S32 ecefz = ConvertLeonS32(src + 18);

	S32 vx = ConvertLeonS32(src + 22);
	S32 vy = ConvertLeonS32(src + 26);
	S32 vz = ConvertLeonS32(src + 30);

  S64 clockBias = ConvertLeonS64(src + 34);
  S32 clockDrift = ConvertLeonS32(src + 42);

  U16 gDop = ConvertLeonU16(src + 46);
  S08 raimStatus = (S08)*(src + 48);

	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, (D64)tow / 1000, &utc);

  POS_T pos;
	pos.px = (D64)ecefx / 100;
	pos.py = (D64)ecefy / 100;
	pos.pz = (D64)ecefz / 100;

	LLA_T lla;
	CooCartesianToGeodetic(&pos, &lla);

	lla.lat *= R2D;
	lla.lon *= R2D;
	double lat_d = (S16)fabs(lla.lat );
	double lon_d = (S16)fabs(lla.lon );
	double lat_m = fmod( fabs(lla.lat), 1.0) * 60.0;
	double lon_m = fmod( fabs(lla.lon), 1.0) * 60.0;

	D64 fv = sqrt((D64)vx * vx + (D64)vy * vy + (D64)vz * vz) / 100;

  strBuffer.Format("$Navigation data and status(0xEF),FixMode=%d,nSvInFix=%d,WN=%d,TOW=%d,ecefX=%d,ecefY=%d,,ecefZ=%d,ecefVX=%d,ecefVY=%d,ecefVZ=%d,clockBias=%d,clockDrife=%d,gDop=%d,raimStatus=%d",
    fixMode, nSv, wn, tow, ecefx, ecefy, ecefz, vx, vy, vz, (S32)clockBias, clockDrift, gDop, raimStatus);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	if(convertOnly)
	{
		return;
	}

	if(fixMode == 0)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '0';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 1;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'V';
	}
	else if(fixMode == 1)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 2;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(fixMode == 2)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(fixMode == 3)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '2';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}

	U16 i_sec;
	i_sec = (U16)(utc.sec * 1000);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = i_sec * 0.001f;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = i_sec * 0.001f;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = lat_d * 100.0 + lat_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = (lla.lat >= 0) ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = lon_d * 100.0 + lon_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = (lla.lon >= 0) ? 'E' : 'W';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = lla.alt - CGPSDlg::gpsDlg->m_gpggaMsgCopy.GeoidalSeparation;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.SpeedKnots = (F32)(fv * MS2KMHR/ KNOTS2KMHR);
}

void ShowSubframe(U08 *src, bool convertOnly, CString* pStr)
{
	U08 msgType = src[4];
	CString subFrameType;
	if(msgType==0xE0)
	{
		subFrameType = "GPS_SUBFRAME(0xE0)";
	}
	else if(msgType==0xE1)
	{
		subFrameType = "GLONASS_STRING(0xE1)";
	}	
	else if(msgType==0xE2)
	{
		subFrameType = "BEIDOU2_D1_SUBFRAME(0xE2)";
	}	
	else if(msgType==0xE3)
	{
		subFrameType = "BEIDOU2_D2_SUBFRAME(0xE3)";
	}	
#if(IS_DEBUG)
	else if(msgType==0xE4)
	{
		subFrameType = "SBAS_SUBFRAME(0xE3)";
	}
#endif
	U08 prn = src[5];
	U08 subFrmId = src[6];
  strBuffer.Format("$%s,SVID=%d,SFID=%d,data=", subFrameType, prn, subFrmId);

	WORD packetLen = MAKEWORD(src[3], src[2]);
	for(int i = 3; i < packetLen; ++i)
	{
		CString tmpBuff;
    tmpBuff.Format("%02X ", src[4+i]);
		strBuffer += tmpBuff;
	}
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
}

void ShowGeneralSubframe(U08 *src, bool convertOnly, CString* pStr)
{
	U08 msgType = src[4];
	CString subFrameType;

	subFrameType = "GENERAL_SUBFRAME(0xE6)";

  CString typeTxt;
	U08 version = src[5];
	U08 typeSig = src[6];
  U08 svid = src[7];
  U08 len = src[8];
  GetGnssTypeSignalTypeString(typeSig, typeTxt);

  strBuffer.Format("$%s,VER=%d,%s,SVID=%d,Len=%d", subFrameType, version, typeTxt, svid, len);

	//WORD packetLen = MAKEWORD(src[3], src[2]);
	//for(int i = 3; i < packetLen; ++i)
	//{
	//	CString tmpBuff;
 //   tmpBuff.Format("%02X ", src[4+i]);
	//	strBuffer += tmpBuff;
	//}
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
}

void ShowBinaryTimeStamp(U08 *src, bool convertOnly, CString* pStr)
{
	U08 msgType = src[4];
	CString subFrameType;

	subFrameType = "TIME_STAMP(0xE9)";

	U08 version = src[5];
	U16 wn = ConvertLeonU16(src + 6);
	D64 tow = ConvertLeonDouble(src + 8);

  strBuffer.Format("$%s,VER=%d,WN=%d,TOW=%f", subFrameType, version, wn, tow);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
}

void ShowMeasurementTime(U08 *src, bool convertOnly, CString* pStr)
{
  static CString strBuffer("", 512);
	U08 *ptr = &src[6];

	U16 wn = 0;
	U32 tow = 0;
	U16 period = 0;

	ptr = decode_2bytes(ptr, &wn);
	ptr = decode_4bytes(ptr, &tow);
	ptr = decode_2bytes(ptr, &period);

  strBuffer.Format("$MEAS_TIME(0xDC),WN=%d,TOW=%d,Period=%d",
    wn, tow, period);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

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
  static CString strBuffer("", 512);
	U08* ptr = &src[5];

  U08 iod = 0;
  U08 nsvs = 0;

	ptr = decode_1bytes(ptr, &iod);
	ptr = decode_1bytes(ptr, &nsvs);

  //SetListCtrlRedraw(FALSE);
  strBuffer.Format("$SV_CH_STATUS(0xDE),IOD=%d,NSVS=%d", iod, nsvs);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nsvs == 0)
  {
    //SetListCtrlRedraw(TRUE);
    return;
  }

  strBuffer.Format(" ChId|SVID|SvInd| URA|CN0|ELE| AZI|ChInd|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  strBuffer.Format(" ----+----+-----+----+---+---+----+-----+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	int fixed_gps_c = 0, fixed_glonass_c = 0, fixed_beidou_c = 0, fixed_galileo_c = 0, fixed_navic_c = 0;
	int gps_c = 0, glonass_c = 0, beidou_c = 0, galileo_c = 0, navic_c = 0;
	SV_CH_DATA sv;
	if(!convertOnly)
	{
		memset(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID));
	}

	for (int i=0; i<nsvs; ++i)
	{
		ptr = decode_1bytes(ptr, &sv.channel_id);
		ptr = decode_1bytes(ptr, &sv.prn);
		ptr = decode_1bytes(ptr, &sv.SV_status);
		ptr = decode_1bytes(ptr, &sv.URA);
		ptr = decode_1bytes(ptr, (U08*)&sv.cn0);
		ptr = decode_2bytes(ptr, (U16*)&sv.elevation);
		ptr = decode_2bytes(ptr, (U16*)&sv.azimuth);
		ptr = decode_1bytes(ptr, &sv.channel_status);

    strBuffer.Format("%5d|%4d| 0x%02X|%4d|%3d|%3d|%4d| 0x%02X|",
			sv.channel_id, sv.prn, sv.SV_status, sv.URA,sv.cn0, 
			sv.elevation, sv.azimuth, sv.channel_status);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
    //SetListCtrlRedraw(TRUE);
    
    if(sv.elevation < 0 || sv.azimuth < 0)
    {
      strBuffer.Format("****ERROR****");
      DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
    }
	  if(convertOnly)
	  {
		  continue;
	  }

		if(NMEA::Glonass == NMEA::GetGNSSSystem(sv.prn))
		{
			if(glonass_c == 0)
			{
        //CGPSDlg::gpsDlg->nmea.satellites2_gl.Clear();
        if(CGPSDlg::gpsDlg->nmea.satellites2_gl.GetSnrIdSize() == 0)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_gl.AddSnrSigId(0);
        }			
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID[fixed_glonass_c] = sv.prn;
				++fixed_glonass_c;
			}

      SnrTable snr(sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_gl.SetSate(sv.prn, sv.elevation, sv.azimuth, snr);
			glonass_c++;
		}
		else if(NMEA::Gps == NMEA::GetGNSSSystem(sv.prn))
		{
			if(gps_c == 0)
			{
        //CGPSDlg::gpsDlg->nmea.satellites2_gp.Clear();
        if(CGPSDlg::gpsDlg->nmea.satellites2_gp.GetSnrIdSize() == 0)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_gp.AddSnrSigId(0);
        }
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID[fixed_gps_c] = sv.prn;
				fixed_gps_c++;
			}

      SnrTable snr(sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_gp.SetSate(sv.prn, sv.elevation, sv.azimuth, snr);
			gps_c++;
		}
		else if(NMEA::Beidou == NMEA::GetGNSSSystem(sv.prn))
		{
			if(beidou_c==0)
			{
        //CGPSDlg::gpsDlg->nmea.satellites2_bd.Clear();
        if(CGPSDlg::gpsDlg->nmea.satellites2_bd.GetSnrIdSize() == 0)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_bd.AddSnrSigId(0);
        }
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID[fixed_beidou_c] = sv.prn - 200;
				fixed_beidou_c++;
			}

      SnrTable snr(sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_bd.SetSate(sv.prn - 200, sv.elevation, sv.azimuth, snr);
			beidou_c++;
		}
		else if(NMEA::Galileo == NMEA::GetGNSSSystem(sv.prn))
		{
			if(galileo_c==0)
			{
        //CGPSDlg::gpsDlg->nmea.satellites2_ga.Clear();
        if(CGPSDlg::gpsDlg->nmea.satellites2_ga.GetSnrIdSize() == 0)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_ga.AddSnrSigId(0);
        }
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID[fixed_galileo_c] = sv.prn;
				fixed_galileo_c++;
			}

      SnrTable snr(sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_ga.SetSate(sv.prn, sv.elevation, sv.azimuth, snr);
			galileo_c++;
		}
		else if(NMEA::Navic == NMEA::GetGNSSSystem(sv.prn))
		{
			if(navic_c==0)
			{
        //CGPSDlg::gpsDlg->nmea.satellites2_gi.Clear();
        if(CGPSDlg::gpsDlg->nmea.satellites2_gi.GetSnrIdSize() == 0)
        {
          CGPSDlg::gpsDlg->nmea.satellites2_gi.AddSnrSigId(0);
        }			
      }
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID[fixed_navic_c] = sv.prn - 240;
				fixed_navic_c++;
			}

      SnrTable snr(sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_gi.SetSate(sv.prn - 240, sv.elevation, sv.azimuth, snr);
			navic_c++;
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

	CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfSate = galileo_c;
	CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfMessage = (galileo_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gagsvMsgCopy.SequenceNum = (galileo_c + 3) / 4;

	CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfSate = navic_c;
	CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfMessage = (navic_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gigsvMsgCopy.SequenceNum = (navic_c + 3) / 4;
}

void ShowMeasurementSvEleAzi(U08 *src, bool convertOnly, CString* pStr)
{
  static CString strBuffer("", 512);
	U08* ptr = &src[5];

  U08 ver = 0;
  U08 iod = 0;
  U08 nsvs = 0;

	ptr = decode_1bytes(ptr, &ver);
	ptr = decode_1bytes(ptr, &iod);
	ptr = decode_1bytes(ptr, &nsvs);

  //SetListCtrlRedraw(FALSE);
  strBuffer.Format("$GNSS_SV_ELE_AZI(0xE8),VER=%d,IOD=%d,NSVS=%d", ver, iod, nsvs);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nsvs == 0)
  {
    //SetListCtrlRedraw(TRUE);
    return;
  }

  strBuffer.Format(" GNSS|SVID| Ele| Azi|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  strBuffer.Format(" ----+----+----+----+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	int fixed_gps_c = 0, fixed_glonass_c = 0, fixed_beidou_c = 0, fixed_galileo_c = 0, fixed_navic_c = 0;
	int gps_c = 0, glonass_c = 0, beidou_c = 0, galileo_c = 0, navic_c = 0;

  U08 type, svid;
  U16 ele, azi;
	for (int i = 0; i < nsvs; ++i)
	{
		ptr = decode_1bytes(ptr, &type);
		ptr = decode_1bytes(ptr, &svid);
		ptr = decode_2bytes(ptr, &ele);
		ptr = decode_2bytes(ptr, &azi);
  //strBuffer.Format(" GNSS|SVID| Ele| Azi|");
  //strBuffer.Format(" ----+----+----+----+");
    strBuffer.Format(" %4d|%4d|%4d|%4d|",
			type, svid, ele, azi);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
    //SetListCtrlRedraw(TRUE);
    
    if(ele < 0 || azi < 0)
    {
      strBuffer.Format("****ERROR****");
      DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
    }
	  if(convertOnly)
	  {
		  continue;
	  }

    NMEA::GNSS_System gs = NMEA::GsUnknown;
    U08 sigId;
    GetGnssTypeAndSignalId(type, gs, sigId);
    if(NMEA::Gps == gs)
		{
      CGPSDlg::gpsDlg->nmea.satellites2_gp.SetSate3(svid, ele, azi);
			gps_c++;
		}
		else if(NMEA::Glonass == gs)
		{
      CGPSDlg::gpsDlg->nmea.satellites2_gl.SetSate3(svid + 64, ele, azi);
			glonass_c++;
		}
		else if(NMEA::Beidou == gs)
		{
      CGPSDlg::gpsDlg->nmea.satellites2_bd.SetSate3(svid, ele, azi);
			beidou_c++;
		}
		else if(NMEA::Galileo == gs)
		{
      CGPSDlg::gpsDlg->nmea.satellites2_ga.SetSate3(svid, ele, azi);
			galileo_c++;
		}
		else if(NMEA::Navic == gs)
		{
      CGPSDlg::gpsDlg->nmea.satellites2_gi.SetSate3(svid, ele, azi);
			navic_c++;
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

	CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfSate = galileo_c;
	CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfMessage = (galileo_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gagsvMsgCopy.SequenceNum = (galileo_c + 3) / 4;

	CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfSate = navic_c;
	CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfMessage = (navic_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gigsvMsgCopy.SequenceNum = (navic_c + 3) / 4;
}

void ShowMeasurementGnssSv(U08 *src, bool convertOnly, CString* pStr)
{
  static CString strBuffer("", 512);
	U08* ptr = &src[5];

  U08 ver = 0;
  U08 iod = 0;
  U08 nsvs = 0;

	ptr = decode_1bytes(ptr, &ver);
	ptr = decode_1bytes(ptr, &iod);
	ptr = decode_1bytes(ptr, &nsvs);

  //SetListCtrlRedraw(FALSE);
  strBuffer.Format("$GNSS_SV_CH_STATUS(0xE7),VER=%d,IOD=%d,NSVS=%d", ver, iod, nsvs);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nsvs == 0)
  {
    //SetListCtrlRedraw(TRUE);
    return;
  }

  strBuffer.Format(" ChId|Gnss Signal|SVID|SvInd| URA|CN0|ChInd|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  strBuffer.Format(" ----+-----------|----+-----+----+---+-----+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	//int fixed_gps_c = 0, fixed_glonass_c = 0, fixed_beidou_c = 0, fixed_galileo_c = 0, fixed_navic_c = 0;
	//int gps_c = 0, glonass_c = 0, beidou_c = 0, galileo_c = 0, navic_c = 0;
	bool hasGp = false;
  bool hasGl = false;
  bool hasGa = false;
  bool hasBd = false;
  bool hasGi = false;
	if(!convertOnly)
	{
		memset(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID));
	}

  GnssSvChannelData sv;
	//U08 channelId;
	//U08 gnssTypeSignal;
 // U08 svid;
	//U08 svStatusInd;
	//U08 ura;
	//S08 cn0;
	//U08 chStatusInd;
	for (int i = 0; i < nsvs; ++i)
	{
		ptr = decode_1bytes(ptr, &sv.channelId);
		ptr = decode_1bytes(ptr, &sv.gnssTypeSignal);
		ptr = decode_1bytes(ptr, &sv.svid);
		ptr = decode_1bytes(ptr, &sv.svStatusInd);
		ptr = decode_1bytes(ptr, &sv.ura);
		ptr = decode_1bytes(ptr, (U08*)&sv.cn0);
		ptr = decode_1bytes(ptr, &sv.chStatusInd);

    CString typeTxt;
    GetGnssTypeSignalTypeString(sv.gnssTypeSignal, typeTxt);
 // strBuffer.Format(" ChId|Gnss Signal|SVID|SvInd| URA|CN0|ChInd|");
    strBuffer.Format("%5d|%11s|%4d| 0x%02X|%4d|%3d| 0x%02X|",
			sv.channelId, typeTxt, sv.svid, sv.svStatusInd, 
      sv.ura, sv.cn0, sv.chStatusInd);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
    //SetListCtrlRedraw(TRUE);
    
	  if(convertOnly)
	  {
		  continue;
	  }

    U08 sigId = 0;
    NMEA::GNSS_System gs = NMEA::GsUnknown;
	  GetGnssTypeAndSignalId(sv.gnssTypeSignal, gs, sigId);

    if(gs == NMEA::Gps)
    {
      if(hasGp == false)
      {
        CGPSDlg::gpsDlg->nmea.satellites2_gp.ClearSnr();
        hasGp = true;
      }
			if (sv.chStatusInd & 0x30)
			{
        int p = CGPSDlg::gpsDlg->m_gpgsaMsgCopy.FindId(sv.svid);
        CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID[p] = sv.svid;
			}
      SnrTable snr(sigId, sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_gp.SetSate2(sv.svid, NonUseValue, NonUseValue, snr);
      CGPSDlg::gpsDlg->nmea.satellites2_gp.AddSnrSigId(sigId);
    }
    if(gs == NMEA::Beidou)
    {
      if(hasBd == false)
      {
        CGPSDlg::gpsDlg->nmea.satellites2_bd.ClearSnr();
        hasBd = true;
      }
			if (sv.chStatusInd & 0x30)
			{
        CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID[CGPSDlg::gpsDlg->m_bdgsaMsgCopy.FindId(sv.svid)] = sv.svid;
			}
      SnrTable snr(sigId, sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_bd.SetSate2(sv.svid, NonUseValue, NonUseValue, snr);
      CGPSDlg::gpsDlg->nmea.satellites2_bd.AddSnrSigId(sigId);
    }
    if(gs == NMEA::Glonass)
    {
      if(hasGl == false)
      {
        CGPSDlg::gpsDlg->nmea.satellites2_gl.ClearSnr();
        hasGl = true;
      }
			if (sv.chStatusInd & 0x30)
			{
        CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID[CGPSDlg::gpsDlg->m_glgsaMsgCopy.FindId(sv.svid)] = sv.svid + 64;
			}
      SnrTable snr(sigId, sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_gl.SetSate2(sv.svid + 64, NonUseValue, NonUseValue, snr);
      CGPSDlg::gpsDlg->nmea.satellites2_gl.AddSnrSigId(sigId);
    }
    if(gs == NMEA::Galileo)
    {
      if(hasGa == false)
      {
        CGPSDlg::gpsDlg->nmea.satellites2_ga.ClearSnr();
        hasGa = true;
      }
			if (sv.chStatusInd & 0x30)
			{
        CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID[CGPSDlg::gpsDlg->m_gagsaMsgCopy.FindId(sv.svid)] = sv.svid;
			}
      SnrTable snr(sigId, sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_ga.SetSate2(sv.svid, NonUseValue, NonUseValue,  snr);
      CGPSDlg::gpsDlg->nmea.satellites2_ga.AddSnrSigId(sigId);
    }
    if(gs == NMEA::Navic)
    {
      if(hasGi == false)
      {
        CGPSDlg::gpsDlg->nmea.satellites2_gi.ClearSnr();
        hasGi = true;
      }
			if (sv.chStatusInd & 0x30)
			{
        CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID[CGPSDlg::gpsDlg->m_gigsaMsgCopy.FindId(sv.svid)] = sv.svid;
			}
      SnrTable snr(sigId, sv.cn0);
      CGPSDlg::gpsDlg->nmea.satellites2_gi.SetSate2(sv.svid, NonUseValue, NonUseValue,  snr);
      CGPSDlg::gpsDlg->nmea.satellites2_gi.AddSnrSigId(sigId);
    }
	}
	
if(hasGp)
  {
    CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = 3;
  }
  if(hasBd)
  {
    CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_bdgsvMsgCopy.SequenceNum = 3;
  }
  if(hasGa)
  {
    CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_gagsvMsgCopy.SequenceNum = 3;
  }
  if(hasGl)
  {
    CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_glgsvMsgCopy.SequenceNum = 3;
  }
  if(hasGi)
  {
    CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfMessage = 3;
    CGPSDlg::gpsDlg->m_gigsvMsgCopy.SequenceNum = 3;
  }
}

void ShowReceiverNav(U08 *src, bool convertOnly, CString* pStr)
{
  U08 *ptr = &src[6];
  U08 iod = src[5];

	U32 tmp[2];
	RECEIVER_NAV_DATA receiver;

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

  strBuffer.Format("$RCV_STATE(0xDF),IOD=%d,NavState=%d,WN=%d,TOW=%lf,ECEF_x=%lf,ECEF_y=%lf,ECEF_z=%lf," \
    "ECEF_vx=%f,ECEF_vy=%f,ECEF_vz=%f,ClockBias=%lf,ClockDrift=%f,GDOP=%f,PDOP=%f,HDOP=%f,VDOP=%f,TDOP=%f",
		iod, receiver.nav_status, receiver.wn, receiver.tow, 
		receiver.ecef_x, receiver.ecef_y, receiver.ecef_z,
		receiver.ecef_vx, receiver.ecef_vy, receiver.ecef_vz,
		receiver.clock_bias, receiver.clock_drift,
		receiver.gdop, receiver.pdop, receiver.hdop, 
		receiver.vdop, receiver.tdop);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  if(convertOnly)
  {
    return;
  }

  UtcTime utc;
  UtcConvertGpsToUtcTime(receiver.wn, receiver.tow, &utc);
	U16 i_sec;
	i_sec = (U16)(utc.sec * 1000);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = i_sec * 0.001f;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = i_sec * 0.001f;

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

//Alex Debug
 // pos.px = -2362161.5270503676;
	//pos.py = 5376223.183230296;
	//pos.pz = 2481200.8335405095;

	LLA_T lla;
	CooCartesianToGeodetic(&pos, &lla);
  //CooCartesianToGeodetic(pos.px, pos.py, pos.pz, lla.lat, lla.lon, lla.alt);

	lla.lat *= R2D;
	lla.lon *= R2D;
	double lat_d = (S16)fabs(lla.lat );
	double lon_d = (S16)fabs(lla.lon );
	double lat_m = fmod( fabs(lla.lat), 1.0) * 60.0;
	double lon_m = fmod( fabs(lla.lon), 1.0) * 60.0;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = lat_d * 100.0 + lat_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = (lla.lat >= 0) ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = lon_d * 100.0 + lon_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = (lla.lon >= 0) ? 'E' : 'W';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = lla.alt - CGPSDlg::gpsDlg->m_gpggaMsgCopy.GeoidalSeparation;

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = receiver.pdop;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.HDOP = receiver.hdop;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = receiver.vdop;
}

void ShowBinaryOutput(U08* src, bool convertOnly, CString* pStr)
{
  U16 size = src[2] << 8 | src[3];
  BOOL v2Format = (size == 0x3B) ? FALSE : TRUE;

  int ix = 5;
  U08 fixMode = src[ix];
  ++ix;

  U08 nos = src[ix];
  ++ix;


  U16 wn = src[ix] << 8 | src[ix + 1];
  ix += 2;

  U32 tow = 0;
  D64 ftow = 0;

  if(v2Format)
  {
    ftow = ConvertLeonDouble(&src[ix]);
    ix += 8;
  }
  else
  {
	  tow = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
    ix += 4;
    ftow = (double)tow / 100.0;
  }

	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, ftow, &utc);

	double lat = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	double lon = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	D64 flat = lat;
	flat /= 10000000;
	flat =((int)flat)*100 +(double)(flat - (int)flat)*60;

	D64 flon = lon;
	flon /= 10000000;
	flon = ((int)flon)*100 +(double)(flon - (int)flon)*60;

  U32 d = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;
	S32 alt_t = *((S32*)(&d));

  d = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;
	S32 alt = *((S32*)(&d));

	U32 gdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	U32 pdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	U32 hdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	U32 vdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	U32 tdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	S32 ecefx = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 ecefy = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 ecefz = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 vx = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 vy = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 vz = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	D64 fx = (D64)vx / 100;
	D64 fy = (D64)vy / 100;
	D64 fz = (D64)vz / 100;
	D64 fv = sqrt(fx * fx + fy * fy + fz * fz);

  //SetListCtrlRedraw(FALSE);
	strBuffer.Format("$fix mode=%d", fixMode);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$number if sv in fix=%d", nos);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS Week=%d", wn);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS TOW=%.2f",ftow);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$latitude=%.6f",flat);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$longitude=%.6f",flon);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ellipsoid altitude=%.2f", (F32)alt_t / 100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$sea level altitude=%.2f", (F32)alt / 100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$gdop=%.2f",(F32)gdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$pdop=%.2f",(F32)pdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$hdop=%.2f",(F32)hdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$tdop=%.2f",(F32)tdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-x=%.2f",(double)ecefx/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-y=%.2f",(double)ecefy/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-z=%.2f",(double)ecefz/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vx=%.2f",fx);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vy=%.2f",fy);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vz=%.2f",fz);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  //SetListCtrlRedraw(TRUE);
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

	//volatile U16 i_sec;
	//i_sec = (U16)(utc.sec * 1000);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	//CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = i_sec*0.001f;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	//CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = i_sec * 0.001f;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;

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

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.SpeedKnots = (F32)(fv * MS2KMHR / KNOTS2KMHR);
}

void ShowDjiBinaryOutput(U08* src, bool convertOnly, CString* pStr)
{
	src += 3;	//pass id and sid
	U16 wn = ConvertLeonU16(src + 6);
	D64 tow = ConvertLeonDouble(src + 8);

	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, tow, &utc);

	D64 lat = ConvertLeonDouble(src + 16);
	D64 lon = ConvertLeonDouble(src + 24);
	lat =((int)lat)*100 +(double)(lat - (int)lat)*60;
	lon = ((int)lon)*100 +(double)(lon - (int)lon)*60;

	F32 alt_t = ConvertLeonFloat(src + 32);
	F32 alt = ConvertLeonFloat(src + 36);

	U16 gdop = ConvertLeonU16(src + 40);
	U16 pdop = ConvertLeonU16(src + 42);
	U16 hdop = ConvertLeonU16(src + 44);
	U16 vdop = ConvertLeonU16(src + 46);
	U16 tdop = ConvertLeonU16(src + 48);

	D64 ecefx = ConvertLeonDouble(src + 50);
	D64 ecefy = ConvertLeonDouble(src + 58);
	D64 ecefz = ConvertLeonDouble(src + 66);

	D64 vx = ConvertLeonDouble(src + 74);
	D64 vy = ConvertLeonDouble(src + 78);
	D64 vz = ConvertLeonDouble(src + 82);

	F32 rtkAge = (F32)ConvertLeonU16(src + 86) / 10;
	F32 rtkFix = (F32)ConvertLeonU16(src + 88) / 10;
	
	CGPSDlg::gpsDlg->PostMessage(UWM_UPDATE_RTK_INFO, *(WPARAM*)&rtkAge, *(LPARAM*)&rtkFix);
	D64 fv = sqrt(vx * vx + vy * vy + vz * vz);

	strBuffer.Format("$fix mode=%d", src[4]);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$number if sv in fix=%d", src[5]);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS Week=%d", wn);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS TOW=%.2f", tow);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$latitude=%.7f", lat);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$longutide=%.7f",lon);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ellipsoid altitude=%.2f", alt_t);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$sea level altitude=%.2f", alt);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$gdop=%.2f",(F32)gdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$pdop=%.2f",(F32)pdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$hdop=%.2f",(F32)hdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$tdop=%.2f",(F32)tdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-x=%.2f",(double)ecefx);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-y=%.2f",(double)ecefy);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-z=%.2f",(double)ecefz);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vx=%.2f",vx);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vy=%.2f",vy);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vz=%.2f",vz);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$RTK age=%.1f", rtkAge);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$RTK ratio=%.1f", rtkFix);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	if(convertOnly)
	{
		return;
	}

	if(src[4]==0)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '0';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 1;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'V';
	}
	else if(src[4] ==1)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 2;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[4] == 2)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[4] == 3)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '2';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[4] == 4)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '5';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'F';
	}
	else if(src[4] == 5)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '4';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'R';
	}	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = src[5];

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

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = lat;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = lat >= 0 ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = lon;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = lon >= 0 ? 'E' : 'W';

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Latitude = lat;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Latitude_N_S = lat >= 0 ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Longitude = lon;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Longitude_E_W = lon >= 0 ? 'E' : 'W';

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = alt;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude /= 100.0F;

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = (F32)pdop/100;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.HDOP = (F32)hdop/100;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = (F32)vdop/100;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.SpeedKnots = (F32)(fv * MS2KMHR/ KNOTS2KMHR);
}

void ShowRtcm1005(U08* src, bool convertOnly, CString* pStr)
{
  U16 satId = 0;
  GetBitData(&src[3], 12, 12, (U08*)(&satId), sizeof(satId));
  U08 gpQtyInd = 0;
  GetBitData(&src[3], 30, 1, (U08*)(&gpQtyInd), sizeof(gpQtyInd));
  U08 glQtyInd = 0;
  GetBitData(&src[3], 31, 1, (U08*)(&glQtyInd), sizeof(glQtyInd));
  U08 gaQtyInd = 0;
  GetBitData(&src[3], 32, 1, (U08*)(&gaQtyInd), sizeof(gaQtyInd));

  S64 ecefx = 0;
  GetBitData(&src[3], 34, 38, (U08*)(&ecefx), sizeof(ecefx));
  ConvertInt38Sign(&ecefx);

  S64 ecefy = 0;
  GetBitData(&src[3], 74, 38, (U08*)(&ecefy), sizeof(ecefy));
  ConvertInt38Sign(&ecefy);
  S64 ecefz = 0;

  GetBitData(&src[3], 114, 38, (U08*)(&ecefz), sizeof(ecefz));
  ConvertInt38Sign(&ecefz);

  strBuffer.Format("$Rtcm1005,sta-Id=%d,gps-Ind=%d,glo-Ind=%d,gal-Ind=%d,ecef-x=%lld,ecef-y=%lld,ecef-z=%lld",
		satId, gpQtyInd, glQtyInd, gaQtyInd, ecefx, ecefy, ecefz);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	if(convertOnly)
	{
		return;
	}

	if(ecefx == 0 && ecefy == 0 && ecefz == 0)
	{
    return;
	}
  //20191121 Alex move to RTCM MSM7
	//CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
	//CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
	//CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';

	POS_T pos;
	pos.px = (D64)ecefx / 10000;
	pos.py = (D64)ecefy / 10000;
	pos.pz = (D64)ecefz / 10000;

	LLA_T lla;
	CooCartesianToGeodetic(&pos, &lla);

	lla.lat *= R2D;
	lla.lon *= R2D;
	double lat_d = (S16)fabs(lla.lat );
	double lon_d = (S16)fabs(lla.lon );
	double lat_m = fmod( fabs(lla.lat), 1.0) * 60.0;
	double lon_m = fmod( fabs(lla.lon), 1.0) * 60.0;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = lat_d * 100.0 + lat_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = (lla.lat >= 0) ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = lon_d * 100.0 + lon_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = (lla.lon >= 0) ? 'E' : 'W';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = lla.alt - CGPSDlg::gpsDlg->m_gpggaMsgCopy.GeoidalSeparation;
}

void ShowRtcm1033(U08* src, bool convertOnly, CString* pStr)
{
  const int BufferSize = 48;
  U16 satId = 0;
  int pos = 12;
  int ps = 0;

  GetBitData(&src[3], pos, 12, (U08*)(&satId), sizeof(satId));
  pos += 12;

  U08 nCounter = 0; //Antenna Descriptor Counter N
  GetBitData(&src[3], pos, 8, (U08*)(&nCounter), sizeof(nCounter));
  pos += 8;

  U08 antennaDescriptor[BufferSize] = { 0 };
  ps = 3 + pos / 8;
  for(int n = 0; n < nCounter; ++n)
  {
    antennaDescriptor[n] = src[ps + n];
    pos += 8;
  }

  U08 setupId = 0; //Antenna Setup ID
  GetBitData(&src[3], pos, 8, (U08*)(&setupId), sizeof(setupId));
  pos += 8;

  U08 mCounter = 0; //Antenna Serial Number Counter M
  GetBitData(&src[3], pos, 8, (U08*)(&mCounter), sizeof(mCounter));
  pos += 8;

  U08 antennaSerialNumber[BufferSize] = { 0 };
  ps = 3 + pos / 8;
  for(int m = 0; m < nCounter; ++m)
  {
    antennaSerialNumber[m] = src[ps + m];
    pos += 8;
  }

  U08 iCounter = 0; //Receiver Type Descriptor Counter I
  GetBitData(&src[3], pos, 8, (U08*)(&iCounter), sizeof(iCounter));
  pos += 8;

  U08 receiverTypeDescriptor[BufferSize] = { 0 };
  ps = 3 + pos / 8;
  for(int i = 0; i < iCounter; ++i)
  {
    receiverTypeDescriptor[i] = src[ps + i];
    pos += 8;
  }

  U08 jCounter = 0; //Receiver Firmware Version Counter J
  GetBitData(&src[3], pos, 8, (U08*)(&jCounter), sizeof(jCounter));
  pos += 8;

  U08 receiverFirmwareVersion[BufferSize] = { 0 };
  ps = 3 + pos / 8;
  for(int j = 0; j < jCounter; ++j)
  {
    receiverFirmwareVersion[j] = src[ps + j];
    pos += 8;
  }

  U08 kCounter = 0; //Receiver Serial Number Counter K
  GetBitData(&src[3], pos, 8, (U08*)(&kCounter), sizeof(kCounter));
  pos += 8;

  U08 receiverSerialNumber[BufferSize] = { 0 };
  ps = 3 + pos / 8;
  for(int k = 0; k < kCounter; ++k)
  {
    receiverSerialNumber[k] = src[ps + k];
    pos += 8;
  }

  strBuffer.Format("$Rtcm1033,staId=%d,n=%d,\"%s\",setupId=%d,m=%d,\"%s\",i=%d,\"%s\",j=%d,\"%s\",k=%d,\"%s\"",
		satId, nCounter, antennaDescriptor, setupId, mCounter, antennaSerialNumber, 
    iCounter, receiverTypeDescriptor, jCounter, receiverFirmwareVersion, kCounter, receiverSerialNumber);

  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	if(convertOnly)
	{
		return;
	}
}

//#define MAX_NSAT    (64)
//#define MAX_NSIG    (32)
//#define MAX_NCELL   (64)
//typedef struct {
//  U32 tow_ms;
//  U32 glo_tk_ms;
//  U16 msg_id;
//  U08 glo_dow;
//  U08 multi_msg_bit;
//  U08 iods;
//  U08 clk_steering_ind;
//  U08 ext_clk_ind;
//  U08 smooth_ind;
//  U08 smooth_interval;
//  U08 n_sat;
//  U08 n_sig;
//  U08 n_cell;
//  U08 sat_mask[MAX_NSAT];
//  U08 sig_mask[MAX_NSIG];
//  U08 cell_mask[MAX_NCELL];
//} RTCM3_MSM_HEADER_T;

//typedef struct {
//  D64 rough_range;
//  U08 ext_sat_info;
//} RTCM3_MSM_SAT_DATA_T;
//
//typedef struct {
//  D64 pr;
//  D64 phase_range;
//  U16 lock_ind;
//  U08 half_cycle_ind;
//  U08 cn0;
//} RTCM3_MSM_SIG_DATA_T;

//RTCM3_MSM_HEADER_T msm_header = {0};
//RTCM3_MSM_SAT_DATA_T satData[MAX_NSAT] = {0};
//RTCM3_MSM_SIG_DATA_T sigData[MAX_NSIG] = {0};
//
//enum GpsSig {
//  RTCM_MSM_GPS_SIG_L1C = 2,
//  RTCM_MSM_GPS_SIG_L1P = 3,
//  RTCM_MSM_GPS_SIG_L1W = 4,
//  RTCM_MSM_GPS_SIG_L2C = 8,
//  RTCM_MSM_GPS_SIG_L2P = 9,
//  RTCM_MSM_GPS_SIG_L2W = 10,
//  RTCM_MSM_GPS_SIG_L2S = 15,
//  RTCM_MSM_GPS_SIG_L2L = 16,
//  RTCM_MSM_GPS_SIG_L2X = 17,
//  RTCM_MSM_GPS_SIG_L5I = 22,
//  RTCM_MSM_GPS_SIG_L5Q = 23,
//  RTCM_MSM_GPS_SIG_L5X = 24
//};
//enum {
//  RTCM_MSM_SBAS_SIG_L1C = 2,
//  RTCM_MSM_SBAS_SIG_L5I = 22,
//  RTCM_MSM_SBAS_SIG_L5Q = 23,
//  RTCM_MSM_SBAS_SIG_L5X = 24,
//};
//enum QzssSig {
//  RTCM_MSM_QZSS_SIG_L1C = 2,
//  RTCM_MSM_QZSS_SIG_L6S = 9,
//  RTCM_MSM_QZSS_SIG_L6L = 10,
//  RTCM_MSM_QZSS_SIG_L6X = 11,
//  RTCM_MSM_QZSS_SIG_L2S = 15,
//  RTCM_MSM_QZSS_SIG_L2L = 16,
//  RTCM_MSM_QZSS_SIG_L2X = 17,
//  RTCM_MSM_QZSS_SIG_L1S = 30,
//  RTCM_MSM_QZSS_SIG_L1L = 31,
//  RTCM_MSM_QZSS_SIG_L1X = 32
//};
//enum {
//  RTCM_MSM_GLO_SIG_G1C = 2,
//  RTCM_MSM_GLO_SIG_G1P = 3,
//  RTCM_MSM_GLO_SIG_G2C = 8,
//  RTCM_MSM_GLO_SIG_G2P = 9,
//};
//enum {
//  RTCM_MSM_BDS_SIG_B1I = 2,
//  RTCM_MSM_BDS_SIG_B1Q = 3,
//  RTCM_MSM_BDS_SIG_B1X = 4,
//  RTCM_MSM_BDS_SIG_B3I = 8,
//  RTCM_MSM_BDS_SIG_B3Q = 9,
//  RTCM_MSM_BDS_SIG_B3X = 10,
//  RTCM_MSM_BDS_SIG_B2I = 14,
//  RTCM_MSM_BDS_SIG_B2Q = 15,
//  RTCM_MSM_BDS_SIG_B2X = 16,
//};

//int GetNoneZeroBitPosition(U08* d, int size, int order)
//{
//  int count = 0;
//  for(int i = 0; i < size * 8; ++i)
//  {
//    if((d[size - i / 8 - 1] >> (8 - (i % 8) - 1)) & 0x1)
//    {
//      if(count++ == order)
//      {
//        return i;
//      }
//    }
//  }
//  return -1;
//}

//
//#define SPEED_OF_LIGHT  (299792458.0)
//#define RANGE_MS    (SPEED_OF_LIGHT*0.001)    // range in 1 ms
//#define P2_10       (0.0009765625)            // 2^-10
////---------------------------------------------------------
//static S16  rtcm3_decode_msm5_sat_data( S16 len, const U08 buf[],
//  S16 start_bit_ndx, U08 n_sat, RTCM3_MSM_SAT_DATA_T *sat_data_p)
//{
//  U08 i;
//  S16 bit_ndx = start_bit_ndx;
//
//  // DF397 The number of integer milliseconds in GNSS satellite rough ranges
//  for( i=0; i<n_sat; i++ )
//  {
//    U08 tmp;
//    tmp = (U08)EBIT_extract_unsigned_from_bytes( buf, bit_ndx, 8 );
//    bit_ndx += 8;
//    if( tmp != 0xFF )
//      sat_data_p[i].rough_range = (D64)tmp * RANGE_MS;
//  }
//
//  // Extended satellite information
//  for( i=0; i<n_sat; i++ )
//  {
//    sat_data_p[i].ext_sat_info = (U08)EBIT_extract_unsigned_from_bytes( buf, bit_ndx, 4 );
//    bit_ndx += 4;
//  }
//
//  // DF398 GNSS satellite rough ranges modulo 1 millisecond
//  for( i=0; i<n_sat; i++ )
//  {
//    U16 tmp;
//    tmp = (U08)EBIT_extract_unsigned_from_bytes( buf, bit_ndx, 10 );
//    bit_ndx += 10;
//    if( sat_data_p[i].rough_range != 0.0 )
//      sat_data_p[i].rough_range += (D64)tmp * P2_10 * RANGE_MS;
//  }
//
//  // DF399 GNSS satellite rough phaserage rates
//  bit_ndx += 14 * n_sat; // neglect rough phase range rate
//
//  return bit_ndx;
//}
//
//#define P2_29       (1.862645149230957E-09)   // 2^-29
//#define P2_31       (4.656612873077393E-10)   // 2^-31
////---------------------------------------------------------
//static S16  rtcm3_decode_msm6_sig_data(S16 len, const U08 buf[],
//  S16 start_bit_ndx, U08 n_cell, RTCM3_MSM_SIG_DATA_T *sig_data_p)
//{
//  U08 i;
//  S16 bit_ndx = start_bit_ndx;
//
//  // DF405 GNSS signal fine pseudoranges with extended resolution
//  for( i=0; i<n_cell; i++ )
//  {
//    S32 tmp;
//    tmp = EBIT_extract_signed_from_bytes( buf, bit_ndx, 20 );
//    bit_ndx += 20;
//    if( tmp != 0x80000 )
//      sig_data_p[i].pr = (D64)tmp * P2_29 * RANGE_MS;
//  }
//
//  // DF406 GNSS signal fine phaserange data with extended resolution
//  for( i=0; i<n_cell; i++ )
//  {
//    S32 tmp;
//    tmp = EBIT_extract_signed_from_bytes( buf, bit_ndx, 24 );
//    bit_ndx += 24;
//    if( tmp != 0x800000 )
//      sig_data_p[i].phase_range = (D64)tmp * P2_31 * RANGE_MS;
//  }
//
//  // DF407 GNSS phaserange lock time indicator with extended range and resolution
//  for( i=0; i<n_cell; i++ )
//  {
//    sig_data_p[i].lock_ind = (U16)EBIT_extract_unsigned_from_bytes( buf, bit_ndx, 10 );
//    bit_ndx += 10;
//  }
//
//  // DF420 Half-cycle ambiguity indicator
//  for( i=0; i<n_cell; i++ )
//  {
//    sig_data_p[i].half_cycle_ind = (U08)EBIT_extract_1bit_from_bytes( buf, bit_ndx );
//    bit_ndx++;
//  }
//
//  // DF408 GNSS signal CNRs with extended resolution
//  for( i=0; i<n_cell; i++ )
//  {
//    sig_data_p[i].cn0 = (U08)( EBIT_extract_unsigned_from_bytes( buf, bit_ndx, 10 ) * 0.0625 );
//    bit_ndx += 10;
//  }
//
//  return bit_ndx;
//}
//
//enum RTK_GNSS_SYSTEM {
//  RTK_GPS_SYSTEM,
//  RTK_SBAS_SYSTEM,
//  RTK_QZSS_SYSTEM,
//  RTK_GLONASS_SYSTEM,
//  RTK_BEIDOU_SYSTEM,
//  RTK_GALILEO_SYSTEM,
//  RTK_IRNSS_SYSTEM,
//  RTK_NUM_OF_GNSS_SYSTEM
//};
//
//enum RTK_SIGNAL_TYPES {
//  RTK_SIGNAL_L1,
//  RTK_SIGNAL_L2,
//  RTK_SIGNAL_L5,
//  RTK_SIGNAL_L6,
//  RTK_SIGNAL_GLO_L1,
//  RTK_SIGNAL_GLO_L2,
//  RTK_SIGNAL_BD_B1,   // Beidou B1I
//  RTK_SIGNAL_BD_B2,   // Beidou B2I
//  RTK_SIGNAL_BD_B2A,
//  RTK_SIGNAL_BD_B3,   // Beidou B3I
//  RTK_SIGNAL_IRN_L5,
//  RTK_NUM_OF_SIGNAL_TYPES
//};
////---------------------------------------------------------
//static S16  rtcm3_msm_signal_type_converter( U08 rtk_gnss_sys, U08 rtcm3_msm_signal_type,
//                                             U08 *rtk_signal_type )
//{
//  switch( rtk_gnss_sys )
//  {
//    case RTK_GPS_SYSTEM:
//    {
//      enum {
//      RTCM_MSM_GPS_SIG_L1C = 2,
//      RTCM_MSM_GPS_SIG_L1P = 3,
//      RTCM_MSM_GPS_SIG_L1W = 4,
//      RTCM_MSM_GPS_SIG_L2C = 8,
//      RTCM_MSM_GPS_SIG_L2P = 9,
//      RTCM_MSM_GPS_SIG_L2W = 10,
//      RTCM_MSM_GPS_SIG_L2S = 15,
//      RTCM_MSM_GPS_SIG_L2L = 16,
//      RTCM_MSM_GPS_SIG_L2X = 17,
//      RTCM_MSM_GPS_SIG_L5I = 22,
//      RTCM_MSM_GPS_SIG_L5Q = 23,
//      RTCM_MSM_GPS_SIG_L5X = 24
//      };
//
//      if( rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L1C ||
//          rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L1P ||
//          rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L1W   )
//        *rtk_signal_type = RTK_SIGNAL_L1;
//      else if( rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L2C ||
//               rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L2P ||
//               rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L2W ||
//               rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L2S ||
//               rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L2L ||
//               rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L2X   )
//        *rtk_signal_type = RTK_SIGNAL_L2;
//      else if( rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L5I ||
//               rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L5Q ||
//               rtcm3_msm_signal_type == RTCM_MSM_GPS_SIG_L5X   )
//        *rtk_signal_type = RTK_SIGNAL_L5;
//      else
//        return FALSE;
//      break;
//    }
//
//    case RTK_SBAS_SYSTEM:
//    {
//      enum {
//      RTCM_MSM_SBAS_SIG_L1C = 2,
//      RTCM_MSM_SBAS_SIG_L5I = 22,
//      RTCM_MSM_SBAS_SIG_L5Q = 23,
//      RTCM_MSM_SBAS_SIG_L5X = 24,
//      };
//
//      if( rtcm3_msm_signal_type == RTCM_MSM_SBAS_SIG_L1C )
//        *rtk_signal_type = RTK_SIGNAL_L1;
//      else if( rtcm3_msm_signal_type == RTCM_MSM_SBAS_SIG_L5I ||
//               rtcm3_msm_signal_type == RTCM_MSM_SBAS_SIG_L5Q ||
//               rtcm3_msm_signal_type == RTCM_MSM_SBAS_SIG_L5X   )
//        *rtk_signal_type = RTK_SIGNAL_L5;
//      else
//        return FALSE;
//      break;
//    }
//
//    case RTK_QZSS_SYSTEM:
//    {
//      enum {
//      RTCM_MSM_QZSS_SIG_L1C = 2,
//      RTCM_MSM_QZSS_SIG_L6S = 9,
//      RTCM_MSM_QZSS_SIG_L6L = 10,
//      RTCM_MSM_QZSS_SIG_L6X = 11,
//      RTCM_MSM_QZSS_SIG_L2S = 15,
//      RTCM_MSM_QZSS_SIG_L2L = 16,
//      RTCM_MSM_QZSS_SIG_L2X = 17,
//      RTCM_MSM_QZSS_SIG_L1S = 30,
//      RTCM_MSM_QZSS_SIG_L1L = 31,
//      RTCM_MSM_QZSS_SIG_L1X = 32
//      };
//
//      if( rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L1C ||
//          rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L1S ||
//          rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L1L ||
//          rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L1X   )
//        *rtk_signal_type = RTK_SIGNAL_L1;
//      else if( rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L2S ||
//               rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L2L ||
//               rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L2X   )
//        *rtk_signal_type = RTK_SIGNAL_L2;
//      else if( rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L6S ||
//               rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L6L ||
//               rtcm3_msm_signal_type == RTCM_MSM_QZSS_SIG_L6X   )
//        *rtk_signal_type = RTK_SIGNAL_L6;
//      else
//        return FALSE;
//      break;
//    }
//
//    case RTK_GLONASS_SYSTEM:
//    {
//      enum {
//      RTCM_MSM_GLO_SIG_G1C = 2,
//      RTCM_MSM_GLO_SIG_G1P = 3,
//      RTCM_MSM_GLO_SIG_G2C = 8,
//      RTCM_MSM_GLO_SIG_G2P = 9,
//      };
//
//      if( rtcm3_msm_signal_type == RTCM_MSM_GLO_SIG_G1C ||
//          rtcm3_msm_signal_type == RTCM_MSM_GLO_SIG_G1P   )
//        *rtk_signal_type = RTK_SIGNAL_GLO_L1;
//      else if( rtcm3_msm_signal_type == RTCM_MSM_GLO_SIG_G2C ||
//               rtcm3_msm_signal_type == RTCM_MSM_GLO_SIG_G2P   )
//        *rtk_signal_type = RTK_SIGNAL_GLO_L2;
//      else
//        return FALSE;
//      break;
//    }
//
//    case RTK_BEIDOU_SYSTEM:
//    {
//      enum {
//      RTCM_MSM_BDS_SIG_B1I = 2,
//      RTCM_MSM_BDS_SIG_B1Q = 3,
//      RTCM_MSM_BDS_SIG_B1X = 4,
//      RTCM_MSM_BDS_SIG_B3I = 8,
//      RTCM_MSM_BDS_SIG_B3Q = 9,
//      RTCM_MSM_BDS_SIG_B3X = 10,
//      RTCM_MSM_BDS_SIG_B2I = 14,
//      RTCM_MSM_BDS_SIG_B2Q = 15,
//      RTCM_MSM_BDS_SIG_B2X = 16,
//      };
//
//      if( rtcm3_msm_signal_type == RTCM_MSM_BDS_SIG_B1I ||
//          rtcm3_msm_signal_type == RTCM_MSM_BDS_SIG_B1Q ||
//          rtcm3_msm_signal_type == RTCM_MSM_BDS_SIG_B1X   )
//        *rtk_signal_type = RTK_SIGNAL_BD_B1;
//      else if( rtcm3_msm_signal_type == RTCM_MSM_BDS_SIG_B2I ||
//               rtcm3_msm_signal_type == RTCM_MSM_BDS_SIG_B2Q ||
//               rtcm3_msm_signal_type == RTCM_MSM_BDS_SIG_B2X   )
//        *rtk_signal_type = RTK_SIGNAL_BD_B2;
//      else if( rtcm3_msm_signal_type == RTCM_MSM_BDS_SIG_B3I ||
//               rtcm3_msm_signal_type == RTCM_MSM_BDS_SIG_B3Q ||
//               rtcm3_msm_signal_type == RTCM_MSM_BDS_SIG_B3X   )
//        *rtk_signal_type = RTK_SIGNAL_BD_B3;
//      else
//        return FALSE;
//      break;
//    }
//
//    default:
//      return FALSE;
//      break;
//  }
//  return TRUE;
//}
//
//#define NVALID_SIG  (4)
//static S16 Rtcm3_n_valid_signal_types = 4;
//static U08 Rtcm3_valid_signal_types_ary[NVALID_SIG] = 
//{ RTK_SIGNAL_L1, RTK_SIGNAL_L2, RTK_SIGNAL_BD_B1, RTK_SIGNAL_BD_B2 };;
//
////---------------------------------------------------------
//static S16  rtcm3_rtk_signal_type_is_valid( U08 signal_type )
//{
//  U08 i;
//  for( i=0; i<Rtcm3_n_valid_signal_types; i++ )
//    if( signal_type == Rtcm3_valid_signal_types_ary[i] )
//      return TRUE;
//
//  return FALSE;
//}
////---------- functions definition --------------------------------------------//
//D64 RTKWL_get_wave_length( U08 signal_type )
//{
//  assert( signal_type < RTK_NUM_OF_SIGNAL_TYPES );
//  const D64 WAVE_LENGTH_TABLE[] = {
//    (SPEED_OF_LIGHT/1.57542e9),    // GPS L1
//    (SPEED_OF_LIGHT/1.22760e9),    // GPS L2
//    (SPEED_OF_LIGHT/1.17645e9),    // GPS L5
//    (SPEED_OF_LIGHT/1.27875e9),    // GPS L6
//    (SPEED_OF_LIGHT/1.60200e9),    // GLONASS L1 K number 0
//    (SPEED_OF_LIGHT/1.24600e9),    // GLONASS L2 K number 0
//    (SPEED_OF_LIGHT/1.561098e9),   // Beidou B1I
//    (SPEED_OF_LIGHT/1.207140e9),   // Beidou B2I
//    (SPEED_OF_LIGHT/1.17645e9),    // Beidou B2a
//    (SPEED_OF_LIGHT/1.268520e9),   // Beidou B3I
//    (SPEED_OF_LIGHT/1.17645e9) };  // IRNSS L5
//
//  //assert( NELEMS(WAVE_LENGTH_TABLE) == RTK_NUM_OF_SIGNAL_TYPES );
//
//  return WAVE_LENGTH_TABLE[signal_type];
//}
////---------- constant definitions --------------------------
//#define MAX_GPS_NSVS     32
//#define MAX_QZSS_NSVS    5
//#define MAX_SBAS_NSVS    19   // PRN 120~138
//#define MAX_BD_NSVS      37
//#define MAX_GLO_NSVS     24
//#define MAX_IRN_NSVS     7
////---------------------------------------------------------
//static BOOL check_rtcm3_msm_svid_validity( U08 gnss_sys, SINT svid )
//{
//  switch( gnss_sys )
//  {
//    case RTK_GPS_SYSTEM:
//      if( (svid > 0) && (svid <= MAX_GPS_NSVS) )
//        return TRUE;
//      break;
//
//    case RTK_SBAS_SYSTEM:
//      if( (svid > 0) && (svid <= MAX_SBAS_NSVS) )
//        return TRUE;
//      break;
//
//    case RTK_QZSS_SYSTEM:
//      if( (svid > 0) && (svid <= MAX_QZSS_NSVS) )
//        return TRUE;
//      break;
//
//    case RTK_GLONASS_SYSTEM:
//      if( (svid > 0) && (svid <= MAX_GLO_NSVS) )
//        return TRUE;
//      break;
//
//    case RTK_BEIDOU_SYSTEM:
//      if( (svid > 0) && (svid <= MAX_BD_NSVS) )
//        return TRUE;
//      break;
//
//    case RTK_GALILEO_SYSTEM:
//      assert( FALSE );  // not support yet 
//      break;
//
//    default:
//      break;
//  }
//  return FALSE;
//}
//enum RTK_START_SVID {
//  RTK_GPS_START_SVID  = 1,   // GPS: 1~32
//  RTK_GPS_END_SVID    = 32,  // GPS: 1~32
//  RTK_SBAS_START_SVID = 33,  // SBAS: 33~51
//  RTK_SBAS_END_SVID   = 51,  // SBAS: 33~51
//  RTK_QZSS_START_SVID = 52   // QZSS: 52~56
//};
////---------------------------------------------------------
//static void rtcm3_updata_msm_measurements( U08 base_rover, U08 gnss_sys,
//                                           RTCM3_MSM_HEADER_T *header_p,
//                                           RTCM3_MSM_SAT_DATA_T *sat_data_p,
//                                           RTCM3_MSM_SIG_DATA_T *sig_data_p )
//{
//  U08 i, j, rtk_signal_type;
//  U08 cell_ndx = 0;
//  U08 rtk_chan_indicator_bits = 0;
//  //U08 (*cell_mask_mat)[header_p->n_sig] = (U08 (*)[])header_p->cell_mask;
//  U08 *(*cell_mask_mat) = (U08 **)header_p->cell_mask;
//  D64 pr_out = 0;
//  D64 acc_cycles_out = 0;
//  for( i=0; i<header_p->n_sat; i++ )
//    for( j=0; j<header_p->n_sig; j++ )
//    {
//      S08 k_num = 0;
//      S08 can_update_meas = TRUE;
//      //if( !cell_mask_mat[i][j] )
//      //  continue;
//      if( !rtcm3_msm_signal_type_converter( gnss_sys, header_p->sig_mask[j], &rtk_signal_type ) ||
//          !rtcm3_rtk_signal_type_is_valid( rtk_signal_type ) )
//      {
//        cell_ndx++;
//        continue;
//      }
//
//      //D64 wave_length = RTKWL_get_wave_length( rtk_signal_type );
//      //if( gnss_sys == RTK_GLONASS_SYSTEM )
//      //{
//      //  // only MSM5 & MSM7 contain ext_sat_info
//      //  if( (header_p->msg_id == 1085) ||
//      //      (header_p->msg_id == 1087)   )
//      //  {
//      //    k_num = (S08)(sat_data_p[i].ext_sat_info - 7);
//      //    wave_length = RTKWL_get_GLONASS_wave_length( rtk_signal_type, k_num );
//      //  }
//      //  else
//      //    k_num = 10; // invalid value
//      //}
//
//      SINT msm_svid = header_p->sat_mask[i];
//      if( !check_rtcm3_msm_svid_validity( gnss_sys, msm_svid ) )
//      {
//        cell_ndx++;
//        continue;
//      }
//
//      //if( (sig_data_p[cell_ndx].pr == 0.0) ||
//      //    (sat_data_p[i].rough_range == 0.0) ||
//      //    (sig_data_p[cell_ndx].phase_range == 0.0) )
//      //  can_update_meas = FALSE;
//      //else
//      //{
//      //  pr_out = sat_data_p[i].rough_range + sig_data_p[cell_ndx].pr;
//      //  acc_cycles_out = (sat_data_p[i].rough_range +
//      //                    sig_data_p[cell_ndx].phase_range) / wave_length;
//      //  rtk_chan_indicator_bits = 0;  // init at first
//      //  if( rtcm3_check_cycle_slip_and_set_lock_ind(
//      //          base_rover, gnss_sys, msm_svid,
//      //          rtk_signal_type, sig_data_p[cell_ndx].lock_ind ) )
//      //    rtk_chan_indicator_bits |= BIT_RTK_INDICATOR_CYCLE_SLIP;
//      //  if( sig_data_p[cell_ndx].half_cycle_ind )
//      //    rtk_chan_indicator_bits |= BIT_RTK_INDICATOR_HALF_CYCLE_AMBIGUITY;
//      //}
//
//      U08 rtk_svid = msm_svid;
//      // SVID in RTCM3 data field DF394 all start from 1
//      if( gnss_sys == RTK_QZSS_SYSTEM )
//        rtk_svid = rtk_svid - 1 + RTK_QZSS_START_SVID;
//      else if( gnss_sys == RTK_SBAS_SYSTEM )
//        rtk_svid = rtk_svid - 1 + RTK_SBAS_START_SVID;
//
//      //if( can_update_meas )
//      //  RTK_MDATA_update_one_channel_measurement_from_rtcm(
//      //                base_rover, rtk_svid, rtk_signal_type, k_num,
//      //                sig_data_p[cell_ndx].cn0, pr_out, acc_cycles_out, rtk_chan_indicator_bits );
//
//      //if( DO_DEBUG_PRINT_RTCM_IN_MSG )
//      //  DEBUG_PRINTF( DBGPRINT_LEVEL0,
//      //                "rtk_svid: %3u, rtk_signal_type: %u, k_num: %2d, cn0: %2u, "
//      //                "pr:%.4lf, acc_cycle: %.2lf, rt_chan_indicator_bits: %u\n",
//      //                rtk_svid, rtk_signal_type, k_num, sig_data_p[cell_ndx].cn0,
//      //                pr_out, acc_cycles_out, rtk_chan_indicator_bits );
//
//      cell_ndx++;
//    }
//
//  //if( !header_p->multi_msg_bit )
//  //  RTK_MDATA_update_channel_measurements_finished_from_rtcm( base_rover );
//
////  assert( cell_ndx == header_p->n_cell );
//}
/* decode msm 7: full pseudorange, phaserange, phaserangerate and cnr (h-res) */

//static int decode_msm7(U08* buff, int sys)
//{
//    msm_h_t h={0};
//    double r[64],rr[64],pr[64],cp[64],rrf[64],cnr[64];
//    int i,j,type,sync,iod,ncell,rng,rng_m,rate,prv,cpv,rrv,lock[64];
//    int ex[64],half[64];
//    
//    type=getbitu(rtcm->buff,24,12);
//    
//    //* decode msm header */
//    if ((ncell=decode_msm_head(buff,sys,&sync,&iod,&h,&i))<0) return -1;
//    
//    if (i+h.nsat*36+ncell*80>rtcm->len*8) {
//        trace(2,"rtcm3 %d length error: nsat=%d ncell=%d len=%d\n",type,h.nsat,
//              ncell,rtcm->len);
//        return -1;
//    }
//    for (j=0;j<h.nsat;j++) {
//        r[j]=rr[j]=0.0; ex[j]=15;
//    }
//    for (j=0;j<ncell;j++) pr[j]=cp[j]=rrf[j]=-1E16;
//    
//    /* decode satellite data */
//    for (j=0;j<h.nsat;j++) { /* range */
//        rng  =getbitu(rtcm->buff,i, 8); i+= 8;
//        if (rng!=255) r[j]=rng*RANGE_MS;
//    }
//    for (j=0;j<h.nsat;j++) { /* extended info */
//        ex[j]=getbitu(rtcm->buff,i, 4); i+= 4;
//    }
//    for (j=0;j<h.nsat;j++) {
//        rng_m=getbitu(rtcm->buff,i,10); i+=10;
//        if (r[j]!=0.0) r[j]+=rng_m*P2_10*RANGE_MS;
//    }
//    for (j=0;j<h.nsat;j++) { /* phaserangerate */
//        rate =getbits(rtcm->buff,i,14); i+=14;
//        if (rate!=-8192) rr[j]=rate*1.0;
//    }
//    /* decode signal data */
//    for (j=0;j<ncell;j++) { /* pseudorange */
//        prv=getbits(rtcm->buff,i,20); i+=20;
//        if (prv!=-524288) pr[j]=prv*P2_29*RANGE_MS;
//    }
//    for (j=0;j<ncell;j++) { /* phaserange */
//        cpv=getbits(rtcm->buff,i,24); i+=24;
//        if (cpv!=-8388608) cp[j]=cpv*P2_31*RANGE_MS;
//    }
//    for (j=0;j<ncell;j++) { /* lock time */
//        lock[j]=getbitu(rtcm->buff,i,10); i+=10;
//    }
//    for (j=0;j<ncell;j++) { /* half-cycle amiguity */
//        half[j]=getbitu(rtcm->buff,i,1); i+=1;
//    }
//    for (j=0;j<ncell;j++) { /* cnr */
//        cnr[j]=getbitu(rtcm->buff,i,10)*0.0625; i+=10;
//    }
//    for (j=0;j<ncell;j++) { /* phaserangerate */
//        rrv=getbits(rtcm->buff,i,15); i+=15;
//        if (rrv!=-16384) rrf[j]=rrv*0.0001;
//    }
//    /* save obs data in msm message */
//    save_msm_obs(rtcm,sys,&h,r,pr,cp,rr,rrf,cnr,lock,ex,half);
//    
//    rtcm->obsflag=!sync;
//    return sync?0:1;
//}
/*
int ParsingMSM7(U08* src, U08 isGlonass)
{
  S16 len = 0;
  memset(&msm_header, 0, sizeof(RTCM3_MSM_HEADER_T));

  S16 bit_ndx = rtcm3_decode_msm_header(len, src, isGlonass, &msm_header);
  if( !rtcm3_msm_header_is_valid( &msm_header ) )
    return 0;

  // decode satellite data
  //RTCM3_MSM_SAT_DATA_T sat_data[header.n_sat];
  memset( (void *)satData, 0, (sizeof(RTCM3_MSM_SAT_DATA_T) * MAX_NSAT) );
  bit_ndx = rtcm3_decode_msm5_sat_data(len, src, bit_ndx, msm_header.n_sat, satData);

  // decode signal data
  //RTCM3_MSM_SIG_DATA_T sig_data[header.n_cell];
  memset( (void *)sigData, 0, (sizeof( RTCM3_MSM_SIG_DATA_T ) * MAX_NSIG) );
  bit_ndx = rtcm3_decode_msm6_sig_data(len, src, bit_ndx, msm_header.n_cell, sigData );

  // DF404 GNSS signal fine phaserange rates
  bit_ndx += 15 * msm_header.n_cell; // neglect phase_range_rate
  
  U08 base_rover = TRUE;
  rtcm3_updata_msm_measurements( base_rover, isGlonass, &msm_header, satData, sigData );

  return msm_header.n_sat;

  int bitIndex = 0;
  bitIndex += 12;  //pass Message number

  //U16 satId = 0;
  GetBitData(&src[3], bitIndex, 12, (U08*)(&msm_header.sta_id), sizeof(msm_header.sta_id));
  bitIndex += 12;

  GetBitData(&src[3], bitIndex, 30, (U08*)(&msm_header.gnss_epoch_time), sizeof(msm_header.gnss_epoch_time));
  bitIndex += 30;

  GetBitData(&src[3], bitIndex, 1, (U08*)(&msm_header.multi_msg_bit), sizeof(msm_header.multi_msg_bit));
  bitIndex += 1;

  GetBitData(&src[3], bitIndex, 3, (U08*)(&msm_header.iods), sizeof(msm_header.iods));
  bitIndex += 3;

  //Reserved 7 bits
  //Reserved 2 bits for clock steering ind
  //Reserved 2 bits for external clock ind
  bitIndex += 11;

  GetBitData(&src[3], bitIndex, 1, (U08*)(&msm_header.div_free_smoothing_ind), sizeof(msm_header.div_free_smoothing_ind));
  bitIndex += 1;

  GetBitData(&src[3], bitIndex, 3, (U08*)(&msm_header.smoothing_interval), sizeof(msm_header.smoothing_interval));
  bitIndex += 3;

  GetBitData(&src[3], bitIndex, 64, (U08*)(&msm_header.sat_mask), sizeof(msm_header.sat_mask));
  bitIndex += 64;

  int nSat = GetBitFlagCounts((U08*)&msm_header.sat_mask, sizeof(msm_header.sat_mask));
  if(nSat == 0)
  {
    return 0;
  }

  GetBitData(&src[3], bitIndex, 32, (U08*)(&msm_header.sig_mask), sizeof(msm_header.sig_mask));
  bitIndex += 32;

  int nSig = GetBitFlagCounts((U08*)&msm_header.sig_mask, sizeof(msm_header.sig_mask));
  for (int i = 0; i < nSig; ++i)
	{
    msm_header.sig[i] = (U08)GetNoneZeroBitPosition((U08*)(&msm_header.sig_mask), 
      sizeof(msm_header.sig_mask), i) + 1;
  }

  GetBitData(&src[3], bitIndex, nSat, (U08*)(&msm_header.cell_mask), sizeof(msm_header.cell_mask));
  bitIndex += nSat;

  //Table 3.5-81 Content of Satellite Data for MSM5 and MSM7
  int i;
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 8, (U08*)&(satData[i].rough_range_mudulo_1_ms), sizeof(satData[i].rough_range_mudulo_1_ms));
    bitIndex += 8;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 4, (U08*)&(satData[i].ext_sat_info), sizeof(satData[i].ext_sat_info));
    bitIndex += 4;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 10, (U08*)&(satData[i].rough_range_mudulo_1_ms), sizeof(satData[i].rough_range_mudulo_1_ms));
    bitIndex += 10;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 14, (U08*)&(satData[i].rough_phase_range_rate), sizeof(satData[i].rough_phase_range_rate));
    bitIndex += 14;
  }

  //Table 3.5-88 Content of Signal Data for MSM7
  // decode signal data 
  for(i = 0; i < nSig; ++i)
  {
    GetBitData(&src[3], bitIndex, 20, (U08*)&(sigData[i].fine_pseudorange_ext_resol), sizeof(sigData[i].fine_pseudorange_ext_resol));
    bitIndex += 20;
  }
  for(i = 0; i < nSig; ++i)
  {
    GetBitData(&src[3], bitIndex, 24, (U08*)&(sigData[i].fine_phase_range_ext_resol), sizeof(sigData[i].fine_phase_range_ext_resol));
    bitIndex += 24;
  }
  for(i = 0; i < nSig; ++i)
  {
    GetBitData(&src[3], bitIndex, 10, (U08*)&(sigData[i].lock_time_ind_ext_resol), sizeof(sigData[i].lock_time_ind_ext_resol));
    bitIndex += 10;
  }
  for(i = 0; i < nSig; ++i)
  {
    GetBitData(&src[3], bitIndex, 1, (U08*)&(sigData[i].half_cycle_ambiguity_ind), sizeof(sigData[i].half_cycle_ambiguity_ind));
    bitIndex += 1;
  }
  for(i = 0; i < nSig; ++i)
  {
    GetBitData(&src[3], bitIndex, 10, (U08*)&(sigData[i].snr_ext_resol), sizeof(sigData[i].snr_ext_resol));
    if(sigData[i].snr_ext_resol > 0)
    {
      int a = 0;
    }
    bitIndex += 10;
  }
  for(i = 0; i < nSig; ++i)
  {
    GetBitData(&src[3], bitIndex, 15, (U08*)&(sigData[i].fine_phase_range_rate), sizeof(sigData[i].fine_phase_range_rate));
    bitIndex += 15;
  }
  return nSat;
  return 0;
  //*/
//}
// decode msm 7: full pseudorange, phaserange, phaserangerate and cnr (h-res)
typedef struct {                    // multi-signal-message header type
    unsigned int staid;
    unsigned char iod;              // issue of data station
    unsigned char sync;
    unsigned char time_s;           // cumulative session transmitting time
    unsigned char clk_str;          // clock steering indicator
    unsigned char clk_ext;          // external clock indicator
    unsigned char smooth;           // divergence free smoothing indicator
    unsigned char tint_s;           // soothing interval
    unsigned char nsat,nsig;        // number of satellites/signals
    unsigned char ncell;
    unsigned char sats[64];         // satellites
    unsigned char sigs[32];         // signals
    unsigned char cellmask[64];     // cell mask
} msm_h_t;
static msm_h_t msm_header = { 0 };
double msm_cnr[64];

void ShowRtcmMsm7Data(int satNum, const char* title, U08* src, bool convertOnly, CString* pStr)
{
  //return;
  
  strBuffer.Format("$%s,sta_id=0x%04X,multi_msg_bit=0x%02X,iods=0x%02X,div_free_smoothing_ind=0x%02X,smoothing_interval=0x%02X,sig_mask=0x%08X,sat_mask=0x%016llX,cell_mask=0x%016llX",
    title,
    msm_header.staid,
    //msm_header.tow_ms,-
    msm_header.sync,
    msm_header.iod,
    msm_header.smooth,
    msm_header.tint_s,
    msm_header.nsat,
    msm_header.nsig,
    msm_header.ncell);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  
  if(satNum == 0)
  {
    return;
  }
/*
  strBuffer.Format("Content of Satellite Data for MSM5 and MSM7");
  strBuffer.Format("    |Rough Range|Ext. Sat.|Rough Range|Rough Phase|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" NO.|      In ms|     Info|Mudulo 1 ms| Range Rate|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" ---+-----------+---------+-----------+-----------+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  for (int i = 0; i < satNum; ++i)
	{
    strBuffer.Format("  %02d|       0x%02X|     0x%02X|     0x%04X|     0x%04X|",
      i,
      satData[i].rough_range_in_ms,
      satData[i].ext_sat_info,
      satData[i].rough_range_mudulo_1_ms,
      satData[i].rough_phase_range_rate);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
	}

  strBuffer.Format("Content of Signal Data for MSM7");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format("    |    Fine Psedudo |Fine Phase Range|  Time Ind.|    Half Cycle|SNR Ext.|Fine Phase|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" NO.|Range Ext. Resol.|     Ext. Resol.|Ext. Resol.|Ambiguity Ind.|  Resol.|Range Rate|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" ---+-----------------+----------------+-----------+--------------+--------+----------+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  for (int i = 0; i < satNum; ++i)
	{
    strBuffer.Format("  %02d|       0x%08X|      0x%08X|     0x%04X|          0x%02X|  0x%04X|    0x%04X|",
      i,
      sigData[i].fine_pseudorange_ext_resol,
      sigData[i].fine_phase_range_ext_resol,
      sigData[i].lock_time_ind_ext_resol,
      sigData[i].half_cycle_ambiguity_ind,
      sigData[i].snr_ext_resol,
      sigData[i].fine_phase_range_rate);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
	}
  //SetListCtrlRedraw(TRUE);
  */
}


static unsigned int getbitu(const unsigned char *buff, int pos, int len)
{
    unsigned int bits=0;
    int i;
    for (i=pos;i<pos+len;i++) bits=(bits<<1)+((buff[i/8]>>(7-i%8))&1u);
    return bits;
}
static int getbits(const unsigned char *buff, int pos, int len)
{
    unsigned int bits=getbitu(buff,pos,len);
    if (len<=0||32<=len||!(bits&(1u<<(len-1)))) return (int)bits;
    return (int)(bits|(~0u<<len)); /* extend sign */
}

// decode type msm message header
static int decode_msm_head(U08* buff, int len, msm_h_t *h, int *hsize)
{
  //int *sync, int *iod, 
  msm_h_t h0 = { 0 };

  int i = 24, j, mask;

  //int type = getbitu(buff, i, 12); 
  i += 12;

  *h = h0;
  if (i + 157 > len * 8) 
  {
    return -1;
  }

  h->staid = getbitu(buff, i, 12);       
  i += 12;
  i += 30;

  h->sync      = getbitu(buff, i, 1); i+= 1;
  h->iod       = getbitu(buff, i, 3); i+= 3;
  h->time_s  = getbitu(buff, i, 7); i+= 7;
  h->clk_str = getbitu(buff, i, 2); i+= 2;
  h->clk_ext = getbitu(buff, i, 2); i+= 2;
  h->smooth  = getbitu(buff, i, 1); i+= 1;
  h->tint_s  = getbitu(buff, i, 3); i+= 3;

  for (j = 1; j <= 64; j++) 
  {
      mask = getbitu(buff, i, 1); i+=1;
      if (mask) 
        h->sats[h->nsat++] = j;
  }
  for (j = 1; j <= 32; j++) 
  {
      mask = getbitu(buff, i, 1); i+=1;
      if (mask) 
        h->sigs[h->nsig++] = j;
  }
  if (h->nsat * h->nsig > 64) 
  {
      return -1;
  }

  if (i + h->nsat * h->nsig > len * 8) 
  {
      return -1;
  }
  
  h->ncell = 0;
  for (j = 0; j < h->nsat * h->nsig; j++) 
  {
      h->cellmask[j] = getbitu(buff, i, 1); 
      i += 1;
      if (h->cellmask[j])
      {
        h->ncell++;
      }
  }
  *hsize = i;
  return h->ncell;
}

const char *msm_sig_gps[32] = {
    /* GPS: ref [13] table 3.5-87, ref [14][15] table 3.5-91 */
    ""  ,"1C","1P","1W","1Y","1M",""  ,"2C","2P","2W","2Y","2M", /*  1-12 */
    ""  ,""  ,"2S","2L","2X",""  ,""  ,""  ,""  ,"5I","5Q","5X", /* 13-24 */
    ""  ,""  ,""  ,""  ,""  ,"1S","1L","1X"                      /* 25-32 */
};
const char *msm_sig_glo[32]={
    /* GLONASS: ref [13] table 3.5-93, ref [14][15] table 3.5-97 */
    ""  ,"1C","1P",""  ,""  ,""  ,""  ,"2C","2P",""  ,"3I","3Q",
    "3X",""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,
    ""  ,""  ,""  ,""  ,""  ,""  ,""  ,""
};
const char *msm_sig_gal[32] = {
    /* Galileo: ref [15] table 3.5-100 */
    ""  ,"1C","1A","1B","1X","1Z",""  ,"6C","6A","6B","6X","6Z",
    ""  ,"7I","7Q","7X",""  ,"8I","8Q","8X",""  ,"5I","5Q","5X",
    ""  ,""  ,""  ,""  ,""  ,""  ,""  ,""
};
const char *msm_sig_qzs[32] = {
    /* QZSS: ref [15] table 3.5-103 */
    ""  ,"1C",""  ,""  ,""  ,""  ,""  ,""  ,"6S","6L","6X",""  ,
    ""  ,""  ,"2S","2L","2X",""  ,""  ,""  ,""  ,"5I","5Q","5X",
    ""  ,""  ,""  ,""  ,""  ,"1S","1L","1X"
};
const char *msm_sig_sbs[32] = {
    /* SBAS: ref [13] table 3.5-T+005 */
    ""  ,"1C",""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,
    ""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,"5I","5Q","5X",
    ""  ,""  ,""  ,""  ,""  ,""  ,""  ,""
};
const char *msm_sig_cmp[32]={
    /* BeiDou: ref [15] table 3.5-106 */
    ""  ,"1I","1Q","1X",""  ,""  ,""  ,"6I","6Q","6X",""  ,""  ,
    ""  ,"7I","7Q","7X",""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,
    ""  ,""  ,""  ,""  ,""  ,""  ,""  ,""
};
#define CODE_NONE   0                   /* obs code: none or unknown */
#define MAXCODE     48                  /* max number of obs code */
static unsigned char obsfreqs[]={ /* 1:L1,2:L2,3:L5,4:L6,5:L7,6:L8,7:L3 */
    
    0, 1, 1, 1, 1,  1, 1, 1, 1, 1, /*  0- 9 */
    1, 1, 1, 1, 2,  2, 2, 2, 2, 2, /* 10-19 */
    2, 2, 2, 2, 3,  3, 3, 5, 5, 5, /* 20-29 */
    4, 4, 4, 4, 4,  4, 4, 6, 6, 6, /* 30-39 */
    2, 2, 4, 4, 3,  3, 3, 1, 1, 0  /* 40-49 */
};
static char *obscodes[]={       /* observation code strings */
    
    ""  ,"1C","1P","1W","1Y", "1M","1N","1S","1L","1E", /*  0- 9 */
    "1A","1B","1X","1Z","2C", "2D","2S","2L","2X","2P", /* 10-19 */
    "2W","2Y","2M","2N","5I", "5Q","5X","7I","7Q","7X", /* 20-29 */
    "6A","6B","6C","6X","6Z", "6S","6L","8L","8Q","8X", /* 30-39 */
    "2I","2Q","6I","6Q","3I", "3Q","3X","1I","1Q",""    /* 40-49 */
};
#define MAXFREQ     7                   /* max NFREQ */
static char codepris[6][MAXFREQ][16]={  /* code priority table */
   
   /* L1,G1E1a   L2,G2,B1     L5,G3,E5a L6,LEX,B3 E5a,B2    E5a+b */
    {"CPYWMNSL","PYWCMNDSLX","IQX"     ,""       ,""       ,""   }, /* GPS */
    {"PC"      ,"PC"        ,"IQX"     ,""       ,""       ,""   }, /* GLO */
    {"CABXZ"   ,""          ,"IQX"     ,"ABCXZ"  ,"IQX"    ,"IQX"}, /* GAL */
    {"CSLXZ"   ,"SLX"       ,"IQX"     ,"SLX"    ,""       ,""   }, /* QZS */
    {"C"       ,""          ,"IQX"     ,""       ,""       ,""   }, /* SBS */
    {"IQX"     ,"IQX"       ,"IQX"     ,"IQX"    ,"IQX"    ,""   }  /* BDS */
};

extern char *code2obs(unsigned char code, int *freq)
{
    if (freq) *freq=0;
    if (code<=CODE_NONE||MAXCODE<code) return "";
    if (freq) *freq=obsfreqs[code];
    return obscodes[code];
}
extern int getcodepri(int sys, unsigned char code, const char *opt)
{
    const char *p,*optstr;
    char *obs,str[8]="";
    int i,j;
    
    switch (sys) {
        case 0: i=0; optstr="-GL%2s"; break;
        //case SYS_GLO: i=1; optstr="-RL%2s"; break;
        //case SYS_GAL: i=2; optstr="-EL%2s"; break;
        //case SYS_QZS: i=3; optstr="-JL%2s"; break;
        //case SYS_SBS: i=4; optstr="-SL%2s"; break;
        //case SYS_CMP: i=5; optstr="-CL%2s"; break;
        default: return 0;
    }
    obs=code2obs(code,&j);
    
    ///* parse code options */
    //for (p=opt;p && (p=strchr(p,'-'));p++) 
    //{
    //    if (sscanf(p,optstr,str)<1||str[0]!=obs[0]) 
    //      continue;
    //    return str[1]==obs[1]?15:0;
    //}
    /* search code priority */
    return (p=strchr(codepris[i][j-1],obs[1]))?14-(int)(p-codepris[i][j-1]):0;
}
/* get signal index ----------------------------------------------------------*/
#define NFREQ       3
#define NEXOBS      0                   /* number of extended obs codes */
static void sigindex(int sys, const unsigned char *code, const int *freq, int n,
                     const char *opt, int *ind)
{
    int i,nex,pri,pri_h[8]={0},index[8]={0},ex[32]={0};
    
    /* test code priority */
    for (i=0;i<n;i++) 
    {
        if (!code[i]) 
          continue;
        
        if (freq[i]>NFREQ) 
        { /* save as extended signal if freq > NFREQ */
            ex[i]=1;
            continue;
        }
        /* code priority */
        pri=getcodepri(sys,code[i],opt);
        
        /* select highest priority signal */
        if (pri>pri_h[freq[i]-1]) 
        {
            if (index[freq[i]-1]) 
              ex[index[freq[i]-1]-1]=1;
            pri_h[freq[i]-1]=pri;
            index[freq[i]-1]=i+1;
        }
        else 
          ex[i]=1;
    }
    /* signal index in obs data */
    for (i=nex=0;i<n;i++) {
        if (ex[i]==0) ind[i]=freq[i]-1;
        else if (nex<NEXOBS) ind[i]=NFREQ+nex++;
        else { /* no space in obs data */
            //trace(2,"rtcm msm: no space in obs data sys=%d code=%d\n",sys,code[i]);
            ind[i]=-1;
        }
    }
}
#define MINPRNGPS   1                   /* min satellite PRN number of GPS */
#define MAXPRNGPS   32    

#define MINPRNSBS   33                 /* min satellite PRN number of SBAS */
#define MAXPRNSBS   55                 /* max satellite PRN number of SBAS */

#define MINPRNQZS   193                 /* min satellite PRN number of QZSS */
#define MAXPRNQZS   199                 /* max satellite PRN number of QZSS */

#define MINPRNCMP   1                   /* min satellite sat number of BeiDou */
#define MAXPRNCMP   42                  /* max satellite sat number of BeiDou */

#define MINPRNGLO   65                   /* min satellite slot number of GLONASS */
#define MAXPRNGLO   96                  /* max satellite slot number of GLONASS */

#define MINPRNGAL   1                   /* min satellite slot number of GLONASS */
#define MAXPRNGAL   42                  /* max satellite slot number of GLONASS */

/* max satellite PRN number of GPS */
int satno(int sys, int prn)
{
    if(prn <= 0)
    {
      return 0;
    }

    switch(sys) 
    {
        case 0: //SYS_GPS
            //if (prn<MINPRNGPS||MAXPRNGPS<prn) return 0;
            return prn - MINPRNGPS + 1;
        case 1:
            //if (prn<MINPRNSBS||MAXPRNSBS<prn) return 0;
            //return NSATGPS+NSATGLO+NSATGAL+NSATQZS+NSATCMP+NSATLEO+prn-MINPRNSBS+1;
            return prn + MINPRNSBS - 1;
        case 2: //SYS_QZS
            //if (prn<MINPRNQZS||MAXPRNQZS<prn) return 0;
            //return NSATGPS+NSATGLO+NSATGAL+prn-MINPRNQZS+1;
            return prn + MINPRNQZS - 1;
        case 3: //SYS_CMP
            //if (prn<MINPRNQZS||MAXPRNQZS<prn) return 0;
            //return NSATGPS+NSATGLO+NSATGAL+prn-MINPRNQZS+1;
            return prn + MINPRNCMP - 1;
        case 4: //SYS_GLO:
            //if (prn<MINPRNGLO||MAXPRNGLO<prn) return 0;
            return prn + MINPRNGLO - 1;
        case 5:
            //if (prn<MINPRNGAL||MAXPRNGAL<prn) return 0;
            return prn + MINPRNGAL - 1;;
    }
    return 0;
}
extern unsigned char obs2code(const char *obs, int *freq)
{
    int i;
    if (freq) 
      *freq=0;
    for (i=1;*obscodes[i];i++) 
    {
        if (strcmp(obscodes[i],obs)) continue;
        if (freq) *freq=obsfreqs[i];
        return (unsigned char)i;
    }
    return CODE_NONE;
}

static void save_msm_obs(int sys, msm_h_t *h, const double *cnr, Satellites* sate)
{ 
    const char *sig[32];
    unsigned char code[32];
    int i, j, k, prn, sat, index = 0, freq[32], ind[32];

    /* id to signal */
    for (i = 0; i < h->nsig; i++) 
    {
      int sigId = 0;
      switch (sys) 
      {
        case 0: 
          sig[i] = msm_sig_gps[h->sigs[i] - 1]; 
          if(h->sigs[i] == 2) sigId = 1;
          else if(h->sigs[i] == 16) sigId = 6;
          break;
            //case SYS_GAL: sig[i]=msm_sig_gal[h->sigs[i]-1]; break;
        case 1: 
          sig[i] = msm_sig_sbs[h->sigs[i] - 1]; 
          break;
        case 2:   //SYS_QZS
           sig[i] = msm_sig_qzs[h->sigs[i] - 1]; 
           break;
        case 3: //SYS_CMP: 
          sig[i] = msm_sig_cmp[h->sigs[i] - 1]; 
          if(h->sigs[i] == 2) sigId = 1;
          else if(h->sigs[i] == 14) sigId = 3;
          break;
        case 4: //SYS_GLO: 
          sig[i] = msm_sig_glo[h->sigs[i] - 1];
          if(h->sigs[i] == 2) sigId = 1;
          else if(h->sigs[i] == 8) sigId = 3;
          break;
        case 5: //SYS_GAL: 
          sig[i] = msm_sig_gal[h->sigs[i] - 1];
          if(h->sigs[i] == 2) sigId = 7;
          else if(h->sigs[i] == 15) sigId = 2;
          break;
        default: 
          sig[i]=""; 
          break;
      }
      if(sys == 0 || sys == 3 || sys == 4|| sys == 5)
      {
        sate->AddSnrSigId(sigId);
      }
      else
      {
        int aaaq = 0;
      }
      /* signal to rinex obs type */
      code[i] = obs2code(sig[i], freq + i);
      
      /* freqency index for beidou */
      //if (sys==SYS_CMP) {
      //    if      (freq[i]==5) freq[i]=2; /* B2 */
      //    else if (freq[i]==4) freq[i]=3; /* B3 */
      //}
    }
    
    /* get signal index */
    sigindex(sys, code, freq, h->nsig, "-EPHALL", ind);
    
    for (i=j=0;i<h->nsat;i++) 
    {
        
        prn = h->sats[i];
        //if      (sys==SYS_QZS) prn+=MINPRNQZS-1;
        //else if (sys==SYS_SBS) prn+=MINPRNSBS-1;
        if(prn == 0)
        {
          int aaa = 0;
        }

        sat = satno(sys, prn);
        if(sat == 0)
        {
          int aaa = 0;
        }
        for (k=0 ;k < h->nsig; k++) 
        {
            if (!h->cellmask[k + i * h->nsig]) 
              continue;
            
            //if (sat && index >= 0 && ind[k] >= 0) 
            {
              int sid = sate->GetSnrSigId(k);
              F32 s = (F32)(cnr[j]);
              SnrTable snr(sid, s);
              sate->SetSate(sat, 0, 0, snr);
            }
            j++;
        }
    }
}

static int decode_msm7(U08 *buff, int len)
{
  int i, j;
  if (decode_msm_head(buff, len, &msm_header, &i) < 0) 
  {
    return -1;
  }
  
  if (i + msm_header.nsat * 36 + msm_header.ncell * 80 > len * 8) 
  {
      return -1;
  }

  i += msm_header.nsat * 8;
  i += msm_header.nsat * 4;
  i += msm_header.nsat * 10;
  i += msm_header.nsat * 14;
  i += msm_header.ncell * 20;
  i += msm_header.ncell * 24;
  i += msm_header.ncell * 10;
  i += msm_header.ncell * 1;

  for (j = 0; j < msm_header.ncell; ++j) 
  { // cnr 
      msm_cnr[j] = getbitu(buff, i, 10) * 0.0625; 
      i += 10;
  }

  i += msm_header.ncell * 15;
  return msm_header.sync ? 0 : 1;
}

//static int decode_msm7(rtcm_t *rtcm, int sys)

void ShowRtcm1077(U08* src, int len, bool convertOnly, CString* pStr)
{
  decode_msm7(src, len);
  int satNum = msm_header.nsat;
  ShowRtcmMsm7Data(satNum, "Rtcm1077", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Gps, 1, 32);
  save_msm_obs(0, &msm_header, msm_cnr, &(CGPSDlg::gpsDlg->nmea.satellites_gp));

	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
}

void ShowRtcm1107(U08* src, int len, bool convertOnly, CString* pStr)
{
  decode_msm7(src, len);
  int satNum = msm_header.nsat;
  ShowRtcmMsm7Data(satNum, "Rtcm1107", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Gps, 33, 64);
  save_msm_obs(1, &msm_header, msm_cnr, &(CGPSDlg::gpsDlg->nmea.satellites_gp));

	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
}

void ShowRtcm1117(U08* src, int len, bool convertOnly, CString* pStr)
{
  decode_msm7(src, len);
  int satNum = msm_header.nsat;
  ShowRtcmMsm7Data(satNum, "Rtcm1117", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Gps, 193, 199);
  save_msm_obs(2, &msm_header, msm_cnr, &(CGPSDlg::gpsDlg->nmea.satellites_gp));

	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
}

void ShowRtcm1127(U08* src, int len, bool convertOnly, CString* pStr)
{
  decode_msm7(src, len);
  int satNum = msm_header.nsat;
  ShowRtcmMsm7Data(satNum, "Rtcm1127", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Beidou, 1, 48);
  save_msm_obs(3, &msm_header, msm_cnr, &(CGPSDlg::gpsDlg->nmea.satellites_bd));
               
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
  //*/
}

void ShowRtcm1087(U08* src, int len, bool convertOnly, CString* pStr)
{
  decode_msm7(src, len);
  int satNum = msm_header.nsat;
  ShowRtcmMsm7Data(satNum, "Rtcm1087", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Glonass, 65, 100);
  save_msm_obs(4, &msm_header, msm_cnr, &(CGPSDlg::gpsDlg->nmea.satellites_gl));
              
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
}

void ShowRtcm1097(U08* src, int len, bool convertOnly, CString* pStr)
{
  decode_msm7(src, len);
  int satNum = msm_header.nsat;
  ShowRtcmMsm7Data(satNum, "Rtcm1097", src, convertOnly, pStr);
//return;
  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Galileo, 1, 36);
  save_msm_obs(5, &msm_header, msm_cnr, &(CGPSDlg::gpsDlg->nmea.satellites_ga));
              
	CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_gagsvMsgCopy.SequenceNum = 1;

  CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
}

void ShowRtcmEphemeris(U08* src, int len, bool convertOnly, CString* pStr)
{
  CString strBuffer, strConst;
  U16 msgType = (src[3] << 4) | (src[4] >> 4);

  switch(msgType)
  {
  case 1019:
    strConst = "GPS Ephemeris";
    break;
  case 1020:
    strConst = "GLONASS Ephemeris";
    break;
  case 1042:
    strConst = "BeiDou Ephemeris";
    break;
  case 1046:
    strConst = "Galileo Ephemeris";
    break;
  }
  strBuffer.Format("$Rtcm%d,%s", msgType, strConst);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
}

void ShowUbxTimTp(U08* src, bool convertOnly, CString* pStr)
{
  U32 towMs = 0;
  U32 towSubMs = 0;
  S32 qErr = 0;
  U16 week = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&towMs), sizeof(towMs));
  GetByteDataFromLE(&src[6], 4, 4, (U08*)(&towSubMs), sizeof(towSubMs));
  GetByteDataFromLE(&src[6], 8, 4, (U08*)(&qErr), sizeof(qErr));
  GetByteDataFromLE(&src[6], 12, 2, (U08*)(&week), sizeof(week));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-TIM-TP,%d,%d,%d,%d",
      towMs, towSubMs, qErr, week);
  }
  else
  {
    strBuffer.Format("$UBX-TIM-TP(0x0D,0x01),towMS=%d,towSubMS=%d,qErr=%d,week=%d",
      towMs, towSubMs, qErr, week);
  }
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

	UtcTime utc;
	UtcConvertGpsToUtcTime(week, (towMs / 1000.0), &utc);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;
}

void ShowUbxNavSol(U08* src, bool convertOnly, CString* pStr)
{
  U32 towMs = 0;
  U32 towNs = 0;
  U16 week = 0;
  U08 fixMode = 0, flags = 0;
  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&towMs), sizeof(towMs));
  GetByteDataFromLE(&src[6], 4, 4, (U08*)(&towNs), sizeof(towNs));
  GetByteDataFromLE(&src[6], 8, 2, (U08*)(&week), sizeof(week));
  GetByteDataFromLE(&src[6], 10, 1, (U08*)(&fixMode), sizeof(fixMode));
  GetByteDataFromLE(&src[6], 11, 1, (U08*)(&flags), sizeof(flags));

  S32 ecefX = 0, ecefY = 0, ecefZ = 0;
  U32 pAcc = 0;
  GetByteDataFromLE(&src[6], 12, 4, (U08*)(&ecefX), sizeof(ecefX));
  GetByteDataFromLE(&src[6], 16, 4, (U08*)(&ecefY), sizeof(ecefY));
  GetByteDataFromLE(&src[6], 20, 4, (U08*)(&ecefZ), sizeof(ecefZ));
  GetByteDataFromLE(&src[6], 24, 4, (U08*)(&pAcc), sizeof(pAcc));

  S32 ecefVX = 0, ecefVY = 0, ecefVZ = 0;
  U32 sAcc = 0;
  GetByteDataFromLE(&src[6], 28, 4, (U08*)(&ecefVX), sizeof(ecefVX));
  GetByteDataFromLE(&src[6], 32, 4, (U08*)(&ecefVY), sizeof(ecefVY));
  GetByteDataFromLE(&src[6], 36, 4, (U08*)(&ecefVZ), sizeof(ecefVZ));
  GetByteDataFromLE(&src[6], 40, 4, (U08*)(&sAcc), sizeof(sAcc));

  U16 pDop = 0;
  U08 numSv = 0;
  U08 rev1 = 0, rev2 = 0;
  GetByteDataFromLE(&src[6], 44, 2, (U08*)(&pDop), sizeof(pDop));
  GetByteDataFromLE(&src[6], 46, 1, (U08*)(&rev1), sizeof(rev1));
  GetByteDataFromLE(&src[6], 47, 1, (U08*)(&numSv), sizeof(numSv));
  GetByteDataFromLE(&src[6], 48, 1, (U08*)(&rev2), sizeof(rev2));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-SOL,%d,%d,%d,%d,%08Xh," \
      "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
      towMs, towNs, week, fixMode, flags,
      ecefX, ecefY, ecefZ, pAcc, ecefVX, ecefVY, ecefVZ, sAcc, pDop, numSv);
  }
  else
  {
    /*
    strBuffer.Format("$UBX-NAV-SOL(0x01,0x06),iTOW=%d,fTOW=%d,week=%d,gpsFix=%d,flags=0x%08X," \
      "ecefX=%d,ecefY=%d,ecefZ=%d,pAcc=%d,ecefVX=%d,ecefVY=%d,ecefVZ=%d,sAcc=%d,pDop=%d,numSv=%d",
      towMs, towNs, week, fixMode, flags,
      ecefX, ecefY, ecefZ, pAcc, ecefVX, ecefVY, ecefVZ, sAcc, pDop, numSv);
    */
    strBuffer.Format("$UBX-NAV-SOL(0x01,0x06),rev1=%d,rev2=%d,iTOW=%d,fTOW=%d,week=%d,gpsFix=%d,flags=0x%08X," \
      "ecefX=%d,ecefY=%d,ecefZ=%d,pAcc=%d,ecefVX=%d,ecefVY=%d,ecefVZ=%d,sAcc=%d,pDop=%d,numSv=%d",
      rev1,rev2,towMs, towNs, week, fixMode, flags,
      ecefX, ecefY, ecefZ, pAcc, ecefVX, ecefVY, ecefVZ, sAcc, pDop, numSv);
  }

  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

  if(fixMode == 0)
  {
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '0';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 1;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'V';
    //return;
  }
	else if(fixMode == 2 || fixMode == 3)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = fixMode;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(fixMode == 4 || fixMode == 5)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(fixMode == 6)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '2';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'D';
	}
	else if(fixMode == 7)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '5';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'F';
	}
	else if(fixMode == 8)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '4';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'R';
	}
  CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = numSv;

	UtcTime utc;
	UtcConvertGpsToUtcTime(week, (towMs / 1000.0), &utc);
 
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;
  if(fixMode == 0)
  {
    return;
  }
 // D64 latitude, longitude, altitude;
 // ConvertEcefToLonLatAlt(ecefX / 100, ecefY / 100, ecefZ / 100, latitude, longitude, altitude);
	//CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = latitude;
	//CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = (latitude >= 0) ? 'N' : 'S';
	//CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = longitude;
	//CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = (longitude >= 0) ? 'E' : 'W';
	//CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = (F32)altitude - CGPSDlg::gpsDlg->m_gpggaMsgCopy.GeoidalSeparation;
}

void ShowUbxNavPvt(U08* src, bool convertOnly, CString* pStr)
{
  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-PVT");
  }
  else
  {
    strBuffer.Format("$UBX-NAV-PVT(0x01,0x07)");
  }

  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}
}

void ShowUbxNavClock(U08* src, bool convertOnly, CString* pStr)
{
  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-CLOCK");
  }
  else
  {
    strBuffer.Format("$UBX-NAV-CLOCK(0x01,0x22)");
  }

  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}
}

//U08 AdjustUbloxPrn(U08 p)
//{
//    if(p >= 1 && p <= 32)
//    { //GPS
//      return p;
//    }
//    else if(p >= 120 && p <= 148)
//    { //SBAS
//      return p - 97;
//    }
//    else if(p >= 65 && p <= 96)
//    { //GLONASS
//      return p;
//    }
//    else if(p >= 33 && p <= 64)
//    { //Beidou
//      return p + 168;
//    }
//    return p;
//}

void ShowUbxNavSvInfo(U08* src, bool convertOnly, CString* pStr)
{
  U32 towMs = 0;
  U08 numCh = 0;
  U08 globalFlags = 0;
  U16 rev = 0;
  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&towMs), sizeof(towMs));
  GetByteDataFromLE(&src[6], 4, 1, (U08*)(&numCh), sizeof(numCh));
  GetByteDataFromLE(&src[6], 5, 1, (U08*)(&globalFlags), sizeof(globalFlags));
  GetByteDataFromLE(&src[6], 6, 2, (U08*)(&rev), sizeof(rev));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-SVINFO,%d,%d,%02Xh", towMs, numCh, globalFlags);
  }
  else
  {
    //strBuffer.Format("$UBX-NAV-SVINFO(0x01,0x30),iTOW=%d,numCh=%d,globalFlags=0x%02X", towMs, numCh, globalFlags);
    strBuffer.Format("$UBX-NAV-SVINFO(0x01,0x30),rev=%d,iTOW=%d,numCh=%d,globalFlags=0x%02X", rev, towMs, numCh, globalFlags);
  }
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  
  if(numCh == 0 || convertOnly)
  {
    return;
  }

  strBuffer.Format("  chn|svid|flags| qty|cn0|ele| azm|prRes|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  strBuffer.Format(" ----+----+-----+----+---+---+----+-----+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	int fixed_gps_c = 0, fixed_glonass_c = 0, fixed_beidou_c = 0, fixed_galileo_c = 0, fixed_navic_c = 0;
	int gps_c = 0, glonass_c = 0, beidou_c = 0, galileo_c = 0, navic_c = 0;
	SV_CH_DATA sv;
	if(!convertOnly)
	{
		memset(CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID));
	}

	for (int i = 0; i < numCh; ++i)
	{
    S32 prRes = 0;
    S08 elev = 0;
    GetByteDataFromLE(&src[6], 8 + 12 * i, 1, (U08*)(&sv.channel_id), sizeof(sv.channel_id));
    GetByteDataFromLE(&src[6], 9 + 12 * i, 1, (U08*)(&sv.prn), sizeof(sv.prn));
    GetByteDataFromLE(&src[6], 10 + 12 * i, 1, (U08*)(&sv.SV_status), sizeof(sv.SV_status));
    GetByteDataFromLE(&src[6], 11 + 12 * i, 1, (U08*)(&sv.URA), sizeof(sv.URA));
    GetByteDataFromLE(&src[6], 12 + 12 * i, 1, (U08*)(&sv.cn0), sizeof(sv.cn0));
    GetByteDataFromLE(&src[6], 13 + 12 * i, 1, (U08*)(&elev), sizeof(elev));
    GetByteDataFromLE(&src[6], 14 + 12 * i, 2, (U08*)(&sv.azimuth), sizeof(sv.azimuth));
    GetByteDataFromLE(&src[6], 16 + 12 * i, 4, (U08*)(&prRes), sizeof(prRes));

    strBuffer.Format("%5d|%4d| 0x%02X|%4d|%3d|%3d|%4d|%5d|",
			sv.channel_id, sv.prn, sv.SV_status, sv.URA,sv.cn0, 
			elev, sv.azimuth, prRes);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
	  if(convertOnly)
	  {
		  continue;
	  }

    //sv.prn = AdjustUbloxPrn(sv.prn);
    //NMEA::GNSS_System gs = NMEA::GetGNSSSystem(sv.prn);
    NMEA::GNSS_System gs = NMEA::GetUbloxSystem(sv.prn);
		if(NMEA::Gps == gs)
		{
			if(gps_c == 0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gp.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_gp.AddSnrSigId(0);
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID[fixed_gps_c] = sv.prn;
				fixed_gps_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_gp.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			gps_c++;
		}
		else if(NMEA::Glonass == gs)
		{
			if(glonass_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gl.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_gl.AddSnrSigId(0);
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID[fixed_glonass_c] = sv.prn;
				fixed_glonass_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_gl.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			glonass_c++;
    }
		else if(NMEA::Beidou == gs)
		{
			if(beidou_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_bd.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_bd.AddSnrSigId(0);
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID[fixed_beidou_c] = sv.prn;
				fixed_beidou_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_bd.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			beidou_c++;
    }
		else if(NMEA::Galileo == gs)
		{
			if(galileo_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_ga.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_ga.AddSnrSigId(0);
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID[fixed_galileo_c] = sv.prn;
				fixed_galileo_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_ga.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			galileo_c++;
    }
		else if(NMEA::Navic == gs)
		{
			if(navic_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gi.Clear();
        CGPSDlg::gpsDlg->nmea.satellites_gi.AddSnrSigId(0);
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID[fixed_navic_c] = sv.prn;
				fixed_navic_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_gi.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			navic_c++;
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

	  CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfSate = galileo_c;
	  CGPSDlg::gpsDlg->m_gagsvMsgCopy.NumOfMessage = (galileo_c + 3) / 4;
	  CGPSDlg::gpsDlg->m_gagsvMsgCopy.SequenceNum = (galileo_c + 3) / 4;

	  CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfSate = navic_c;
	  CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfMessage = (navic_c + 3) / 4;
	  CGPSDlg::gpsDlg->m_gigsvMsgCopy.SequenceNum = (navic_c + 3) / 4;
  }

}

void ShowUbxNavSvStatus(U08* src, bool convertOnly, CString* pStr)
{
  U32 towMs = 0;
  U08 gpsFix = 0;
  U08 flags = 0;
  U08 fixStat = 0;
  U08 flags2 = 0;
  U32 ttff = 0;
  U32 msss = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&towMs), sizeof(towMs));
  GetByteDataFromLE(&src[6], 4, 1, (U08*)(&gpsFix), sizeof(gpsFix));
  GetByteDataFromLE(&src[6], 5, 1, (U08*)(&flags), sizeof(flags));
  GetByteDataFromLE(&src[6], 6, 1, (U08*)(&fixStat), sizeof(fixStat));
  GetByteDataFromLE(&src[6], 7, 1, (U08*)(&flags2), sizeof(flags2));
  GetByteDataFromLE(&src[6], 8, 4, (U08*)(&ttff), sizeof(ttff));
  GetByteDataFromLE(&src[6], 12, 4, (U08*)(&msss), sizeof(msss));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-STATUS,%d,%d,%02Xh,%02Xh," \
      "%d,%d,%d", towMs, gpsFix, flags, fixStat, flags2, ttff, msss);
  }
  else
  {
    strBuffer.Format("$UBX-NAV-STATUS(0x01,0x03),iTOW=%d,gpsFix=%d,flags=0x%02X,fixStat=0x%02X," \
      "flags2=%d,ttff=%d,msss=%d", towMs, gpsFix, flags, fixStat, flags2, ttff, msss);
  }
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  //if(convertOnly)
	//{
	//	return;
	//}
}

void ShowUbxNavPosllh(U08* src, bool convertOnly, CString* pStr)
{
  U32 iTOW = 0;
  S32 lon = 0;
  S32 lat = 0;
  S32 height = 0;
  S32 hMSL = 0;
  U32 hAcc = 0;
  U32 vAcc = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&iTOW), sizeof(iTOW));
  GetByteDataFromLE(&src[6], 4, 4, (U08*)(&lon), sizeof(lon));
  GetByteDataFromLE(&src[6], 8, 4, (U08*)(&lat), sizeof(lat));
  GetByteDataFromLE(&src[6], 12, 4, (U08*)(&height), sizeof(height));
  GetByteDataFromLE(&src[6], 16, 4, (U08*)(&hMSL), sizeof(hMSL));
  GetByteDataFromLE(&src[6], 20, 4, (U08*)(&hAcc), sizeof(hAcc));
  GetByteDataFromLE(&src[6], 24, 4, (U08*)(&vAcc), sizeof(vAcc));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-POSLLH,%d,%d,%d,%d," \
      "%d,%d,%d", iTOW, lon, lat, height, hMSL, hAcc, vAcc);
  }
  else
  {
    strBuffer.Format("$UBX-NAV-POSLLH(0x01,0x02),iTOW=%d,lon=%d,lat=%d,height=%d," \
      "hMSL=%d,hAcc=%d,vAcc=%d", iTOW, lon, lat, height, hMSL, hAcc, vAcc);
  }
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

  S16 lat_deg = (S16)(lat / 10000000);
  S16 lon_deg = (S16)(lon / 10000000);
  D64 lat_m = (D64)(lat % 10000000) / 10000000 * 60;
  D64 lon_m = (D64)(lon % 10000000) / 10000000 * 60;;


  D64 latitude = lat_deg * 100 + lat_m;
  D64 longitude = lon_deg * 100 + lon_m;
  F32 altitude = (F32)((D64)hMSL / 1000);

  CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = latitude;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = (latitude >= 0) ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = longitude;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = (longitude >= 0) ? 'E' : 'W';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = altitude;
  CGPSDlg::gpsDlg->m_gpggaMsgCopy.GeoidalSeparation = (F32)(height - hMSL) / 1000;
}

void ShowUbxNavDop(U08* src, bool convertOnly, CString* pStr)
{
  U32 iTOW = 0;
  U16 gDOP = 0;
  U16 pDOP = 0;
  U16 tDOP = 0;
  U16 vDOP = 0;
  U16 hDOP = 0;
  U16 nDOP = 0;
  U16 eDOP = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&iTOW), sizeof(iTOW));
  GetByteDataFromLE(&src[6], 4, 2, (U08*)(&gDOP), sizeof(gDOP));
  GetByteDataFromLE(&src[6], 6, 2, (U08*)(&pDOP), sizeof(pDOP));
  GetByteDataFromLE(&src[6], 8, 2, (U08*)(&tDOP), sizeof(tDOP));
  GetByteDataFromLE(&src[6], 10, 2, (U08*)(&vDOP), sizeof(vDOP));
  GetByteDataFromLE(&src[6], 12, 2, (U08*)(&hDOP), sizeof(hDOP));
  GetByteDataFromLE(&src[6], 14, 2, (U08*)(&nDOP), sizeof(nDOP));
  GetByteDataFromLE(&src[6], 16, 2, (U08*)(&eDOP), sizeof(eDOP));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-DOP,%d,%d,%d,%d," \
      "%d,%d,%d,%d", iTOW, gDOP, pDOP, tDOP, vDOP, hDOP, nDOP, eDOP);
  }
  else
  {
    strBuffer.Format("$UBX-NAV-DOP(0x01,0x04),iTOW=%d,gDOP=%d,pDOP=%d,tDOP=%d," \
      "vDOP=%d,hDOP=%d,nDOP=%d,eDOP=%d", iTOW, gDOP, pDOP, tDOP, vDOP, hDOP, nDOP, eDOP);
  }

  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = (F32)pDOP / 1000;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.HDOP = (F32)hDOP / 1000;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = (F32)vDOP / 1000;
}

void ShowUbxNavVelned(U08* src, bool convertOnly, CString* pStr)
{
  U32 iTOW = 0;
  U16 velN = 0;
  U16 velE = 0;
  U16 velD = 0;
  U32 speed = 0;
  U32 gSpeed = 0;
  S32 heading = 0;
  U32 sAcc = 0;
  U32 cAcc = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&iTOW), sizeof(iTOW));
  GetByteDataFromLE(&src[6], 4, 4, (U08*)(&velN), sizeof(velN));
  GetByteDataFromLE(&src[6], 8, 4, (U08*)(&velE), sizeof(velE));
  GetByteDataFromLE(&src[6], 12, 4, (U08*)(&velD), sizeof(velD));
  GetByteDataFromLE(&src[6], 16, 4, (U08*)(&speed), sizeof(speed));
  GetByteDataFromLE(&src[6], 20, 4, (U08*)(&gSpeed), sizeof(gSpeed));
  GetByteDataFromLE(&src[6], 24, 4, (U08*)(&heading), sizeof(heading));
  GetByteDataFromLE(&src[6], 28, 4, (U08*)(&sAcc), sizeof(sAcc));
  GetByteDataFromLE(&src[6], 32, 4, (U08*)(&cAcc), sizeof(cAcc));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-VELEND,%d,%d,%d,%d,%d,%d,%d,%d,%d", 
      iTOW, velN, velE, velD, speed, gSpeed, heading, sAcc, cAcc);
  }
  else
  {
    strBuffer.Format("$UBX-NAV-VELEND(0x01,0x12),iTOW=%d,velN=%d,velE=%d,velD=%d," \
      "speed=%d,gSpeed=%d,heading=%d,sAcc=%d,cAcc=%d", 
      iTOW, velN, velE, velD, speed, gSpeed, heading, sAcc, cAcc);
  }

  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

  CGPSDlg::gpsDlg->m_gprmcMsgCopy.SpeedKnots = (F32)gSpeed / 100 * MS2KMHR / KNOTS2KMHR;
  CGPSDlg::gpsDlg->m_gprmcMsgCopy.TrueCourse = (F32)heading / 100000;
}

void ShowStqSpecialDrInfo(U08* src, bool convertOnly, CString* pStr)
{
  U32 timeTag = 0;
  U16 flags = 0;
  U16 id = 0;
  U32 data1 = 0;  //x-axis gyroscope
  U32 data2 = 0;  //y-axis gyroscope
  U32 data3 = 0;  //z-axis gyroscope
  U32 data4 = 0;  //x-axis accelerometer
  U32 data5 = 0;  //y-axis accelerometer
  U32 data6 = 0;  //z-axis accelerometer
  U32 data7 = 0;  //gyroscope temperature
  U32 data8 = 0;  //speed
  U32 data9 = 0;  //speed


  GetByteDataFromLE(&src[6],  0, 4, (U08*)(&timeTag), sizeof(timeTag));
  GetByteDataFromLE(&src[6],  4, 2, (U08*)(&flags), sizeof(flags));
  GetByteDataFromLE(&src[6],  6, 2, (U08*)(&id), sizeof(id));
  GetByteDataFromLE(&src[6],  8, 4, (U08*)(&data1), sizeof(data1));
  data1 = ((data1 & 0x800000) != 0) ? (data1 | 0xFF800000) : (data1 & 0x00FFFFFF);
  GetByteDataFromLE(&src[6], 12, 4, (U08*)(&data2), sizeof(data2));
  data2 = ((data2 & 0x800000) != 0) ? (data2 | 0xFF800000) : (data2 & 0x00FFFFFF);
  GetByteDataFromLE(&src[6], 16, 4, (U08*)(&data3), sizeof(data3));
  data3 = ((data3 & 0x800000) != 0) ? (data3 | 0xFF800000) : (data3 & 0x00FFFFFF);

  GetByteDataFromLE(&src[6], 20, 4, (U08*)(&data4), sizeof(data4));
  data4 = ((data4 & 0x800000) != 0) ? (data4 | 0xFF800000) : (data4 & 0x00FFFFFF);
  GetByteDataFromLE(&src[6], 24, 4, (U08*)(&data5), sizeof(data5));
  data5 = ((data5 & 0x800000) != 0) ? (data5 | 0xFF800000) : (data5 & 0x00FFFFFF);
  GetByteDataFromLE(&src[6], 28, 4, (U08*)(&data6), sizeof(data6));
  data6 = ((data6 & 0x800000) != 0) ? (data6 | 0xFF800000) : (data6 & 0x00FFFFFF);

  GetByteDataFromLE(&src[6], 32, 4, (U08*)(&data7), sizeof(data7));
  data7 = ((data7 & 0x800000) != 0) ? (data7 | 0xFF800000) : (data7 & 0x00FFFFFF);

  GetByteDataFromLE(&src[6], 36, 4, (U08*)(&data8), sizeof(data8));
  GetByteDataFromLE(&src[6], 40, 4, (U08*)(&data9), sizeof(data9));

  //strBuffer.Format("$STQ-DR(0x6C,0xA0,t=%d,f=%02X,id=%04X,gx=%d,gy=%d,gz=%d,ax=%d,ay=%d,az=%d,w=%c%d,s=%d", 
  //  timeTag, flags, id, data1 & 0xFFFFFF, data2 & 0xFFFFFF, data3 & 0xFFFFFF, data4 & 0xFFFFFF, 
  //  data5 & 0xFFFFFF, data6 & 0xFFFFFF, ((data7 & 0x800000) == 0) ? 'F' : 'B', data7 & 0x7FFFFF, data8 & 0xFFFFFF);
  strBuffer.Format("$STQ-DR(0x6C,0xA0,T=%d,f=%02X,id=%04X,gx=%d,gy=%d,gz=%d,ax=%d,ay=%d,az=%d,t=%d,w=%c%d,s=%d", 
    timeTag, flags, id, data1, data2, data3, data4, data5, data6, data7,
    ((data8 & 0x800000) == 0) ? 'F' : 'B', data8 & 0x7FFFFF, data9 & 0xFFFFFF);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}
}




