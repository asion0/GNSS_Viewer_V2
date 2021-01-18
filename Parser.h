#pragma once

#include "GnssStatus.h"

class CMessageParser
{
public:
  enum ParsingResult
  {
    StqUnknown,
    StqCheckSumError,
    StqE5Message,
  };

  CMessageParser(void) { generateTxt = true; gnssStatus = new CGnssStatus(); };
  virtual ~CMessageParser(void) { };
  
  virtual ParsingResult ParseOneMessage(const U08* buffer, int size) = 0;

  UpdateFlag GetUpdateFlag() { return updateFlag; }
  const CGnssStatus* GetGnssStatus() { return gnssStatus; }; 
  const CStringArray* GetOutputTxt() { return &outputTxt; }; 

protected:
  CStringArray outputTxt;
  CGnssStatus* gnssStatus; 
  bool generateTxt;
  UpdateFlag updateFlag;

  static CString GetGnssTypeSignalTypeString(U08 typeSig);
  static U08 GetBeU08(const U08*& ptr);
  static U16 GetBeU16(const U08*& ptr);
  static U32 GetBeU32(const U08*& ptr);
  static F32 GetBeF32(const U08*& ptr);
  static D64 GetBeD64(const U08*& ptr);
};

class CStqMessageParser: public CMessageParser
{
public:
  CStqMessageParser(void);
  virtual ~CStqMessageParser(void);

  virtual ParsingResult ParseOneMessage(const U08* buffer, int size);
protected:
  UpdateFlag Parse0xE5ExtRawMeas(const U08* src);

};


class CParser
{
public:
  enum MessageType {
    MtUnknown = 0,
    MtParsingIsNotComplete,
    MtReadError,

    StqBinary,
    NmeaMessage,
    RtcmMessage,
    UbloxMessage,
  };
  CParser(void);
  virtual ~CParser(void);

  MessageType CParser::PushParsingData(U08 cData);
  const U08* GetOutputMessage() const { return outputBuffer; }
  int GetOutputMessageSize() const { return outputSize; }
  CMessageParser::ParsingResult ParsingMessage(MessageType type, const U08* buffer, int size);
  const CGnssStatus* GetGnssStatus(MessageType type);
  const CStringArray* GetOutputTxt(MessageType type);
protected:
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
    StqSpecialDrEol0A,  //10
    StqHeaderS1,
    StqHeaderS2,
    StqEol0D,
    NmeaHeaderDoller,
    NmeaBody,
    NmeaEol0D,
    RtcmHeaderD3,
    RtcmHeaderS1,
    RtcmHeaderS2,
    RtcmBodyType1,    //20
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

    ParsingError,
    ParsingDone,
  };
  ParsingState psStat;
  U16 rtcmType;
  U08 ubloxClass;
  U08 ubloxId;
  U08 ubloxCkA;
  U08 ubloxCkB;
  U32 messageSize;
  U32 messageLength;
  int messageStart;
  enum { ParsingBufferSize = 8 * 1024 };
  U08 parsingBuffer[ParsingBufferSize];
  int parsingPtr;
  CGnssStatus* gnssStatus; 
  CStqMessageParser* stqParser;

  U08* outputBuffer;
  int outputSize;

  static ParsingState CheckHeader(U08 cData);
  static bool CheckUbloxClass(U08 cData);

  void ClearStat(bool output);

  //Message Parser
  //static bool ParseStqBinary(CGnssStatus* gnss, const U08* buffer, int size);
};