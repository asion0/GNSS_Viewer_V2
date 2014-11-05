// Serial.cpp

#include "stdafx.h"
#include "Serial.h"
#include "Utility.h"


int CSerial::BaudrateTable[] = {4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
const int CSerial::BaudrateTableSize = sizeof(CSerial::BaudrateTable) / sizeof(CSerial::BaudrateTable[0]);
const DWORD defaultSendUnit = 512;

CSerial::CSerial()
{
	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));

	m_comDeviceHandle = NULL;
	m_comPort = 0;
	m_baudRate = 0;
	m_cancelTransmission = false;
	m_isOpened = false;
	m_sendUnit = defaultSendUnit;
	m_logStatus = 0;
} 

CSerial::~CSerial()
{
	Close();
}

bool CSerial::Open(int port, int baudIndex)
{
	return OpenByBaudrate(port, BaudrateTable[baudIndex]);
}

bool CSerial::OpenByBaudrate(int port, int baudrate)
{
	CString portName;
	portName.Format("\\\\.\\COM%d", port);

	m_comDeviceHandle = CreateFile(portName, GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, 0, 0);

	if(m_comDeviceHandle==INVALID_HANDLE_VALUE)
	{
		return false;
	}

	ComInitial();

	if(!SetupComm(m_comDeviceHandle, InQueueSize, OutQueueSize))
	{
		return false;
	}

	if(!PurgeComm(m_comDeviceHandle, 
			PURGE_TXABORT | PURGE_RXABORT |
			PURGE_TXCLEAR |	PURGE_RXCLEAR))
	{
		return false;
	}

	if(!ResetPortNoDelay(baudrate))
	{
		return false;
	}

	m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_isOpened = true;
	m_comPort = port;
	return true;
}

int CSerial::ComInitial()
{
	DWORD error = 0;
	COMSTAT comStatus = {0};
//	Utility::Log(__FUNCTION__, "Serial", __LINE__);
	ClearCommError(m_comDeviceHandle, &error, &comStatus);
//	Utility::Log(__FUNCTION__, "Serial", __LINE__);
	return comStatus.cbInQue;
}

void CSerial::Close()
{
	if(!m_isOpened || m_comDeviceHandle == NULL) 
	{
		return;
	}

	if(m_OverlappedRead.hEvent != NULL) 
	{
		CloseHandle(m_OverlappedRead.hEvent);
		m_OverlappedRead.hEvent = NULL;
	}

	if(m_OverlappedWrite.hEvent != NULL) 
	{
		CloseHandle(m_OverlappedWrite.hEvent);
		m_OverlappedWrite.hEvent = NULL;
	}

	CloseHandle(m_comDeviceHandle);
	m_comDeviceHandle = NULL;
	m_isOpened = false;
	return;
}

int CSerial::WaitingDataIn()
{
	int loopcount = 0;
	//int bytesInQueue = ComInitial();
	DWORD error = 0;
	COMSTAT comStatus = {0};
	ClearCommError(m_comDeviceHandle, &error, &comStatus);
	DWORD& bytesInQueue = comStatus.cbInQue;
	while(bytesInQueue==0) 
	{
		++loopcount;
		if(loopcount == MaxQueueLoop)
		{
			return 0;
		}

		if(m_cancelTransmission)
		{
			m_cancelTransmission = false;
			return -1;
		}
		ClearCommError(m_comDeviceHandle, &error, &comStatus);
		//bytesInQueue = ComInitial();
	};
	return bytesInQueue;
}

//Read data until buffer full, or com port is empty.
// if once is false, it'll check com port empty again after read data.
DWORD CSerial::ReadData(void* buffer, DWORD bufferSize, bool once)
{
	if(!m_isOpened || m_comDeviceHandle == NULL) 
	{
		return READ_ERROR;
	}

	DWORD totalSize = 0;
	char* bufferIter = (char*)buffer;
	do
	{
		DWORD dwBytesRead = 0;
		dwBytesRead = (DWORD)WaitingDataIn();

		if(0 == dwBytesRead)
		{
			return totalSize;
		}

		if(READ_ERROR == dwBytesRead)
		{
			return READ_ERROR;
		}

		if(bufferSize < (totalSize + dwBytesRead)) 
		{
		
			dwBytesRead = bufferSize - totalSize;
		}

		DWORD dwBytesDoRead = 0;
//		BOOL bReadStatus = ReadFile(m_comDeviceHandle, bufferIter, 
//							dwBytesRead, &dwBytesDoRead, &m_OverlappedRead);
		BOOL bReadStatus = ReadFile(m_comDeviceHandle, bufferIter, 
							dwBytesRead, &dwBytesDoRead, 0);

		//if(dwBytesDoRead != dwBytesRead)
		{
		//	int a = 0;
		}
		//if(bufferSize>1 && totalSize!=0)
		{
		//	int a = 0;
		}	

		if(!bReadStatus)
		{ 
			DWORD error = ::GetLastError();
			if(error == ERROR_IO_PENDING)
			{
				WaitForSingleObject(m_OverlappedRead.hEvent, IoPendingTimeOut);
				return totalSize + dwBytesDoRead;
			}
			return 0;
		}
		bufferIter += dwBytesRead;
		totalSize += dwBytesRead;
	} while(!once && totalSize < bufferSize);
	return totalSize;
}

DWORD CSerial::SendData(const void* buffer, DWORD bufferSize, bool blockTransfer, int delayDuration)
{
	if(!m_isOpened || m_comDeviceHandle == NULL) 
	{
		return 0;
	}

//	DWORD sendUnit = SendUnit;
	DWORD sentBytes = 0;
	DWORD leftSize = bufferSize;
	char* bufferIter = (char*)buffer;

	while(leftSize > 0)
	{
		sentBytes = (leftSize >= m_sendUnit) ? m_sendUnit : leftSize;

		if(blockTransfer)
		{
			WriteCommBytes(bufferIter, sentBytes);
		}
		else
		{
			for(DWORD i=0; i<sentBytes; ++i)
			{
				WriteCommBytes(bufferIter + i, 1);
			}
		}

		if(delayDuration)
		{
			Sleep(delayDuration);
		}

		bufferIter += sentBytes;
		leftSize -= sentBytes;
	}
	return bufferSize;	
}

bool CSerial::WriteCommBytes(char* buffer, int bufferSize)
{
	DWORD dwBytesDoWrite = 0;
	BOOL bWriteStatus = WriteFile(m_comDeviceHandle, buffer, 
							bufferSize, &dwBytesDoWrite, &m_OverlappedWrite);

	if(!bWriteStatus)
	{ 
		DWORD error = ::GetLastError();
		if(error == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_OverlappedWrite.hEvent, IoPendingTimeOut);
			GetOverlappedResult(m_comDeviceHandle, &m_OverlappedWrite, &dwBytesDoWrite, FALSE);
			m_OverlappedWrite.Offset += dwBytesDoWrite;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool CSerial::ResetPortNoDelay(int baudrate)
{
	DCB dcb = {0};
	if(!GetCommState(m_comDeviceHandle, &dcb))
	{
		DWORD error = ::GetLastError();
		return false;
	}
	dcb.BaudRate = baudrate;
	dcb.Parity   = NOPARITY;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;

	if(!SetCommState(m_comDeviceHandle, &dcb))
	{
		DWORD error = ::GetLastError();
		return false;
	}

	m_baudRate = baudrate;
	return true;
}

bool CSerial::ResetPort(int baudIndex)
{
	if(ResetPortNoDelay(BaudrateTable[baudIndex]))
	{
		Sleep(800);
		return true;
	}
	return false;
}

void CSerial::ClearQueue()
{
	if(!m_isOpened || m_comDeviceHandle == NULL) 
	{
		return;
	}
//	Utility::Log(__FUNCTION__, "Serial", __LINE__);
	ComInitial();
//	Utility::Log(__FUNCTION__, "Serial", __LINE__);
	PurgeComm(m_comDeviceHandle, PURGE_RXCLEAR);
//	Utility::Log(__FUNCTION__, "Serial", __LINE__);
	FlushFileBuffers(m_comDeviceHandle);
//	Utility::Log(__FUNCTION__, "Serial", __LINE__);
}

//Read until eos or empty.
DWORD CSerial::GetString(void* buffer, DWORD bufferSize, DWORD timeOut)
{
	char* bufferIter = (char*)buffer;
	DWORD totalSize = 0;
	ScopeTimer t;
	do
	{ 
		DWORD nBytesRead = ReadData(bufferIter, 1);
		if(nBytesRead <= 0 && t.GetDuration() > timeOut)
		//if(nBytesRead <= 0)
		{
			break;
		}

		if(*bufferIter == 0)
		{
			break;
		}

		bufferIter += nBytesRead;
		totalSize += nBytesRead;
	} while(totalSize < bufferSize);
	return totalSize;
}

enum BufferContentType
{
	Unknown = 0,
	BinaryMessageStart,
	BinaryMessageEnd,
	MessageEnd,
};

BufferContentType CheckBufferContent(U08 current, U08 previous, U08 bufferStart, U08 sizeHigh, U08 sizeLow, DWORD total)
{
	if(BINARY_HD2==current && BINARY_HD1==previous)
	{
		return BinaryMessageStart;
	}

	if(ASCII_LF==current && ASCII_CR==previous)
	{
		if(BINARY_HD1==bufferStart) 
		{
			int packetLength = (sizeHigh << 8) + sizeLow;
			if(total == (packetLength + 6))
			{
				return BinaryMessageEnd;
			}
		}
		return MessageEnd;
	}
	return Unknown;
}

DWORD CSerial::GetBinary(void *buffer, DWORD bufferSize, DWORD timeout)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	DWORD failCount = 10;
	ScopeTimer clock;
	while(totalSize < bufferSize - 1)
	{ 
		if(clock.GetDuration() > timeout)
		{
			return READ_ERROR;
		}

		if(m_cancelTransmission)
		{
			m_cancelTransmission = false;
			return READ_ERROR;
		}

		DWORD dwBytesDoRead = 0;
		DWORD dwErrorFlags = 0;
		COMSTAT comStat;
		BOOL b2 = ClearCommError(m_comDeviceHandle, &dwErrorFlags, &comStat);
		if(!b2)
		{
			DWORD err = ::GetLastError();
			return -1;
		}
		if(comStat.cbInQue <= 0) 
		{
			Sleep(10);
			continue;
		}

		DWORD bytesinbuff = comStat.cbInQue;
		while(bytesinbuff)
		{
			if(m_cancelTransmission)
			{
				m_cancelTransmission = false;
				return -1;
			}
			BOOL bb = ReadFile(m_comDeviceHandle, bufferIter, 1, &dwBytesDoRead, 0);
			if(dwBytesDoRead <= 0)
			{	//Read fail.
				DWORD dwErr = ::GetLastError();
				continue;
			}

			if(totalSize > 0)
			{	//not first char.
				if(*bufferIter==0xa1 && *(bufferIter-1)==0xa0)
				{
					bufferIter -= totalSize;
					*bufferIter = 0xa0; 
					++bufferIter;
					*bufferIter = 0xa1; 
					++bufferIter;
					totalSize = 2;
					continue;
				}
				else if(*bufferIter==0x0a && *(bufferIter-1)==0x0d)
				{
					unsigned char *chk_ptr = bufferIter - totalSize;
					
					if (*chk_ptr == 0xa0)
					{
						int tmp_len = *(chk_ptr + 2);
						tmp_len = tmp_len << 8 | *(chk_ptr+3);
						if (totalSize == tmp_len + 6) 
						{
							*(bufferIter+1) = 0;
							return totalSize + 1;
						}
					}
					else
					{
						return totalSize;
					}
				}
			}
			++totalSize;
			if (totalSize >=  bufferSize - 1)
			{	//Check 
				*(bufferIter+1) = 0;

				break;
			}
				
			++bufferIter;
			--bytesinbuff;
		} //while(bytesinbuff)
	} //while(total < size - 1)
	return totalSize;
}

DWORD CSerial::GetBinaryBlock(void* buffer, DWORD bufferSize, DWORD blockSize)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	do
	{ 
		DWORD nBytesRead = ReadData(bufferIter, 1);
		if(nBytesRead <= 0)
		{
			continue;
		}

		if(totalSize == blockSize && (*bufferIter==ASCII_LF) && (*(bufferIter-1)==ASCII_CR))
		{
			break;
		}
		++totalSize;
		if(totalSize >=  bufferSize - 1)
		{	//add end of string (eos)
			break;
		}
		bufferIter++;
	} while(totalSize < bufferSize);
	return totalSize;
}

DWORD CSerial::GetBinaryBlockInTime(void* buffer, DWORD bufferSize, DWORD timeout)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	const int tempBufferSize = 1024;
	U08* tmpBuffer[tempBufferSize];
	ScopeTimer t;
	do
	{
		if(t.GetDuration() >= timeout)
		{
			return READ_ERROR;
		}

		memset(tmpBuffer, 0, tempBufferSize);
		int readSize = ((totalSize + tempBufferSize) <= bufferSize) ? tempBufferSize : bufferSize - totalSize;
		DWORD nBytesRead = ReadData(tmpBuffer, readSize);
		if(nBytesRead == 0)
		{
			continue;
		}

		if(nBytesRead == READ_ERROR)
		{
			return READ_ERROR;
		}

		if((totalSize + nBytesRead) <= bufferSize)
		{
			memcpy(bufferIter, tmpBuffer, nBytesRead);
			bufferIter += nBytesRead;
			totalSize += nBytesRead;
		}
	} while(totalSize < bufferSize);
	return totalSize;
}
