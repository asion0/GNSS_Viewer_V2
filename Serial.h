#pragma once

// Serial.h
//#define FC_DTRDSR       0x01
//#define FC_RTSCTS       0x02
//#define FC_XONXOFF      0x04
//#define ASCII_BEL       0x07
//#define ASCII_BS        0x08
#define ASCII_CR        0x0D
#define ASCII_LF        0x0A
//#define ASCII_XON       0x11
//#define ASCII_XOFF      0x13
#define BINARY_HD1      0xA0
#define BINARY_HD2      0xA1

#define READ_ERROR		((DWORD)(-1))
#define ReadOK(len)			((len==READ_ERROR)?0 :len)

class CSerial
{
public:
	CSerial();
	~CSerial();

	static int BaudrateTable[];
	static const int BaudrateTableSize;

	enum { InQueueSize = 1024 };
	enum { OutQueueSize = 1024 };
	enum { MaxQueueLoop = 128 };
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
	bool OpenByBaudrate(int port = 2, int baudrate = 115200);
	void Close();
	void CancelTransmission() { m_cancelTransmission = true; }
	void ResetTransmission() { m_cancelTransmission = false; }
	bool GetCancelTransmission() { return m_cancelTransmission; }
	bool ResetPort(int baudIndex);
	void ClearQueue();

	//Read or write data
	/* Read data until buffer full, or com port is empty.
	   If once is false, it'll check com port empty again after read data. //*/
	DWORD ReadData(void* buffer, DWORD bufferSize, bool once = false);
	//int SendDataBatch(unsigned char *buffer, int size);
	//int ReadDataLog( void *buffer, int limit );
	//int gets_log( char *buffer,int size);
	//int getdatalog( unsigned char *,int );	
	//int SendDataNoDelay(unsigned char *buffer, int size );
	//int SendDataDelay(unsigned char *buffer, int size );
	DWORD GetString(void* buffer, DWORD bufferSize, DWORD timeOut);
	//int getsbin( unsigned char *,int );	
	DWORD GetBinary(void* buffer, DWORD bufferSize, DWORD timeout = 10000);
	//int getsbin( unsigned char *,int );	
	DWORD GetBinaryBlock(void* buffer, DWORD bufferSize, DWORD blockSize);
	DWORD GetBinaryBlockInSize(void* buffer, DWORD bufferSize, DWORD blockSize);

	DWORD SendData(const void* buffer, DWORD bufferSize, bool blockTransfer = false, int delayDuration = 0);
	
	DWORD GetBinaryBlockInTime(void* buffer, DWORD bufferSize, DWORD timeout);

protected:

	HANDLE m_comDeviceHandle;
	int m_comPort;
	int m_baudRate;
	bool m_cancelTransmission;
	bool m_isOpened;
	OVERLAPPED m_OverlappedRead;
	OVERLAPPED m_OverlappedWrite;
	DWORD m_sendUnit;
	DWORD m_logStatus;


	int ComInitial();
	int WaitingDataIn();
	bool WriteCommBytes(char* buffer, int bufferSize);
	bool ResetPortNoDelay(int baud);
};
