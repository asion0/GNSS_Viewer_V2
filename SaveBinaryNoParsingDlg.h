#pragma once

class BufferBlock
{
public:
  BufferBlock() 
  {
    for(int i = 0; i < BlockSize; ++i)
    {
      bufferBlcok[i] = new U08[BufferSize];
      bufferIndex[i] = 0;
    }
    writePtr = 0;
    readPtr = 0;
    timeTick = 0;
    maxWriteSize = 0;
    maxBlockSize = 0;
    totalWriteSize = 0;
    maxReadSize = 0;
    alreadyAck = FALSE;
    sleepCount = 0;
    readCount = 0;
  }

  ~BufferBlock()
  {
    for(int i = 0; i < BlockSize; ++i)
    {
      delete [] bufferBlcok[i];
      bufferIndex[i] = 0;
    }
  }
  enum { BufferSize = 128 * 1024 };
  enum { BlockSize = 4 };
  
  bool WriteBlock(U08* ptr, int size);
  const U08* ReadBlock(int index, int* size) { *size = bufferIndex[index]; return bufferBlcok[index]; };
  int GetWritePtr() { return writePtr; }
  int GetReadPtr() { return readPtr; }
  bool NeedWrite() { return writePtr != readPtr; }
  void NextWritePtr() { if(++writePtr == BlockSize) writePtr = 0; }
  void NextReadPtr() 
  { 
    if(bufferIndex[readPtr] > maxWriteSize)
      maxWriteSize = bufferIndex[readPtr];
    totalWriteSize += bufferIndex[readPtr];
    bufferIndex[readPtr] = 0; 
    if(++readPtr == BlockSize) 
      readPtr = 0; 
    int gap = writePtr - readPtr;
    if(gap < 0) gap += BlockSize;
    if(gap >= maxBlockSize) maxBlockSize = gap + 1;
  }
  ULONGLONG totalWriteSize;
  BOOL alreadyAck;

  //For debug
  int bufferIndex[BlockSize];
  int maxWriteSize;
  int maxBlockSize;
  int maxReadSize;
  int sleepCount;
  int readCount;

protected:
  U08 *bufferBlcok[BlockSize];
  int writePtr;
  int readPtr;
  DWORD timeTick;
};

class SaveBinaryNoParsingDlg : public CDialog
{
	DECLARE_DYNAMIC(SaveBinaryNoParsingDlg)

public:
  enum Type
  {
    Default,
    Telit,
  };

	SaveBinaryNoParsingDlg(CWnd* pParent = NULL);   // 
	virtual ~SaveBinaryNoParsingDlg();

	virtual BOOL OnInitDialog();
  void SetFilePath(LPCSTR s) { filePath = s; };
  void SetComPort(int c) { comPort = c; };
  void SetBaudRate(int b) { baudrate = b; };
  void SetType(Type t) { type = t; }
protected:
  BufferBlock buffer;
  Type type;

  CString filePath;
  int comPort;
  int baudrate;
  BOOL stopThreadBufferWrite;
  BOOL stopThreadComRead;
  CWinThread* threadBufferWrite;
  CWinThread* threadComRead;
  CTime startTime;
  int noAckAlerm;

  static UINT ThreadBufferWrite(LPVOID pParam);
  static UINT ThreadComRead(LPVOID pParam);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 
	virtual void OnOK();
  virtual void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

  UINT ComRead();
  UINT BufferWrite();
  void ShowSize(ULONGLONG size, UINT id);
  void ShowDuration(CTimeSpan ts, UINT id);

	DECLARE_MESSAGE_MAP()
};
