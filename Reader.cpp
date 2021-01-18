#include "StdAfx.h"
#include "Reader.h"
#include "GnssBuffer.h"
#include "Parser.h"

//CReader
CReader::CReader(void)
{
  pReadProcess = NULL;
  pParsingProcess = NULL;
  pLoggerProcess = NULL;

  pReadProg = NULL;
  pParsingProg = NULL;
  pLoggerProg = NULL;
  pWriteProg = NULL;

  cancelReadProcess = true;
  cancelParsingProcess = true;
  cancelLoggerProcess = true;

  pLoggerBuffer = new GnssBuffer();
  pGnssBuffer = new GnssBuffer();
  pGnssParser = new CParser();
}

CReader::~CReader(void)
{
  SafelyDelPtr(pGnssParser);
  SafelyDelPtr(pLoggerBuffer);
  SafelyDelPtr(pGnssBuffer);
}

//CComReader
int CComReader::BaudrateTable[] = {4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
CComReader::CComReader(void)
  : CReader()
{
  m_comHandle = NULL;
}

CComReader::~CComReader(void)
{
}

bool CComReader::OpenByBaudrateIndex(int port, int baudIndex)
{
  CString portName;
	portName.Format("\\\\.\\COM%d", port);
	return _Open(portName, g_setting.BaudrateValue(baudIndex));
}

bool CComReader::OpenByBaudrateIndex(LPCSTR comPort, int baudIndex)
{
  CString portName;
	portName.Format("\\\\.\\%s", comPort);
	return _Open(portName, g_setting.BaudrateValue(baudIndex));
}

bool CComReader::_Open(LPCSTR portName, int baudrate)
{
	int tryCnt = 10;
  CString txt;
  txt.Format("Open,%s,%d", portName, baudrate);
  Utility::Log2(__FILE__, __LINE__, txt);
	while(TRUE)
	{
		m_comHandle = CreateFile(portName, GENERIC_READ | GENERIC_WRITE,
				0, NULL, OPEN_EXISTING,  0, 0);

		if(m_comHandle == INVALID_HANDLE_VALUE)
		{
      errorCode = ::GetLastError();
      txt.Format("OpenFailed,%d,%d", errorCode, tryCnt);
      Utility::Log2(__FILE__, __LINE__, txt);
			if(--tryCnt == 0)
			{
				return false;
			}
			else
			{
				Sleep(50);
				continue;
			}
		}
		break;
	}
	ComInitial();
  COMMTIMEOUTS timeouts = 
  { 
      0, //interval timeout. 0 = not used
      0, // read multiplier
      5, // read constant (milliseconds)
      0, // Write multiplier
      0  // Write Constant 
  };
  SetCommTimeouts(m_comHandle, &timeouts);
	if(!SetupComm(m_comHandle, InQueueSize, OutQueueSize))
	{
		return false;
	}

	if(!PurgeComm(m_comHandle, 
			PURGE_TXABORT | PURGE_RXABORT |
			PURGE_TXCLEAR |	PURGE_RXCLEAR))
	{
		return false;
	}

  if(!ResetPortNoDelay(baudrate))
	{
		return false;
	}

	//m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  CString p(portName);
  p.Replace("\\\\.\\COM", "");
  m_comPort = atoi(p);
	m_isOpened = true;


	return true;
}

void CComReader::Close()
{
	if(!m_isOpened || m_comHandle == NULL) 
	{
		return;
	}
  if(pReadProcess)
  {
    _StopReadProcess();
  }
  Sleep(500);
  if(pParsingProcess)
  {
    _StopParsingProcess();
  }
  Sleep(2000);
  if(pLoggerProcess)
  {
    _StopLoggerProcess();
  }

  CloseHandle(m_comHandle);
  m_comHandle = NULL;
	m_isOpened = false;

	return;
}

int CComReader::GetBaudrateIndex(int baudRate)
{
  int idx = -1;
	for(int i = 0; i < Dim(BaudrateTable); ++i)
	{
		if(BaudrateTable[i] == baudRate)
		{
			idx = i;
			return idx;
		}
	}
  return idx;
}

int CComReader::GetBaudrateByIndex(int index)
{
  if(index < 0 || index > Dim(BaudrateTable))
  {
    return -1;
  }
  return BaudrateTable[index];
}

int CComReader::ComInitial()
{
	DWORD error = 0;
	COMSTAT comStatus = {0};
	ClearCommError(m_comHandle, &error, &comStatus);
	return comStatus.cbInQue;
}

bool CComReader::ResetPortNoDelay(int b)
{
	DCB dcb = { 0 };
	if(!GetCommState(m_comHandle, &dcb))
	{
		DWORD error = ::GetLastError();
		return false;
	}

	dcb.BaudRate = b;
	dcb.Parity   = NOPARITY;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;

	if(!SetCommState(m_comHandle, &dcb))
	{
		DWORD error = ::GetLastError();
		return false;
	}

	m_baudRate = b;
	return true;
}

UINT CComReader::ReadProcessThread(LPVOID pParam)
{
  static int totalIdle = 0;
  static int totalBusy = 0;
  ScopeTimer st;
  const int BufferSize = 8 * 1024;
  U08 buffer[BufferSize];
  CComReader* pReader = (CComReader*)pParam;
  CString txt;
  Utility::Log2(__FILE__, __LINE__, "ReadProcessThread,Start\r\n");
  if(pReader->pReadProg)
  {
    pReader->pReadProg(0, BufferSize);
  }
  while(!pReader->GetCancelReadProcess())
  {
    DWORD comInQueueSize = pReader->_GetComReadableSize();
    if(comInQueueSize == 0)
    {
      Sleep(10);
      totalIdle += 10;
      continue;
    }

    DWORD dwBytesDoRead = 0;
    DWORD dwBytesToRead = BufferSize;
    DWORD freeSize;
    
    freeSize = pReader->pGnssBuffer->GetWritableSize();
    if(!pReader->cancelParsingProcess &&  freeSize < dwBytesToRead)
    { //
      if(freeSize == 0)
      {
        txt.Format("%s(%d)GnssBuffer full!!!\r\n", __FILE__, __LINE__);
        ::OutputDebugString(txt);
        pReader->pGnssBuffer->ClearBuffer();
        continue;
      }
      dwBytesToRead = freeSize;
    }
    
    freeSize = pReader->pLoggerBuffer->GetWritableSize();
    if(!pReader->cancelLoggerProcess && freeSize < dwBytesToRead)
    { //
      if(freeSize == 0)
      {
        txt.Format("%s(%d)LoggerBuffer full!!!\r\n", __FILE__, __LINE__);
        ::OutputDebugString(txt);
        pReader->pLoggerBuffer->ClearBuffer();
        continue;
      }
      dwBytesToRead = freeSize;
    }
    DWORD d1 = st.GetDuration();
		BOOL bReadStatus = ReadFile(pReader->m_comHandle, buffer, dwBytesToRead, &dwBytesDoRead, 0);
    DWORD d2 = st.GetDuration();

    if(dwBytesDoRead > 0 && !pReader->cancelParsingProcess)
    {
      memcpy(pReader->pGnssBuffer->GetWritePtr(), buffer, dwBytesDoRead);
      pReader->pGnssBuffer->SetWritePtr(dwBytesDoRead);
    }
    if(dwBytesDoRead > 0 && !pReader->cancelLoggerProcess)
    {
      memcpy(pReader->pLoggerBuffer->GetWritePtr(), buffer, dwBytesDoRead);
      pReader->pLoggerBuffer->SetWritePtr(dwBytesDoRead);
    }
    if(pReader->pReadProg)
    {
      pReader->pReadProg(dwBytesDoRead, BufferSize);
    }
  }
  Utility::Log2(__FILE__, __LINE__, "ReadProcessThread,End\r\n");
  return 0;
}

UINT CComReader::ParsingProcessThread(LPVOID pParam)
{
  //static int totalIdle = 0;
  //static int totalBusy = 0;
  ScopeTimer st;
  CComReader* pReader = (CComReader*)pParam;
  GnssBuffer* pBuffer = pReader->pGnssBuffer;
  CParser* pParser = pReader->pGnssParser;
  CString txt;
  //DWORD totalBytes = 0;
  //int count = 0;
  
  

  { //Test
    CFile file;
    file.Open("456.bin", CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite);
    file.Close();
  }

  Utility::Log2(__FILE__, __LINE__, "ParsingProcessThread,Start\r\n");
  while(!pReader->GetCancelParsingProcess())
  {
    int c = pBuffer->ReadOneChar();
    if(c == GnssBuffer::ReadError)
    {
      Sleep(10);
      //totalIdle += 10;
      continue;
    }
    //++totalBytes;
  //tmpBuffer[tmpPtr++] = (U08)c;
  //if(tmpPtr == BufferSize)
  //{
  //  if(pReader->pParsingProg)
  //  {
  //    pReader->pParsingProg(tmpPtr, 0);
  //    { //Test
  //      Utility::WriteBufferToFileAddNew("456.bin", tmpBuffer, tmpPtr);
  //    }
  //  }
  //  tmpPtr = 0;
  //  ASSERT(FALSE);
  //}

    CParser::MessageType type = pParser->PushParsingData((U08)c);
    if(type >= CParser::StqBinary)
    {
      CMessageParser::ParsingResult res = pParser->ParsingMessage(type, pParser->GetOutputMessage(), pParser->GetOutputMessageSize());
      pReader->pParserNotify(type, res, pParser->GetGnssStatus(type), pParser->GetOutputTxt(type));
    }


    /*
    if(c == 0x0A && tmpBuffer[tmpPtr - 2] == 0x0D)
    {
      if(tmpBuffer[0] == 0xA0 && tmpBuffer[1] == 0xA1 && tmpPtr < MAKEWORD(tmpBuffer[3], tmpBuffer[2]) + 7)
      { //Not a complete STQ RAW message
        //txt.Format("P:C,%d,%d,%d\r\n", totalBytes, pBuffer->GetDataSize(), tmpPtr);
        //Utility::Log2(__FILE__, __LINE__, txt);
        continue;
      }
      if(pReader->pParsingProg)
      {
        pReader->pParsingProg(tmpPtr, 0);
        { //Test
          Utility::WriteBufferToFileAddNew("456.bin", tmpBuffer, tmpPtr);
        }
      }
      if(tmpBuffer[0] != 0xA0 || tmpBuffer[1] != 0xA1)
      { //Format error
        txt.Format("P:E,%d,%d,%d\r\n", totalBytes, pBuffer->GetDataSize(), tmpPtr);
        Utility::Log2(__FILE__, __LINE__, txt);
        //ASSERT(FALSE);
      }
      else
      { //A complete STQ RAW message
        //txt.Format("P:V,%d,%d,%d,%02X%02X\r\n", totalBytes, pBuffer->GetDataSize(), tmpPtr, tmpBuffer[4], tmpBuffer[5]);
        //Utility::Log2(__FILE__, __LINE__, txt);
      }
      tmpPtr = 0;
      Sleep(1);
    }
    */
  }
  Utility::Log2(__FILE__, __LINE__, "ParsingProcessThread,End\r\n");
  return 0;
}

class WriteWork
{
public:
  enum { WriteBufferSize = (16 * 1024) };
  enum { WriteArraySize = (16) };
  class WriteObject
  {
  public:
    WriteObject() { };
    ~WriteObject() { };

    void Set(LPCSTR pszFilename, void* ptr, int size)
    {
      strFilename = pszFilename;
      bufferPtr = size;
      memcpy(buffer, ptr, size);
    };
    bool DoWrite()
    {
      return Utility::WriteBufferToFileAddNew(strFilename, buffer, bufferPtr);
    }
    int GetSize() { return bufferPtr; }
  protected:
    U08 buffer[WriteBufferSize];
    int bufferPtr;
    CString strFilename;
  };

  WriteWork()
  {
    nWriteArrayReadPtr = 0;
    nWriteArrayAddPtr = 0;
    bWriteArrayFull = false;
    cancelWriteProcess = true;
    loggerFilePath = "123.bin";
  }

  WriteObject writeArray[WriteArraySize];
  int nWriteArrayReadPtr;
  int nWriteArrayAddPtr;
  bool bWriteArrayFull;
  bool cancelWriteProcess;
  CString loggerFilePath;
  WriteProgress pWriteProg;
  void SetWriteProgress(ReadProgress p) { pWriteProg = p; }

  static UINT WriteProcess(LPVOID pParam)
  {
    WriteWork* writeWork = (WriteWork*)pParam;
    CFile file;
    if(!file.Open(writeWork->loggerFilePath, CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite))
    {
      return 0;
    }
    file.Close();

    while(!writeWork->cancelWriteProcess)
    {
      if(!writeWork->bWriteArrayFull && writeWork->nWriteArrayReadPtr == writeWork->nWriteArrayAddPtr)
      {
        Sleep(5);
        continue;
      }
      bool ret = writeWork->writeArray[writeWork->nWriteArrayReadPtr].DoWrite();
      if(writeWork->pWriteProg)
      {
        writeWork->pWriteProg(writeWork->writeArray[writeWork->nWriteArrayReadPtr].GetSize(), 0);
      }
      writeWork->nWriteArrayReadPtr = (writeWork->nWriteArrayReadPtr + 1 == writeWork->WriteArraySize) ? 0 : (writeWork->nWriteArrayReadPtr + 1);
      writeWork->bWriteArrayFull = false;


    }
    return 0;
  }
};

UINT CComReader::LoggerProcessThread(LPVOID pParam)
{
  //static int totalIdle = 0;
  //static int totalBusy = 0;
  ScopeTimer st, stLogger;
  CComReader* pReader = (CComReader*)pParam;
  GnssBuffer* pBuffer = pReader->pLoggerBuffer;
  WriteWork work;

  work.cancelWriteProcess = false;
  work.SetWriteProgress(pReader->pWriteProg);
  CWinThread* pWriteProcess = ::AfxBeginThread(WriteWork::WriteProcess, &work);

  CString txt;
  DWORD totalBytes = 0;
  int idleCount = 0;
  U08 tmpBuffer[WriteWork::WriteBufferSize];
  int tmpPtr = 0;

  { //Test
    CFile file;
    file.Open("789.bin", CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite);
    file.Close();
  }
  Utility::Log2(__FILE__, __LINE__, "LoggerProcessThread,Start\r\n");
  while(!pReader->GetCancelLoggerProcess())
  {
    DWORD dur = stLogger.GetDuration();
    if(tmpPtr == WriteWork::WriteBufferSize || (dur >= 1000 && tmpPtr > 0) || idleCount > 2 )
    {
      //Save File
      if(work.bWriteArrayFull)
      {
        Sleep(1);
        continue;
      }
      idleCount = 0;
      work.writeArray[work.nWriteArrayAddPtr].Set(work.loggerFilePath, tmpBuffer, tmpPtr);
      work.bWriteArrayFull = false;
      work.nWriteArrayAddPtr = (work.nWriteArrayAddPtr + 1 == work.WriteArraySize) ? 0 : (work.nWriteArrayAddPtr + 1);
      if(work.nWriteArrayAddPtr == work.nWriteArrayReadPtr)
      {
        work.bWriteArrayFull = true;
      }
      stLogger.Reset();
      if(pReader->pLoggerProg)
      {
        pReader->pLoggerProg(tmpPtr, 0);
        { //Test
          Utility::WriteBufferToFileAddNew("789.bin", tmpBuffer, tmpPtr);
        }
      }
      tmpPtr = 0;
    }

    int c = pBuffer->ReadOneChar();
    if(c == GnssBuffer::ReadError)
    {
      Sleep(5);
      //totalIdle += 10;
       ++idleCount;
      continue;
    }
    ++totalBytes;
    ASSERT(tmpPtr < WriteWork::WriteBufferSize);
    tmpBuffer[tmpPtr++] = (U08)c;
  }

  Sleep(500);
  work.cancelWriteProcess = true;
  WaitForSingleObject(pWriteProcess->m_hThread, INFINITE);
  Utility::Log2(__FILE__, __LINE__, "LoggerProcessThread,End\r\n");
  return 0;
}

void CComReader::_StartReadProcess()
{
    cancelReadProcess = false;
    pReadProcess = ::AfxBeginThread(&CComReader::ReadProcessThread, this);
    pGnssBuffer->ClearBuffer();
}

void CComReader::_StopReadProcess()
{
    cancelReadProcess = true;
    WaitForSingleObject(pReadProcess->m_hThread, INFINITE);
    pReadProcess = NULL;
}

void CComReader::_StartParsingProcess()
{
    cancelParsingProcess = false;
    pParsingProcess = ::AfxBeginThread(&CComReader::ParsingProcessThread, this);
    //pGnssBuffer->ClearBuffer();
}

void CComReader::_StopParsingProcess()
{
    cancelParsingProcess = true;
    WaitForSingleObject(pParsingProcess->m_hThread, INFINITE);
    pParsingProcess = NULL;
}

void CComReader::_StartLoggerProcess()
{
    cancelLoggerProcess = false;
    pLoggerProcess = ::AfxBeginThread(&CComReader::LoggerProcessThread, this);
    //pGnssBuffer->ClearBuffer();
}

void CComReader::_StopLoggerProcess()
{
    cancelLoggerProcess = true;
    WaitForSingleObject(pLoggerProcess->m_hThread, INFINITE);
    pLoggerProcess = NULL;
}

void CComReader::StartReadProcess(bool start)
{
  if(start)
  {
    _StartReadProcess();
  }
  else
  {
    ScopeTimer st;
    _StopReadProcess();
    DWORD duration = st.GetDuration();
    CString txt;
    txt.Format("StopRead:%d\r\n", duration);
    Utility::Log2(__FILE__, __LINE__, txt);
  }
}

void CComReader::StartParsingProcess(bool start)
{
  if(start)
  {
    _StartParsingProcess();
  }
  else
  {
    ScopeTimer st;
    _StopParsingProcess();
    DWORD duration = st.GetDuration();
    CString txt;
    txt.Format("StopParsing:%d\r\n", duration);
    Utility::Log2(__FILE__, __LINE__, txt);
  }
}

void CComReader::StartLoggerProcess(bool start)
{
  if(start)
  {
    _StartLoggerProcess();
  }
  else
  {
    ScopeTimer st;
    _StopLoggerProcess();
    DWORD duration = st.GetDuration();
    CString txt;
    txt.Format("StopLogger:%d\r\n", duration);
    Utility::Log2(__FILE__, __LINE__, txt);
  }
}

DWORD CComReader::_GetComReadableSize()
{
  COMSTAT stat;
  if(ClearCommError(m_comHandle, NULL, &stat))
  {
    return stat.cbInQue;
  }
  return 0;
}


//CFileReader
CFileReader::CFileReader(void)
  : CReader()
{
}

CFileReader::~CFileReader(void)
{
}