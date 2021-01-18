#pragma once
#include "Parser.h"
class GnssBuffer;

typedef void (*ReadProgress)(int pos, int total);
typedef void (*ParsingProgress)(int pos, int total);
typedef void (*LoggerProgress)(int pos, int total);
typedef void (*WriteProgress)(int pos, int total);
typedef void (*ParserNotify)(CParser::MessageType type, 
  CMessageParser::ParsingResult res, const CGnssStatus* gnssStatus, const CStringArray* txtArray);

class CReader
{
public:
  CReader(void);
  virtual ~CReader(void);

  virtual void Close() = 0;
  virtual void StartReadProcess(bool start) = 0;
  virtual void StartParsingProcess(bool start) = 0;
  virtual void StartLoggerProcess(bool start) = 0;

  BOOL IsOpened() { return m_isOpened; }
  BOOL IsReadProcessRunning() { return !cancelReadProcess; }
  BOOL IsParsingProcessRunning() { return !cancelParsingProcess; }
  BOOL IsLoggerProcessRunning() { return !cancelLoggerProcess; }

  void SetReadProgress(ReadProgress p) { pReadProg = p; }
  void SetParsingProgress(ReadProgress p) { pParsingProg = p; }
  void SetLoggerProgress(ReadProgress p) { pLoggerProg = p; }
  void SetWriteProgress(ReadProgress p) { pWriteProg = p; }
  void SetParserNotify(ParserNotify p) { pParserNotify = p; }

protected:
	bool m_isOpened;
  ReadProgress pReadProg;
  ParsingProgress pParsingProg;
  LoggerProgress pLoggerProg;
  WriteProgress pWriteProg;
  ParserNotify pParserNotify;

  CWinThread* pReadProcess;
  CWinThread* pParsingProcess;
  CWinThread* pLoggerProcess;

  bool cancelReadProcess;
  bool cancelParsingProcess;
  bool cancelLoggerProcess;

  GnssBuffer* pGnssBuffer;
  GnssBuffer* pLoggerBuffer;
  CParser* pGnssParser;

  //GnssBuffer* pGnssBuffer;

  bool GetCancelReadProcess() { return cancelReadProcess; }
  bool GetCancelParsingProcess() { return cancelParsingProcess; }
  bool GetCancelLoggerProcess() { return cancelLoggerProcess; }
};

//class CMySocket;
class CComReader : public CReader
{
public:
  CComReader(void);
  virtual ~CComReader(void);

  static int GetBaudrateIndex(int baudRate);
  static int GetBaudrateByIndex(int index);
  static UINT ReadProcessThread(LPVOID pParam);
  static UINT ParsingProcessThread(LPVOID pParam);
  static UINT LoggerProcessThread(LPVOID pParam);

  bool OpenByBaudrateIndex(int port, int baudIndex);
  bool OpenByBaudrateIndex(LPCSTR comPort, int baudIndex);
	virtual void Close();
  virtual void StartReadProcess(bool start);
  virtual void StartParsingProcess(bool start);
  virtual void StartLoggerProcess(bool start);

  //Getter and Setter
  //bool OpenTcp(int type, LPCSTR host, int port);
  //bool OpenTcp(CMySocket* soc);

	bool ResetPort(int baudIndex);
	void ClearQueue();

	DWORD ReadData(void* buffer, DWORD bufferSize, bool once = false);
	DWORD GetString(void* buffer, DWORD bufferSize, DWORD timeOut);
  DWORD GetOneLine(void* buffer, DWORD bufferSize, DWORD timeOut);
  BOOL GetOneChar(U08 *c, DWORD* dwBytesDoRead, DWORD timeout);
	DWORD GetBinary(void* buffer, DWORD bufferSize, DWORD timeout = 2000);
	DWORD GetBinaryAck(void* buffer, DWORD bufferSize, DWORD timeout = 2000);
	DWORD GetBinaryBlock(void* buffer, DWORD bufferSize, DWORD blockSize);
	DWORD SendData(const void* buffer, DWORD bufferSize, bool blockTransfer = false, int delayDuration = 0);
  static DWORD GetComBinaryAck(HANDLE com, void *buffer, DWORD bufferSize, DWORD timeout);

  static DWORD readCount;
	DWORD errorCode;
protected:
	enum { InQueueSize = 1024 * 128 };
	enum { OutQueueSize = 1024 * 128 };
  static int BaudrateTable[];

  HANDLE m_comHandle;
	int m_comPort;
	int m_baudRate;


  bool _Open(LPCSTR portName, int baudrate);
  DWORD _GetComReadableSize();

  void _StartReadProcess();
  void _StopReadProcess();
  void _StartParsingProcess();
  void _StopParsingProcess();
  void _StartLoggerProcess();
  void _StopLoggerProcess();














  //https://zh.wikipedia.org/wiki/%E9%87%8D%E5%8F%A0I/O
	//OVERLAPPED m_OverlappedRead;
	//OVERLAPPED m_OverlappedWrite;
	//DWORD m_sendUnit;
	//DWORD m_logStatus;
	//CMySocket* m_psocket;

  //Done
	int ComInitial();
	bool ResetPortNoDelay(int baud);
  //In progress
	int WaitingDataIn();
	bool WriteCommBytes(char* buffer, int bufferSize);
};

class CFileReader :
  public CReader
{
public:
  CFileReader(void);
  virtual ~CFileReader(void);
};