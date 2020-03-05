// MessageParser.cpp

#include "stdafx.h"
#include "MessageParser.h"



MessageParser::MessageParser()
{
  lpReadOneChar = NULL;
  cancelTransmission = NULL;
  readCount = 0;
} 

MessageParser::~MessageParser()
{
}

enum ParsingState {
  NoComing,
  StqHeaderA0,
  StqHeaderA1,
  StqDrSpecialHeader6C,
  StqDrSpecialHeaderA0,
  StqSpecialDrSize1,
  StqSpecialDrSize2,
  StqSpecialDrBody,
  StqSpecialDrChecksum,
  StqSpecialDrEol0D,
  StqSpecialDrEol0A,
  StqHeaderS1,
  StqHeaderS2,
  StqEol0D,
  NmeaHeaderDoller,
  NmeaBody,
  NmeaEol0D,
  RtcmHeaderD3,
  RtcmHeaderS1,
  RtcmHeaderS2,
  RtcmBodyType1,
  RtcmBody,
  UbloxHeaderB5,
  UbloxHeader62,
  UbloxClass,
  UbloxId,
  UbloxSize1,
  UbloxSize2,
  UbloxBody,
  UbloxChecksumA,
  UbloxChecksumB,

  UnknownMessage0D,
  UnknownMessage0A,
  HostLogHeaderS,
  HostLogHeaderT,

  ParsingDone,
};

static ParsingState CheckHeader(U08 c)
{
  ParsingState ps = NoComing;
  switch(c)
  {
  case 0xA0:
    ps = StqHeaderA0;
    break;
  case 0xD3:
    ps = RtcmHeaderD3;
    break;
  case '$':
    ps = NmeaHeaderDoller;
    break;
  case 0xB5:
    ps = UbloxHeaderB5;
    break;
  case 0x0D:
    ps = UnknownMessage0D;
    break;
  }
  return ps;  
}

BOOL IsSupportedRtcmType(U16 rtcmType)
{
  switch(rtcmType)
  {
  case 1005:
  case 1077:
  case 1087:
  case 1107:
  case 1117:
  case 1127:
    return TRUE;
  default:
    return FALSE;
  }
  return FALSE;
}

BOOL CheckUbloxClass(U08 c)
{
  switch(c)
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
    return TRUE;
  default:
    return FALSE;
  }
}

MessageType MessageParser::GetParsingData(void *buffer, DWORD bufferSize, DWORD* totalSize, DWORD timeout)
{	
#ifdef _DEBUG
  const int dbgMode = 0;
  timeout = 300000;
#else
  const int dbgMode = 0;
#endif
	ScopeTimer t;
	U08* bufferIter = (U08*)buffer;
  U16 rtcmType = 0;
  U08 ubloxClass = 0;
  U08 ubloxId = 0;
  U08 ubloxCkA = 0;
  U08 ubloxCkB = 0;
  U32 messageSize = 0;
  U32 messageLength = 0;
  ParsingState ps = NoComing;
  MessageType type = MtUnknown;
  DWORD messageStart = 0;
  *totalSize = 0;
  int emptyCount = 0;
	while(!*cancelTransmission)
	{ 
		if(t.GetDuration() > timeout && dbgMode == 0)
		{
		  return type;
		}

    DWORD b = lpReadOneChar(bufferIter, 10);
    if(MessageParser::READERROR == b)
    { //COM error
      type = MtReadError;
      break;
    }
		if(0 == b)
		{	//No data
			Sleep(2);
      if(++emptyCount > 10)
      {
		    return type;
      }
      continue;
		}
    emptyCount = 0;
    ++readCount;
    ++*totalSize;
    if(NoComing == ps)
    {
      ps = CheckHeader(*bufferIter);
      if(ps != NoComing)
      {
        messageStart = *totalSize - 1;
      }
    }
    else if(StqHeaderA0 == ps)
    {
      switch(*bufferIter)
      {
      case 0xA1:
        ps = StqHeaderA1;
        break;
      default:
        ps = CheckHeader(*bufferIter);
        if(ps != NoComing)
        {
          messageStart = *totalSize - 1;
        }
        break;
      }
    }
    else if(StqHeaderA1 == ps)
    {
      if(*bufferIter == 0x6C)
      {
        ps = StqDrSpecialHeader6C;
      }
      else if(*bufferIter >= 0x10)
      { //Oversized
        ps = CheckHeader(*bufferIter);
        if(ps != NoComing)
        {
          messageStart = *totalSize - 1;
        }
      }
      else
      {
        messageSize = *bufferIter << 8;
        ps = StqHeaderS1;
      }
    }
    else if(StqDrSpecialHeader6C == ps)
    {
      switch(*bufferIter)
      {
      case 0xA0:
        ps = StqDrSpecialHeaderA0;
        break;
      default:
        ps = CheckHeader(*bufferIter);
        if(ps != NoComing)
        {
          messageStart = *totalSize - 1;
        }
        break;
      }
    }
    else if(StqDrSpecialHeaderA0 == ps)
    {
      if(*bufferIter != 0x30)
      { //Oversized
        ps = CheckHeader(*bufferIter);
        if(ps != NoComing)
        {
          messageStart = *totalSize - 1;
        }
      }
      else
      {
        messageSize = *bufferIter;
        ubloxCkA = 0x6C ^ 0xA0 ^ *bufferIter;
        ps = StqSpecialDrSize1;
      }
    }
    else if(StqSpecialDrSize1 == ps)
    {
        messageSize |= *bufferIter << 8;
        ubloxCkA ^= *bufferIter;
        messageLength = 0;
        ps = StqSpecialDrSize2;
    }
    else if(StqSpecialDrSize2 == ps)
    {
        ++messageLength;
        ubloxCkA ^= *bufferIter;
        ps = StqSpecialDrBody;
    }
    else if(StqSpecialDrBody == ps)
    {
      ++messageLength;
      ubloxCkA ^= *bufferIter;
      if(messageSize == messageLength)  
      {
        ps = StqSpecialDrChecksum;
      }
    }
    else if(StqSpecialDrChecksum == ps)
    {
      if(*bufferIter != ubloxCkA)
      {
        ps = NoComing;
      }
      else
      {
        ps = StqSpecialDrEol0D;
      }
    }
    else if(StqSpecialDrEol0D == ps)
    {
      if(*bufferIter == 0x0D)
      {
        ps = StqSpecialDrEol0A;
      }
      else
      {
        ps = NoComing;
      }
    } 
    else if(StqSpecialDrEol0A == ps)
    {
      if(*bufferIter == 0x0A)
      {
        type = StqSpecialDrBinary;
        ps = ParsingDone;
      }
      else
      {
        ps = NoComing;
      }
    }     
    else if(StqHeaderS1 == ps)
    {
        messageSize |= *bufferIter;
        messageLength = 0;
        ps = StqHeaderS2;
    }
    else if(StqHeaderS2 == ps)
    {
      if(*bufferIter == 0x0D && messageSize + 1 == messageLength)
      {
        ps = StqEol0D;
      }
      else if(messageLength <= messageSize)
      {
        ++messageLength;
      }
      else
      {
        break;
      }
    }   
    else if(StqEol0D == ps)
    {
      if(*bufferIter == 0x0A)
      {
        type = StqBinary;
        ps = ParsingDone;
      }
      else
      {
        break;
      }
    } 
    else if(NmeaHeaderDoller == ps) 
    {
      if(IsPrintable(*bufferIter))
      {
        ps = NmeaBody;
        messageLength = 2;
      }
      else
      {
        ps = CheckHeader(*bufferIter);
        if(ps != NoComing)
        {
          messageStart = *totalSize - 1;
        }
      }
    }
    else if(NmeaBody == ps) 
    {
      if(0x0D == *bufferIter)
      {
        //&& messageLength > 5)
        ps = NmeaEol0D;
      }
      else if(IsPrintable(*bufferIter) && messageLength < 128)
      {
        ++messageLength;
      }
      else
      {
        ++bufferIter;
        break;
      }
    }
    else if(NmeaEol0D == ps) 
    {
      if(*bufferIter == 0x0A)
      {
        type = (messageLength < 7) ? MtUnknown : NmeaMessage;
        ps = ParsingDone;
      }
      else
      {
        break;
      }
    }
    else if(RtcmHeaderD3 == ps) 
    {
      if((0xFC & *bufferIter) == 0)
      {
        messageSize  = *bufferIter << 8;
        ps = RtcmHeaderS1;
      }
      else
      {
        ps = CheckHeader(*bufferIter);
        if(ps != NoComing)
        {
          messageStart = *totalSize - 1;
        }
      }
    } 
    else if(RtcmHeaderS1 == ps) 
    {
        messageSize |= *bufferIter;
        if(messageSize > 7) //minimum rtcm message size
        {
          messageLength = 0;
          ps = RtcmHeaderS2;
        }
        else
        {
          ps = CheckHeader(*bufferIter);
          if(ps != NoComing)
          {
            messageStart = *totalSize - 1;
          }
        }
    }
    else if(RtcmHeaderS2 == ps) 
    {
      rtcmType = *bufferIter << 4;
      ++messageLength;
      ps = RtcmBodyType1;
    }
    else if(RtcmBodyType1 == ps) 
    {
      rtcmType |= *bufferIter >> 4;
      ++messageLength;
      if(IsSupportedRtcmType(rtcmType))
      {
        ps = RtcmBody;
      }
      else
      {
        break;
      }
    }
    else if(RtcmBody == ps) 
    {
      ++messageLength;
      if(messageSize + 3 == messageLength)  //Include 3 bytes crc
      {
        type = RtcmMessage;
        ps = ParsingDone;
      }
    }
    else if(UbloxHeaderB5 == ps) 
    {
      switch(*bufferIter)
      {
      case 0x62:
        ps = UbloxHeader62;
        break;
      default:
        ps = CheckHeader(*bufferIter);
        if(ps != NoComing)
        {
          messageStart = *totalSize - 1;
        }
        break;
      }
    }   
    else if(UbloxHeader62 == ps)
    {
      if(!CheckUbloxClass(*bufferIter))
      { //Oversized
        ps = CheckHeader(*bufferIter);
        if(ps != NoComing)
        {
          messageStart = *totalSize - 1;
        }
      }
      else
      {
        ubloxClass = *bufferIter;
        ubloxCkA = *bufferIter;
        ubloxCkB = ubloxCkA;
        ps = UbloxClass;
      }
    }
    else if(UbloxClass == ps)
    {
        ubloxId = *bufferIter;
        ubloxCkA += *bufferIter;
        ubloxCkB += ubloxCkA;

        ps = UbloxId;
    }
    else if(UbloxId == ps)
    {
        messageSize = *bufferIter;
        ubloxCkA += *bufferIter;
        ubloxCkB += ubloxCkA;
        ps = UbloxSize1;
    }
    else if(UbloxSize1 == ps)
    {
        messageSize |= *bufferIter << 8;
        ubloxCkA += *bufferIter;
        ubloxCkB += ubloxCkA;
        messageLength = 0;
        ps = UbloxSize2;
    }
    else if(UbloxSize2 == ps)
    {
        ++messageLength;
        ubloxCkA += *bufferIter;
        ubloxCkB += ubloxCkA;
        ps = UbloxBody;
    }
    else if(UbloxBody == ps)
    {
      ++messageLength;
      ubloxCkA += *bufferIter;
      ubloxCkB += ubloxCkA;

      if(messageSize == messageLength)  
      {
        ps = UbloxChecksumA;
      }
    }
    else if(UbloxChecksumA == ps)
    {
      if(*bufferIter != ubloxCkA)
      {
        ps = NoComing;
      }
      else
      {
        ps = UbloxChecksumB;
      }
    }
    else if(UbloxChecksumB == ps)
    {
      if(*bufferIter != ubloxCkB)
      {
        ps = NoComing;
      }
      else
      {
        type = UbloxMessage;
        ps = ParsingDone;
      }
    }
    else if(UnknownMessage0D == ps)
    {
      if(*bufferIter == 0x0A)
      {
        type = MtUnknown;
        ps = ParsingDone;
      }
      else
      {
        ps = CheckHeader(*bufferIter);
        if(ps != NoComing)
        {
          messageStart = *totalSize - 1;
        }
      }
    }

    ++bufferIter;
    if(*totalSize == bufferSize - 1 || ps == ParsingDone)
		{
			*bufferIter = 0;
			break;
		}
	} //while(!m_cancelTransmission)
  if(ps != ParsingDone)
	{
		*bufferIter = 0;
  }

	if(*cancelTransmission)
	{
		*cancelTransmission = false;
    *totalSize = MessageParser::READERROR;
	}
	return type;
}
