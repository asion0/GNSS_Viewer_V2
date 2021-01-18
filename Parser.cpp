#include "StdAfx.h"
#include "Parser.h"
#include "GnssStatus.h"

CParser::CParser(void)
{
  ClearStat(false);
  gnssStatus = new CGnssStatus();
  stqParser = new CStqMessageParser();
}

CParser::~CParser(void)
{
  SafelyDelPtr(stqParser);
  SafelyDelPtr(gnssStatus);
}

void CParser::ClearStat(bool output)
{
  if(output)
  {
    outputBuffer = parsingBuffer + messageStart;
    outputSize = parsingPtr - messageStart;
  }
  psStat = NoComing;
  rtcmType = 0;
  ubloxClass = 0;
  ubloxId = 0;
  ubloxCkA = 0;
  ubloxCkB = 0;
  messageSize = 0;
  messageLength = 0;
  messageStart = 0;
  parsingPtr = 0;
}



CParser::MessageType CParser::PushParsingData(U08 cData)
{	
  MessageType type = MtParsingIsNotComplete;

  parsingBuffer[parsingPtr] = cData;
  ASSERT(parsingPtr < ParsingBufferSize);

  while(1)
  {    
    if(NoComing == psStat)
    {
      psStat = CheckHeader(cData);
      if(psStat != NoComing)
      {
        messageStart = parsingPtr;
      }
    }
    else if(StqHeaderA0 == psStat)
    {
      switch(cData)
      {
      case 0xA1:
        psStat = StqHeaderA1;
        break;
      default:
        psStat = CheckHeader(cData);
        if(psStat != NoComing)
        {
          messageStart = parsingPtr;
          break;
        }
        psStat = ParsingError;
      }
    }
    else if(StqHeaderA1 == psStat)
    {
      if(cData >= 0x10)
      { //Oversized
        psStat = CheckHeader(cData);
        if(psStat != NoComing)
        {
          messageStart = parsingPtr;
          break;
        }
        psStat = ParsingError;
      }
      else
      {
        messageSize = cData << 8;
        psStat = StqHeaderS1;
      }
    }
    else if(StqHeaderS1 == psStat)
    {
        messageSize |= cData;
        messageLength = 0;
        psStat = StqHeaderS2;
    }
    else if(StqHeaderS2 == psStat)
    {
      if(cData == 0x0D && messageSize + 1 == messageLength)
      {
        psStat = StqEol0D;
      }
      else if(messageLength <= messageSize)
      {
        ++messageLength;
      }
      else
      {
        psStat = ParsingError;
        ASSERT(FALSE);
        break;
      }
    }   
    else if(StqEol0D == psStat)
    {
      if(cData == 0x0A)
      {
        type = StqBinary;
        psStat = ParsingDone;
      }
      else
      {
        psStat = ParsingError;
        ASSERT(FALSE);
			  break;
      }
    } 
    else if(NmeaHeaderDoller == psStat) 
    {
      if(IsPrintable(cData))
      {
        psStat = NmeaBody;
        messageLength = 2;
      }
      else
      {
        psStat = CheckHeader(cData);
        if(psStat != NoComing)
        {
          messageStart = parsingPtr;
          break;
        }
        psStat = ParsingError;
      }
    }
    else if(NmeaBody == psStat) 
    {
      if(0x0D == cData)
      {
        //&& messageLength > 5)
        psStat = NmeaEol0D;
      }
      else if(IsPrintable(cData) && messageLength < 256 )
      {
        ++messageLength;
      }
      else
      {
        psStat = ParsingError;
        ASSERT(FALSE);
			  break;
      }
    }
    else if(NmeaEol0D == psStat) 
    {
      if(cData == 0x0A)
      {
        type = (messageLength < 7) ? MtUnknown : NmeaMessage;
        psStat = ParsingDone;
      }
      else
      {
        psStat = ParsingError;
        ASSERT(FALSE);
			  break;
      }
    }
    else if(RtcmHeaderD3 == psStat) 
    {
      if((0xFC & cData) == 0)
      {
        messageSize  = cData << 8;
        psStat = RtcmHeaderS1;
      }
      else
      {
        psStat = CheckHeader(cData);
        if(psStat != NoComing)
        {
          messageStart = parsingPtr;
          break;
        }
        psStat = ParsingError;
      }
    } 
    else if(RtcmHeaderS1 == psStat) 
    {
        messageSize |= cData;
        if(messageSize > 7) //minimum rtcm message size
        {
          messageLength = 0;
          psStat = RtcmHeaderS2;
        }
        else
        {
          psStat = CheckHeader(cData);
          if(psStat != NoComing)
          {
            messageStart = parsingPtr;
            break;
          }
          psStat = ParsingError;
        }
    }
    else if(RtcmHeaderS2 == psStat) 
    {
      rtcmType = cData << 4;
      ++messageLength;
      psStat = RtcmBodyType1;
    }
    else if(RtcmBodyType1 == psStat) 
    {
      rtcmType |= cData >> 4;
      ++messageLength;
      psStat = RtcmBody;
    }
    else if(RtcmBody == psStat) 
    {
      ++messageLength;
      if(messageSize + 3 == messageLength)  //Include 3 bytes crc
      {
        type = RtcmMessage;
        psStat = ParsingDone;
      }
    }
    else if(UbloxHeaderB5 == psStat) 
    {
      switch(cData)
      {
      case 0x62:
        psStat = UbloxHeader62;
        break;
      default:
        psStat = CheckHeader(cData);
        if(psStat != NoComing)
        {
          messageStart = parsingPtr;
          break;
        }
        psStat = ParsingError;
        break;
      }
    }   
    else if(UbloxHeader62 == psStat)
    {
      if(!CheckUbloxClass(cData))
      { //Oversized
        psStat = CheckHeader(cData);
        if(psStat != NoComing)
        {
          messageStart = parsingPtr;
          break;
        }
        psStat = ParsingError;
      }
      else
      {
        ubloxClass = cData;
        ubloxCkA = cData;
        ubloxCkB = ubloxCkA;
        psStat = UbloxClass;
      }
    }
    else if(UbloxClass == psStat)
    {
        ubloxId = cData;
        ubloxCkA += cData;
        ubloxCkB += ubloxCkA;

        psStat = UbloxId;
    }
    else if(UbloxId == psStat)
    {
        messageSize = cData;
        ubloxCkA += cData;
        ubloxCkB += ubloxCkA;
        psStat = UbloxSize1;
    }
    else if(UbloxSize1 == psStat)
    {
        messageSize |= cData << 8;
        ubloxCkA += cData;
        ubloxCkB += ubloxCkA;
        messageLength = 0;
        psStat = UbloxSize2;
    }
    else if(UbloxSize2 == psStat)
    {
        ++messageLength;
        ubloxCkA += cData;
        ubloxCkB += ubloxCkA;
        psStat = UbloxBody;
    }
    else if(UbloxBody == psStat)
    {
      ++messageLength;
      ubloxCkA += cData;
      ubloxCkB += ubloxCkA;

      if(messageSize == messageLength)  
      {
        psStat = UbloxChecksumA;
      }
    }
    else if(UbloxChecksumA == psStat)
    {
      if(cData != ubloxCkA)
      {
        psStat = ParsingError;
      }
      else
      {
        psStat = UbloxChecksumB;
      }
    }
    else if(UbloxChecksumB == psStat)
    {
      if(cData != ubloxCkB)
      {
        psStat = ParsingError;
      }
      else
      {
        type = UbloxMessage;
        psStat = ParsingDone;
      }
    }
    else if(UnknownMessage0D == psStat)
    {
      if(cData == 0x0A)
      {
        type = MtUnknown;
        psStat = ParsingDone;
      }
      else
      {
        psStat = CheckHeader(cData);
        if(psStat != NoComing)
        {
          messageStart = parsingPtr;
          break;
        }
        psStat = ParsingError;
      }
    }

    ++parsingPtr;
    if(psStat == ParsingDone)
		{
      parsingBuffer[parsingPtr] = 0;
      ASSERT(type != MtParsingIsNotComplete);
      if(type != MtUnknown)
      {
        //type = ParseMessage(parsingBuffer + messageStart, parsingPtr - messageStart, type);
      }
      ClearStat(true);
      return type;
		}
    else if(psStat == ParsingError)
    {
      break;
    }

    if(parsingPtr == ParsingBufferSize - 1)
    {
      return type;
    }
    break;
	} //while(1)
  if(psStat == ParsingError)
  { //Error format
    CString txt;
    txt.Format("E:%d,%02X %02X %02X %02X %02X %02X %02X %02X\r\n", psStat, parsingBuffer[0], parsingBuffer[1], 
      parsingBuffer[2], parsingBuffer[3], parsingBuffer[4], parsingBuffer[5], parsingBuffer[6], parsingBuffer[7]);
    Utility::Log2(__FILE__, __LINE__, txt);
    ClearStat(true);
    return type;
  }

  if(psStat != ParsingDone && type != MtParsingIsNotComplete)
	{
		parsingBuffer[parsingPtr] = 0;
  }

	return type;
//#endif
}

CParser::ParsingState CParser::CheckHeader(U08 cData)
{
  ParsingState psStat = NoComing;
  switch(cData)
  {
  case 0xA0:
    psStat = StqHeaderA0;
    break;
  case 0xD3:
    psStat = RtcmHeaderD3;
    break;
  case '$':
    psStat = NmeaHeaderDoller;
    break;
  case 0xB5:
    psStat = UbloxHeaderB5;
    break;
  case 0x0D:
    psStat = UnknownMessage0D;
    break;
  }
  return psStat;  
}


bool CParser::CheckUbloxClass(U08 cData)
{
  switch(cData)
  {
  case 0x01:
  case 0x02:
  case 0x04:
  case 0x05:
  case 0x06:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0D:
  case 0x10:
  case 0x13:
  case 0x21:
  case 0x27:
    return true;
  default:
    return false;
  }
}

CMessageParser::ParsingResult CParser::ParsingMessage(MessageType type, const U08* buffer, int size)
{
  CString txt;
  txt.Format("P:%d,%d,%02X %02X %02X %02X %02X %02X %02X ... %02X %02X %02X\r\n", type, size, 
    buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], 
    buffer[size - 3], buffer[size - 2], buffer[size - 1]);
  Utility::Log2(__FILE__, __LINE__, txt);

  CMessageParser::ParsingResult res = CMessageParser::StqUnknown;
  switch (type)
  {
  case StqBinary:
    res = stqParser->ParseOneMessage(buffer, size);
    break;
  case NmeaMessage:
    break;
  case RtcmMessage:
    break;
  case UbloxMessage:
    break;
  }

  return res;
}

const CGnssStatus* CParser::GetGnssStatus(MessageType type)
{
  switch (type)
  {
  case StqBinary:
    return stqParser->GetGnssStatus();
  case NmeaMessage:
    break;
  case RtcmMessage:
    break;
  case UbloxMessage:
    break;
  }
    return NULL;
}

const CStringArray* CParser::GetOutputTxt(MessageType type)
{
  switch (type)
  {
  case StqBinary:
    return stqParser->GetOutputTxt();
  case NmeaMessage:
    break;
  case RtcmMessage:
    break;
  case UbloxMessage:
    break;
  }
    return NULL;
}
//CMessageParser
CMessageParser::ParsingResult CStqMessageParser::ParseOneMessage(const U08* buffer, int size)
{
  //Check valid
  U08 id = CalcStqMsgChecksum(buffer);
  if(id == 0)
  { //Checksum error
    return StqCheckSumError;
  }
  
  updateFlag = UpdateNone;
  outputTxt.RemoveAll();
  switch(id)
  {
    case 0xE5:  // EXT_RAW_MEAS Extended Raw Measurement Data v.1 (0xE5) (Periodic)
		  updateFlag = Parse0xE5ExtRawMeas(buffer);
      return StqE5Message;
  }
  return StqCheckSumError;
}


CString CMessageParser::GetGnssTypeSignalTypeString(U08 typeSig)
{
  CString txt;
  switch(typeSig) 
  {
  case 0x00:
    txt = "GPS-L1 C/A";
    break;
  case 0x10:
    txt = "GPS-L1C";
    break;
  case 0x20:
    txt = "GPS-L2C";
    break;
  case 0x40:
    txt = "GPS-L5";
    break;
  case 0x01:
    txt = "SBAS-L1";
    break;
  case 0x02:
    txt = "GLONASS-L1";
    break;
  case 0x22:
    txt = "GLONASS-L2";
    break;
  case 0x42:
    txt = "GLONASS-L3";
    break;
  case 0x03:
    txt = "Galileo-E1";
    break;
  case 0x43:
    txt = "Galileo-E5a";
    break;
  case 0x53:
    txt = "Galileo-E5b";
    break;
  case 0x63:
    txt = "Galileo-E6";
    break;
  case 0x04:
    txt = "QZSS-L1 C/A";
    break;
  case 0x14:
    txt = "QZSS-L1C";
    break;
  case 0x24:
    txt = "QZSS-L2C";
    break;
  case 0x44:
    txt = "QZSS-L5";
    break;
  case 0x64:
    txt = "QZSS-LEX";
    break;
  case 0x05:
    txt = "BeiDou-B1I";
    break;
  case 0x15:
    txt = "BeiDou-B1C";
    break;
  case 0x45:
    txt = "BeiDou-B2A";
    break;
  case 0x55:
    txt = "BeiDou-B2I";
    break;
  case 0x75:
    txt = "BeiDou-B3I";
    break;
  case 0x46:
    txt = "NavIC-L5";
    break;
  default:
    txt.Format("Unknown(%02X)", typeSig);
    break;
  }
  return txt;
}

U08 CMessageParser::GetBeU08(const U08*& ptr)
{
	return *ptr++;
}

U16 CMessageParser::GetBeU16(const U08*& ptr)
{
  U16 d = MAKEWORD(ptr[1], ptr[0]);
  ptr += sizeof(d);
	return d;
}

U32 CMessageParser::GetBeU32(const U08*& ptr)
{
  U32 d = MAKELONG(MAKEWORD(ptr[3], ptr[2]), MAKEWORD(ptr[1], ptr[0]));
  ptr += sizeof(d);
	return d;
}

F32 CMessageParser::GetBeF32(const U08*& ptr)
{
  U08 d[4];
  d[3] = *ptr++;
  d[2] = *ptr++;
  d[1] = *ptr++;
  d[0] = *ptr++;
	return *((F32*)d);
}

D64 CMessageParser::GetBeD64(const U08*& ptr)
{
  U08 d[8];
  d[7] = *ptr++;
  d[6] = *ptr++;
  d[5] = *ptr++;
  d[4] = *ptr++;
  d[3] = *ptr++;
  d[2] = *ptr++;
  d[1] = *ptr++;
  d[0] = *ptr++;
	return *((D64*)d);
}


CStqMessageParser::CStqMessageParser(void) 
{ 

}

CStqMessageParser::~CStqMessageParser(void) 
{ 

}

UpdateFlag CStqMessageParser::Parse0xE5ExtRawMeas(const U08* src)
{
  CString txt;
  UpdateFlag f = UpdateNone;
	const U08* ptr = &src[5];

  U08 ver = GetBeU08(ptr);
  U08 iod = GetBeU08(ptr);
  U16 weeks = GetBeU16(ptr);
  U32 tow = GetBeU32(ptr);
  U16 measPeriod = GetBeU16(ptr);
  U08 measIndFlag = GetBeU08(ptr);
  U08 reserve = GetBeU08(ptr);
  U08 nmeas = GetBeU08(ptr);

  if(generateTxt)
  {
    txt.Format("$EXT_RAW_MEAS(0xE5),V%d,IOD=%d,WN=%d,TOW=%d,MeasPeriod=%d,MeasIndicator=0x%02X,NMEAS=%d",
      ver, iod, weeks, tow, measPeriod, measIndFlag, nmeas);
    outputTxt.Add(txt);
  }

  if(nmeas > 0 && generateTxt)
  {
    txt.Format("Gnss Signal|SVID|FrqID|LTInd|CN0|   PseudoRange|       AccCrrCyc|DopFreq|PRSdDv|ACCSdDv|DFSdDv| ChInd|");
    outputTxt.Add(txt);
    txt.Format("-----------+----+-----+-----+---+--------------+----------------+-------+------+-------+------+------+");
    outputTxt.Add(txt);
  }

  U16 numCycleSlippedTotal = 0;
  for (int i = 0; i < nmeas; ++i)
  {
	  U08 typeNsingel = GetBeU08(ptr);
	  U08 svid = GetBeU08(ptr);
	  U08 freqIdNlockTimeInd = GetBeU08(ptr);
	  U08 cn0 = GetBeU08(ptr);
    D64 pseduRange = GetBeD64(ptr);
    D64 accCarrierCycle = GetBeD64(ptr);
	  F32 dopplerFreq = GetBeF32(ptr);
	  U08 prStdDeviation = GetBeU08(ptr);
	  U08 accStdDeviation = GetBeU08(ptr);
	  U08 dfDeviation = GetBeU08(ptr);
	  U16 chInd = GetBeU16(ptr);
    U08 reserve1 = GetBeU08(ptr);
    U08 reserve2 = GetBeU08(ptr);
 
  //strBuffer.Format("Gnss Signal|SVID|FrqID|LTInd|CN0|   PseudoRange|       AccCrrCyc|DopFreq|PRSdDv|ACCSdDv|DFSdDv| ChInd|");
  //strBuffer.Format("-----------+----+-----+-----+---+--------------+----------------+-------+------+-------+------+------+");
    txt.Format("%11s| %3d|%5d|%5d|%3d| %13.3lf| % 15.3lf| % 6.0f|%6d|%7d|%6d|0x%04X|",
        GetGnssTypeSignalTypeString(typeNsingel), svid, freqIdNlockTimeInd & 0x0f, freqIdNlockTimeInd >> 4, 
        cn0, pseduRange, accCarrierCycle, dopplerFreq, 
        prStdDeviation, accStdDeviation, dfDeviation, chInd);
    outputTxt.Add(txt);

    if(chInd & 0x08)
    {
      ++numCycleSlippedTotal;
    }
  } //for (int i = 0; i < nmeas; ++i)
  f |= gnssStatus->SetDateTimeViaWnTow(weeks, tow);
	f |= gnssStatus->SetBaseTotalCycleSlipped(numCycleSlippedTotal);
	return f;
}



