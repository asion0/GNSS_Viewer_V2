#pragma once
// MessageParser.h

enum MessageType {
  MtUnknown = 0,
  MtReadError,
  StqBinary,
  NmeaMessage,
  RtcmMessage,
  UbloxMessage,
  StqSpecialDrBinary,
  AllystarBinary
};

//If the function succeeds, the return value is 1.
//If no data can be read, the return value is 0.
//If the device fatal error can no longer be read, the return value is READ_ERROR ((DWORD)(-1))
typedef DWORD (READONECHAR)(U08* c, int timeout);

class MessageParser
{
public:
	MessageParser();
	~MessageParser();

  void SetReadOneCharCallback(READONECHAR fun) { lpReadOneChar = fun; }
  void SetCancelTransmission(bool* c) { cancelTransmission = c; }
  MessageType GetParsingData(void *buffer, DWORD bufferSize, DWORD* totalSize, DWORD timeout = 2000);

  enum { READERROR = 0xFFFFFFFFU };
protected:
  READONECHAR* lpReadOneChar;
  bool* cancelTransmission;
  ULONGLONG readCount;

};