#pragma once
// Serial.h

#define ASCII_CR        0x0D
#define ASCII_LF        0x0A

#define BINARY_HD1      0xA0
#define BINARY_HD2      0xA1

#define READ_ERROR		((DWORD)(-1))
#define ReadOK(len)			((len==READ_ERROR) ?0 :len)

//enum MessageType {
//  MtUnknown = 0,
//  StqBinary,
//  NmeaMessage,
//  RtcmMessage,
//};

class CSerial
{
public:
	CSerial();
	~CSerial();

	//static int BaudrateTable[];
	//static const int BaudrateTableSize;

	enum { InQueueSize = 1024 * 128 };
	enum { OutQueueSize = 1024 * 8 };
	enum { MaxQueueLoop = 10 };
	enum { IoPendingTimeOut = 2000 };

	//Getter and Setter
	bool IsOpened() { return m_isOpened; }
	int GetComPort() { return m_comPort; }
	int GetBaudRate() { return m_baudRate; }
	HANDLE GetHandle() { return m_comDeviceHandle; };
	void SetSentUnit(DWORD s) { m_sendUnit = s; };
	void SetLogStatus(DWORD s) { m_logStatus = s; };

	//Flow control
	bool Open(int port = 2, int baudIndex = 5);
#if(SPECIAL_BAUD_RATE)
	bool OpenByBaudrate(LPCSTR comPort, int baudrate = 172800);
#else
	bool OpenByBaudrate(LPCSTR comPort, int baudrate = 115200);
#endif
	void Close();
	void CancelTransmission() { m_cancelTransmission = true; }
	void ResetTransmission() { m_cancelTransmission = false; }
	bool GetCancelTransmission() { return m_cancelTransmission; }
	bool* GetCancelTransmissionPtr() { return &m_cancelTransmission; }
	bool ResetPort(int baudIndex);
	void ClearQueue();

	//Read or write data
	// Read data until buffer full, or com port is empty.
	// If once is false, it'll check com port empty again after read data.
	DWORD ReadData(void* buffer, DWORD bufferSize, bool once = false);
	DWORD GetString(void* buffer, DWORD bufferSize, DWORD timeOut);
  BOOL GetOneChar(U08 *c, DWORD* dwBytesDoRead, DWORD timeout);
	DWORD GetBinary(void* buffer, DWORD bufferSize, DWORD timeout = 2000);
	//DWORD GetParsingData(void* buffer, DWORD bufferSize, DWORD timeout = 2000);
  //MessageType GetParsingData(void *buffer, DWORD bufferSize, DWORD* totalSize, DWORD timeout = 2000);

#if CUSTOMER_ZENLANE_160808
	DWORD GetZenlaneMessage(void* buffer, DWORD bufferSize, DWORD timeout = 2000);
#endif
	DWORD GetZenlaneResponse1(void* buffer, DWORD bufferSize, DWORD timeout = 2000);
	DWORD GetBinaryAck(void* buffer, DWORD bufferSize, DWORD timeout = 2000);
	DWORD GetBinaryBlock(void* buffer, DWORD bufferSize, DWORD blockSize);
	DWORD GetBinaryBlockInSize(void* buffer, DWORD bufferSize, DWORD blockSize);

	DWORD SendData(const void* buffer, DWORD bufferSize, bool blockTransfer = false, int delayDuration = 0);
	//DWORD GetBinaryBlockInTime(void* buffer, DWORD bufferSize, DWORD timeout);
	static inline void AddDebugString(const char* dbg);
	static void SaveDebugString(bool backup = false);
	static void SetDebugModeOn(bool on = true) { debugModeOn = on; };
	static void SetDebugName(LPCSTR name) { debugName = name; };
	static DWORD readCount;
	DWORD errorCode;

protected:
	static const int debugSize = 4 * 1024 * 1024;
	static char debugBuffer[debugSize];
	static char *debugPtr;
	static bool debugModeOn;
	static CString debugWorking;
	static CString debugName;
	static bool m_cancelTransmission;

	HANDLE m_comDeviceHandle;
	int m_comPort;
	int m_baudRate;
	bool m_isOpened;
	OVERLAPPED m_OverlappedRead;
	OVERLAPPED m_OverlappedWrite;
	DWORD m_sendUnit;
	DWORD m_logStatus;


	int ComInitial();
	int WaitingDataIn();
	bool WriteCommBytes(char* buffer, int bufferSize);
	bool ResetPortNoDelay(int baud);

public:
  static DWORD GetComBinaryAck(HANDLE com, void *buffer, DWORD bufferSize, DWORD timeout);

};