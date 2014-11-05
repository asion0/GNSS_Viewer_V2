#include "stdafx.h"
#include "GPS.h"
#include "Serial.h"
#include "GPSDlg.h"

#include "SoftImDwDlg.h"
#include "InputPasswordDlg.h"
#include "CryptHash.inl"

#define tagSize 8L
const unsigned char GG12A_TAG[tagSize] = {0xf4,0x0e,0xe0,0xb8,0x16,0xa0,0x01,0xa2};
const unsigned char TAG_ADDR[tagSize] = {0xf4,0x0e,0xe0,0xb8,0x16,0xa0,0x01,0xa1};
const int BinSizeTimeout = 20*1000;

typedef struct 
{
	char inputFile[256];
	char outputFile[256];
	int gg12a;
	unsigned long long tagAddress;
} Param;

int ParserBinFile(const char *filename, Param *p)
{
	unsigned char buf[tagSize];
	int totalTagSize = 0;

#if defined(_MSC_VER)
	FILE* f = NULL;
	fopen_s(&f, filename, "rb");
#else
	FILE* f = fopen(filename, "rb");
#endif

	p->inputFile[0] = 0;
	p->outputFile[0] = 0;
	p->gg12a = 0;
	p->tagAddress = 0;

	if(f==NULL)
	{
		return totalTagSize;
	}

#if defined(_MSC_VER)
	strcpy_s(p->inputFile, sizeof(p->inputFile), filename);
#else
	strcpy(p->inputFile, filename);
#endif

	fseek(f, 0 - tagSize, SEEK_END);		
	fread(buf, 1, tagSize, f);
	if(0==memcmp(buf, GG12A_TAG, tagSize))
	{
		p->gg12a = 1;
		fseek(f, 0 - tagSize, SEEK_END);
		totalTagSize += tagSize;
	}
	fseek(f, 0 - tagSize, SEEK_CUR);		
	fread(buf, 1, tagSize, f);

	if(0==memcmp(buf, TAG_ADDR, tagSize))
	{	
		fseek(f, 0 - tagSize - tagSize, SEEK_CUR);		
		fread(buf, 1, tagSize, f);

		p->tagAddress = *((unsigned long long*)buf);
		totalTagSize += tagSize * 2;
	}
	
	fclose(f);
	return totalTagSize;
}
U16 customerCrc = 0;

UINT CGPSDlg::GetBinFromResource(int baud)
//void CGPSDlg::GetBinFromResource(U08* &sData, long &bootLoaderSize)
{
	UINT* promTable = NULL;
#if (GNSS_VIEWER && !GG12A)
	UINT glonassPromTable[] = { 
				(IDR_GNSS_PRELOADER_4800),
				(IDR_GNSS_PRELOADER_9600),
				(IDR_GNSS_PRELOADER_19200),
				(IDR_GNSS_PRELOADER_38400),
				(IDR_GNSS_PRELOADER_57600),
				(IDR_GNSS_PRELOADER_115200),
				(IDR_GNSS_PRELOADER_230400),
			};
	promTable = glonassPromTable;
#endif
#if (GNSS_VIEWER && GG12A)
	UINT gg12aPromTable[] = {
				(IDR_GG12A_PRELOADER_4800),
				(IDR_GG12A_PRELOADER_9600),
				(IDR_GG12A_PRELOADER_19200),
				(IDR_GG12A_PRELOADER_38400),
				(IDR_GG12A_PRELOADER_57600),
				(IDR_GG12A_PRELOADER_115200),
				(IDR_GG12A_PRELOADER_230400),
			};
	promTable = gg12aPromTable;
#elif (GPS_VIEWER)
	UINT gpsPromTable[] = {
			(IDR_GPS_PRELOADER_4800),
			(IDR_GPS_PRELOADER_9600),
			(IDR_GPS_PRELOADER_19200),
			(IDR_GPS_PRELOADER_38400),
			(IDR_GPS_PRELOADER_57600),
			(IDR_GPS_PRELOADER_115200),
			(IDR_GPS_PRELOADER_230400),
		};
	promTable = gpsPromTable;
#endif

	return promTable[baud];
}

UINT CGPSDlg::GetSrecFromResource(int buad)
{
	UINT gpsAddTagSrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_GPS_CREATETAG_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT gpsDelTagSrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_GPS_REMOVETAG_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT gnssAddTagSrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_GNSS_CREATETAG_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT gnssDelTagSrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_GNSS_REMOVETAG_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT gpsSrecTable[] = {
			IDR_GPS_SREC_4800,
			IDR_GPS_SREC_9600,
			IDR_GPS_SREC_19200,
			IDR_GPS_SREC_38400,
			IDR_GPS_SREC_57600,
			IDR_GPS_SREC_115200,
			IDR_GPS_SREC_230400,
			NULL,
			NULL };

	UINT gnssSrecTable[] = {
			IDR_GNSS_SREC_4800,
			IDR_GNSS_SREC_9600,
			IDR_GNSS_SREC_19200,
			IDR_GNSS_SREC_38400,
			IDR_GNSS_SREC_57600,
			IDR_GNSS_SREC_115200,
			IDR_GNSS_SREC_230400,
			NULL,
			NULL };

	UINT parallelSrecTable0[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_PARALLEL_TYPE0_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT parallelSrecTable1[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_PARALLEL_TYPE1_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT v8SrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_V8_DL_SREC_115200,
			IDR_V8_DL_SREC_230400,
			IDR_V8_DL_SREC_460800,
			NULL };

	UINT v8AddTagSrecTable[] = {
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC };
			
	UINT* srecTable = NULL;
	switch(m_DownloadMode)
	{
		case EnternalLoader:
			ASSERT(FALSE);
			break;
		case EnternalLoaderInBinCmd:
			ASSERT(FALSE);
			break;
		case ParallelDownloadType0:
			srecTable = parallelSrecTable0;
			break;
		case ParallelDownloadType1:
			srecTable = parallelSrecTable1;
			break;
		case InternalLoaderV6Gps:
			srecTable = gpsSrecTable;
			break;
		case InternalLoaderV6Gnss:
			srecTable = gnssSrecTable;
			break;
		case InternalLoaderV6Gg12a:
			ASSERT(FALSE);
			break;
		case InternalLoaderV6GpsAddTag:
			srecTable = gpsAddTagSrecTable;
			break;
		case InternalLoaderV6GpsDelTag:
			srecTable = gpsDelTagSrecTable;
			break;
		case InternalLoaderV6GnssAddTag:
			srecTable = gnssDelTagSrecTable;
			break;
		case InternalLoaderV6GnssDelTag:
			srecTable = gnssDelTagSrecTable;
			break;
		case InternalLoaderV8AddTag:
			srecTable = v8AddTagSrecTable;
			break;
		case HostBasedDownload:
			ASSERT(FALSE);
			break;
		case HostBasedCmdOnly:
			ASSERT(FALSE);
			break;
		case InternalLoaderV8:
			srecTable = v8SrecTable;
			break;
		case InternalLoaderSpecial:
			return _RESOURCE_LOADER_ID_;
		default:
			ASSERT(FALSE);
	}
	if(srecTable)
	{
		return srecTable[buad];
	}
	return NULL;
}

WlfResult WaitingLoaderFeedback(CSerial* serial, int TimeoutLimit, CWnd* msgWnd)
{
	typedef struct _WlfEntry
	{
		WlfResult result;
		const char* string;
	} WlfEntry;

	WlfEntry feedbackTable[] = {
		{ wlf_Ready, "READY"},
		{ wlf_Ready1, "READY1"},
		{ wlf_Ready2, "READY2"},
		{ wlf_error41, "Error41"},
		{ wlf_error42, "Error42"},
		{ wlf_error43, "Error43"},
		//^^^^^^^^^for Loader debug
		{ wlf_error5, "Error5"},
		{ wlf_error4, "Error4"},
		{ wlf_error3, "Error3"},
		{ wlf_error2, "Error2"},
		{ wlf_error1, "Error1"},
		{ wlf_resendbin, "Resendbin"},
		{ wlf_reset, "Reset"},
		{ wlf_resend, "Resend"},
		{ wlf_end, "END"},
		{ wlf_ok, "OK"},
		{ wlf_None, "WAIT"},
		{ wlf_None, ""},
	};

	WlfResult nReturn = wlf_ok;
	clock_t start = 0;
	clock_t timeDuration = 0;

	const int bufferSize = 256;
	char buff[bufferSize] = {0};
	char messages[100] = {0};

	start = clock();
	while(1)
	{
		timeDuration = clock() - start;	
		if(timeDuration > 1000 && msgWnd != NULL)
		{
			msgWnd->PostMessage(UWM_SETTIMEOUT, timeDuration, 0);
		}
		if(timeDuration > TimeoutLimit)
		{
			nReturn = wlf_timeout;
			break;
		}

		memset(buff, 0, sizeof(buff));
		serial->GetString(buff, sizeof(buff), TimeoutLimit - timeDuration);	     
		Utility::Log(__FUNCTION__, buff, TimeoutLimit - timeDuration);

		nReturn = wlf_None;
		int tableSize = sizeof(feedbackTable) / sizeof(feedbackTable[0]);
		while(buff[0] && tableSize--)
		{
			if(0==strcmp(buff, feedbackTable[tableSize].string)) 
			{
				nReturn = feedbackTable[tableSize].result;
				break;
			}
		}
		if(wlf_None != nReturn)
		{
			break;
		}
		Sleep(100);
	}

	if(nReturn > wlf_timeout)
	{
		AfxMessageBox("Unknow Error!");
		Utility::Log(__FUNCTION__, "return", (int)nReturn);
	}
	Utility::Log(__FUNCTION__, "return", (int)nReturn);
	return nReturn;
}

U08 CGPSDlg::PlRomNoAlloc(const CString& prom_path)
{
	int buad = 0;
	buad = BAUDRATE;

	Param promTag;
	int extraSize = ParserBinFile(prom_path, &promTag);
	if(promTag.tagAddress==0)
	{
		promTag.tagAddress = 0x00077ffc;
	}

	U08 mycheck = 0;
	char messages[100] = {0};

	FILE *f = NULL;

	if(0 != fopen_s(&f, prom_path, "rb"))
	{
		return RETURN_ERROR;
	}

	fseek(f, 0, SEEK_END);
	long promLen = ftell(f);
	promLen -= extraSize;

	//Get preloader bin and it's size.
	CString externalPreloaderPath;
	BinaryData preLoader;
	int bootLoaderSize = 0;
	if(theApp.CheckExternalPreloader(externalPreloaderPath))
	{
		bootLoaderSize = preLoader.ReadFromFile(externalPreloaderPath);
		ASSERT(bootLoaderSize!=0);
	}
	else
	{
		//bootLoaderSize = preLoader.ReadFromResource(GetBinFromResource(buad), "BIN");
		bootLoaderSize = preLoader.ReadFromResource(GetBinFromResource(GetBaudrate()), "BIN");
		ASSERT(bootLoaderSize!=0);
	}
	const U08* sData = preLoader.Ptr();

//	GetBinFromResource(sData, bootLoaderSize);
	int prom_size = promLen + BOOTLOADER_SIZE;

	fseek(f, 0, SEEK_SET);

	char c;
	while(fread(&c,1,1,f)!=0)
	{
		mycheck = mycheck + c;
	}

	for(int i=0; i<bootLoaderSize; ++i)
	{
		mycheck += sData[i];
	}

	bool bResendbin = true;
	while(bResendbin)
	{
		unsigned long long check = prom_size + mycheck + promTag.tagAddress;
		sprintf_s(messages, sizeof(messages), "BINSIZE = %d Checksum = %d %lld %lld ", prom_size, mycheck, promTag.tagAddress, check);
		m_serial->SendData((U08*)messages, (U16)strlen(messages)+1);	
		Utility::Log(__FUNCTION__, messages, __LINE__);

		switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, BinSizeTimeout, &m_responseList))
		{
		case wlf_ok:
			bResendbin = false;
			break;
		case wlf_timeout:
			Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
			fclose(f);
			AfxMessageBox("Timeout: GPS device no response.");						
			return RETURN_ERROR;
			break;
		case wlf_error1:
			AfxMessageBox("Flash Wrong");	
			fclose(f);
			return RETURN_ERROR;
			break;
		case wlf_error3:
			AfxMessageBox("Format Error!");					
			fclose(f);
			return RETURN_ERROR;		        
			break;
		case wlf_error4:
			AfxMessageBox("Flash Test Failed!");				
			fclose(f);
			return RETURN_ERROR;		        
			break;
		case wlf_error5:
			AfxMessageBox("srec timeout!");	
			fclose(f);
			return RETURN_ERROR;		        
			break;
		case wlf_resendbin:
			Utility::Log(__FUNCTION__, messages, __LINE__);
			bResendbin = true;
			break;
		default:
			Utility::LogFatal(__FUNCTION__, messages, __LINE__);
			fclose(f);
			return RETURN_ERROR;		        
			break;
		}	//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))	
	}	//while(bResendbin)
// Alex wait
	m_psoftImgDlDlg->PostMessageA(UWM_SETPROMPT_MSG, IDS_WAITROM_MSG);
//	sprintf_s(messages, sizeof(messages), "Waiting for Rom Writing...");
//	m_psoftImgDlDlg->m_msg.SetWindowText(messages);	
	
	int result = RETURN_NO_ERROR;
	
#if(RESEND_DOWNLOAD==1)
	int retryCount = 3;
	do
	{
		result = SendRomBuffer3(sData, bootLoaderSize, f, promLen, (buad >= 6), m_psoftImgDlDlg);
	} while(result == RETURN_RETRY && (retryCount--) > 0);
#else
	result = SendRomBuffer(prom_buff, prom_size,(buad >= 6) ,&(m_psoftImgDlDlg->m_progress), &(m_psoftImgDlDlg->m_percent), &(m_psoftImgDlDlg->m_msg));
#endif
	fclose(f);
	BoostBaudrate(TRUE);

// Alex wait
	m_psoftImgDlDlg->PostMessageA(UWM_SETPROMPT_MSG, IDS_ROMWRITINGOK_MSG);
//	m_psoftImgDlDlg->m_msg.SetWindowText("Rom Writing is OK!");	

	//free(prom_buff);
	return RETURN_NO_ERROR;		
}

bool CGPSDlg::FirmwareUpdate()
{
	bool isSuccessful = false;
	for(int i=0; i<3; i++)
	{
//		m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, 0, 100);
		U08 res = 0;
		switch(m_DownloadMode)
		{
			case InternalLoaderV6Gps:
			case InternalLoaderV6Gnss:
			case InternalLoaderV6Gg12a:
			case InternalLoaderV6GpsAddTag:
			case InternalLoaderV6GpsDelTag:
			case InternalLoaderV6GnssAddTag:
			case InternalLoaderV6GnssDelTag:
			case HostBasedDownload:
			case HostBasedCmdOnly:
				res = PlRomNoAlloc2(m_strDownloadImage);
				break;
			case EnternalLoader:
			case EnternalLoaderInBinCmd:
			case InternalLoaderV8:
			case ParallelDownloadType0:
			case ParallelDownloadType1:
			case InternalLoaderV8AddTag:
			case InternalLoaderSpecial:
				res = PlRomNoAllocV8(m_strDownloadImage);
				break;
			default:
				ASSERT(FALSE);
		}

		if(res == RETURN_RETRY)
		{
			add_msgtolist("Software Image Download Retry...");
		}
		else 
		{
			if(res == RETURN_NO_ERROR)
			{
				add_msgtolist("Software Image Download Successful...");	
				isSuccessful = true;
			}
			else
			{
				add_msgtolist("Software Image Download Failed...");
			}
			break;
		}
	}
	return isSuccessful;
}

U08 CalcCheckSum16(const U08* dataPtr, int sentbytes)
{
	U16 checkSum = 0;
	const U08* ptr = dataPtr;
	const U16 loopCount = sentbytes / sizeof(U16);
	U16 i;

	for(i=0; i<loopCount; ++i)
	{
		U16 word =  MAKEWORD(*(ptr+1), *ptr);
		checkSum += word;
		ptr += 2;
	}
	return HIBYTE(checkSum) + LOBYTE(checkSum);
	//*/
}

int CGPSDlg::SendRomBuffer3(const U08* sData, int sDataSize, FILE *f, 
							int fbinSize, bool needSleep, CWnd* notifyWnd)
{
	int nFlashBytes = FlashBytes[m_nDownloadBufferIdx];
	/* sending data */	
	const int headerSize = 3;
	const int buffSize = nFlashBytes + headerSize;
	int realTotalSize = BOOTLOADER_SIZE + fbinSize;
	//U08 buff[buffSize];
	U08* buff = new U08[buffSize];
	char messages[100] = {0};
	bool bResend = false;
	int lSize = 0;
	int sentbytes = 0;
	int TotalByte = 0;
	int nBytesSent = 0;
	U16 sequence = 0;

	if(sData)
	{
		char* prom_ptr = (char*)sData;
		lSize = BOOTLOADER_SIZE;

		while(lSize>0)
		{ 
			if(!bResend)
			{
				sentbytes = nFlashBytes;
				TotalByte += sentbytes;	

				memset(buff, 0, buffSize);
				if(sDataSize > nFlashBytes)
				{
					memcpy(&buff[headerSize], prom_ptr, nFlashBytes);
					sDataSize -= nFlashBytes;
					prom_ptr += nFlashBytes;
				}
				else if(sDataSize > 0)
				{
					memcpy(&buff[headerSize], prom_ptr, sDataSize);
					sDataSize = 0;
				}
				int checkSum = CalcCheckSum16(&(buff[headerSize]), sentbytes);
				//Fill packet
				buff[0] = HIBYTE(sequence);
				buff[1] = LOBYTE(sequence);
				buff[2] = checkSum;
			}

			if( EnternalLoaderInBinCmd==m_DownloadMode || 
				EnternalLoader==m_DownloadMode || 
				InternalLoaderV8==m_DownloadMode ||
				InternalLoaderSpecial==m_DownloadMode )
			{
				nBytesSent = SendToTargetNoAck((U08*)buff + headerSize, sentbytes);	 
			}
			else
			{
				nBytesSent = SendToTargetNoAck((U08*)buff, sentbytes + headerSize);	 
			}

			switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, notifyWnd))
			{
			case wlf_ok:
				sequence++;
				lSize -= sentbytes;
				bResend = false;
				break;
			case wlf_timeout:
				Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
				return RETURN_ERROR;
				break;
			case wlf_resend:
				Utility::Log(__FUNCTION__, "wlf_resend", __LINE__);
				bResend = true;
				break;
			case wlf_reset:
				Utility::Log(__FUNCTION__, "wlf_reset", __LINE__);
				return RETURN_RETRY;
				break;
			default:
				Utility::LogFatal(__FUNCTION__, messages, __LINE__);
				return RETURN_ERROR;
				break;
			}	
//* Alex wait
			if(!bResend)
			{
				downloadProgress += sentbytes;
			}
			m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, downloadProgress, downloadTotalSize);

			if(needSleep)
			{
				Sleep(1);
			}
		}	//End of while(lSize>0)
	} //if(sData)
//---------------------------
	bResend = false;
	lSize = (sData) ? fbinSize - BOOTLOADER_SIZE : fbinSize;

	while(lSize>0)
	{ 
		if(!bResend)
		{
			sentbytes = (lSize >= nFlashBytes)?nFlashBytes :lSize;
			TotalByte += sentbytes;	

			memset(buff, 0, buffSize);
			int realRead = fread(&buff[headerSize], 1, sentbytes, f);
			if(realRead<sentbytes)
			{
				fseek(f, 0, SEEK_SET);
				realRead = fread(&buff[realRead + headerSize], 1, sentbytes - realRead, f);
			}
			int checkSum = CalcCheckSum16(&(buff[headerSize]), sentbytes);
			//Fill packet
			buff[0] = HIBYTE(sequence);
			buff[1] = LOBYTE(sequence);
			buff[2] = checkSum;
		}

		if( EnternalLoaderInBinCmd==m_DownloadMode || 
			EnternalLoader==m_DownloadMode || 
			InternalLoaderV8==m_DownloadMode ||
			ParallelDownloadType0==m_DownloadMode || 
			ParallelDownloadType1==m_DownloadMode || 
			InternalLoaderV8AddTag==m_DownloadMode ||
			InternalLoaderSpecial==m_DownloadMode )
		{
			nBytesSent = SendToTargetNoAck((U08*)buff + headerSize, sentbytes);	 
		}
		else
		{
			nBytesSent = SendToTargetNoAck((U08*)buff, sentbytes + headerSize);	 
		}

		switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))
		{
		case wlf_ok:
			sequence++;
			lSize -= sentbytes;
			bResend = false;
			break;
		case wlf_timeout:
			Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
			return RETURN_ERROR;
			break;
		case wlf_resend:
			Utility::Log(__FUNCTION__, "wlf_resend", __LINE__);
			bResend = true;
			break;
		case wlf_reset:
			Utility::Log(__FUNCTION__, "wlf_reset", __LINE__);
			return RETURN_RETRY;
			break;
		default:
			Utility::LogFatal(__FUNCTION__, messages, __LINE__);
			return RETURN_ERROR;
			break;
		}	
//* Alex wait
		if(!bResend)
		{
			downloadProgress += sentbytes;
		}
		m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, downloadProgress, downloadTotalSize);

		if(needSleep)
		{
			Sleep(1);
		}
	}	//End of while(lSize>0)

	switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, 20000, notifyWnd))
	{
	case wlf_end:
		sprintf_s(messages, sizeof(messages), "The total bytes transferred = %d", TotalByte);
		add_msgtolist(messages);
		//msgWnd->SetWindowText(messages);	
		break;
	case wlf_timeout:
		AfxMessageBox("Timeout: GPS device no response.");
		return RETURN_ERROR;
		break;
	case wlf_error2:
		AfxMessageBox("Checksum Wrong!");						
		return RETURN_RETRY;
		break;
	case wlf_reset:
		AfxMessageBox("Reset!");						
		return RETURN_RETRY;
		break;
	default:
		Utility::LogFatal(__FUNCTION__, messages, __LINE__);
		return RETURN_ERROR;
		break;
	}	
	delete [] buff;
	return RETURN_NO_ERROR;		
}

U08 CGPSDlg::PlRomNoAlloc2(const CString& prom_path)
{
	int buad = 0;
	buad = BAUDRATE;

	Param promTag;
	int extraSize = ParserBinFile(prom_path, &promTag);


	char messages[100] = {0};

	FILE *f = NULL;

	if(0 != fopen_s(&f, prom_path, "rb"))
	{
		return RETURN_ERROR;
	}

	fseek(f, 0, SEEK_END);

	long promLen = ftell(f) - extraSize;
	if(promTag.tagAddress==0)
	{
		promTag.tagAddress = (promLen>=0x70000) ? 0x000f7ffc : 0x00077ffc;
	}
	//Get preloader bin and it's size.
	CString externalPreloaderPath;
	BinaryData preLoader;
	int bootLoaderSize = 0;
	if(theApp.CheckExternalPreloader(externalPreloaderPath))
	{
		bootLoaderSize = preLoader.ReadFromFile(externalPreloaderPath);
	}
	else
	{
		bootLoaderSize = preLoader.ReadFromResource(GetBinFromResource(GetBaudrate()), "BIN");
	}

	const U08* sData = preLoader.Ptr();
	U08 mycheck = 0;
	for(int i=0; i<preLoader.Size(); ++i)
	{
		mycheck += sData[i];
	}

	fseek(f, BOOTLOADER_SIZE, SEEK_SET);
	char c;
	for(int i=0; i<promLen-BOOTLOADER_SIZE; ++i)
	{
		fread(&c, 1, 1, f);
		mycheck = mycheck + c;
	}

	bool bResendbin = true;
	while(bResendbin)
	{
		unsigned long long check = promLen + mycheck + promTag.tagAddress;
		sprintf_s(messages, sizeof(messages), "BINSIZE = %d Checksum = %d %lld %lld ", promLen, mycheck, promTag.tagAddress, check);
		m_serial->SendData((U08*)messages, (U16)strlen(messages)+1);	
		Utility::Log(__FUNCTION__, messages, __LINE__);
		switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, BinSizeTimeout, &m_responseList))
		{
		case wlf_ok:
			bResendbin = false;
			break;
		case wlf_timeout:
			Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
			fclose(f);
			AfxMessageBox("Timeout: GPS device no response.");						
			return RETURN_ERROR;
			break;
		case wlf_error1:
			AfxMessageBox("Flash Wrong");	
			fclose(f);
			return RETURN_ERROR;
			break;
		case wlf_error3:
			AfxMessageBox("Format Error!");					
			fclose(f);
			return RETURN_ERROR;		        
			break;
		case wlf_error4:
			AfxMessageBox("Flash Test Failed!");				
			fclose(f);
			return RETURN_ERROR;		        
			break;
		case wlf_error5:
			AfxMessageBox("srec timeout!");	
			fclose(f);
			return RETURN_ERROR;		        
			break;
		case wlf_resendbin:
			Utility::Log(__FUNCTION__, messages, __LINE__);
			bResendbin = true;
			break;
		default:
			Utility::LogFatal(__FUNCTION__, messages, __LINE__);
			fclose(f);
			return RETURN_ERROR;		        
			break;
		}	//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))	
	}	//while(bResendbin)

	downloadTotalSize = BOOTLOADER_SIZE + promLen;
	downloadProgress = 0;
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_WAITROM_MSG);
	
	int result = RETURN_NO_ERROR;
#if(RESEND_DOWNLOAD==1)
	int retryCount = 3;
	do
	{
		fseek(f, BOOTLOADER_SIZE, SEEK_SET);
		result = SendRomBuffer3(sData, bootLoaderSize, f, promLen, (buad >= 6), m_psoftImgDlDlg);
	} while(result == RETURN_RETRY && (retryCount--) > 0);
#else
	result = SendRomBuffer(prom_buff, prom_size,(buad >= 6) ,&(m_psoftImgDlDlg->m_progress), &(m_psoftImgDlDlg->m_percent), &(m_psoftImgDlDlg->m_msg));
#endif
//---------------------------------------------------------------
	mycheck = 0;
	promLen = BOOTLOADER_SIZE;

	fseek(f, 0, SEEK_SET);
	for(int i=0; i<BOOTLOADER_SIZE; ++i)
	{
		fread(&c, 1, 1, f);
		mycheck = mycheck + c;
	}

	bResendbin = true;
	while(bResendbin)
	{
		unsigned long long check = promLen + mycheck + promTag.tagAddress;
		sprintf_s(messages, sizeof(messages), "BINSIZE = %d Checksum = %d %lld %lld ", promLen, mycheck, promTag.tagAddress, check);
		m_serial->SendData((U08*)messages, (U16)strlen(messages)+1);	
		Utility::Log(__FUNCTION__, messages, __LINE__);

		switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, BinSizeTimeout, &m_responseList))
		{
		case wlf_ok:
			bResendbin = false;
			break;
		case wlf_timeout:
			Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
			fclose(f);
			AfxMessageBox("Timeout: GPS device no response.");						
			return RETURN_ERROR;
			break;
		case wlf_error1:
			AfxMessageBox("Flash Wrong");	
			fclose(f);
			return RETURN_ERROR;
			break;
		case wlf_error3:
			AfxMessageBox("Format Error!");					
			fclose(f);
			return RETURN_ERROR;		        
			break;
		case wlf_error4:
			AfxMessageBox("Flash Test Failed!");				
			fclose(f);
			return RETURN_ERROR;		        
			break;
		case wlf_error5:
			AfxMessageBox("srec timeout!");	
			fclose(f);
			return RETURN_ERROR;		        
			break;
		case wlf_resendbin:
			Utility::Log(__FUNCTION__, messages, __LINE__);
			bResendbin = true;
			break;
		default:
			Utility::LogFatal(__FUNCTION__, messages, __LINE__);
			fclose(f);
			return RETURN_ERROR;		        
			break;
		}	//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))	
	}	//while(bResendbin)
// Alex wait
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_WAITROM_MSG);
//	sprintf_s(messages, sizeof(messages), "Waiting for Rom Writing...");
//	m_psoftImgDlDlg->m_msg.SetWindowText(messages);	
	
	result = RETURN_NO_ERROR;
	
#if(RESEND_DOWNLOAD==1)
	retryCount = 3;
	do
	{
		fseek(f, 0, SEEK_SET);
		result = SendRomBuffer3(NULL, bootLoaderSize, f, promLen, (buad >= 6), m_psoftImgDlDlg);
	} while(result == RETURN_RETRY && (retryCount--) > 0);
#else
	result = SendRomBuffer(prom_buff, prom_size,(buad >= 6) ,&(m_psoftImgDlDlg->m_progress), &(m_psoftImgDlDlg->m_percent), &(m_psoftImgDlDlg->m_msg));
#endif

//---------------------------------------------------------------
	fclose(f);
	BoostBaudrate(TRUE);

// Alex wait
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_ROMWRITINGOK_MSG);
//	m_psoftImgDlDlg->m_msg.SetWindowText("Rom Writing is OK!");	

	//free(prom_buff);
	return RETURN_NO_ERROR;		
}

U08 CGPSDlg::PlRomNoAllocV8(const CString& prom_path)
{
	Param promTag;
	int extraSize = ParserBinFile(prom_path, &promTag);
	char messages[100] = {0};

	FILE *f = NULL;

	if(0 != fopen_s(&f, prom_path, "rb"))
	{
		::AfxMessageBox("PROM bin file not found!");
		return RETURN_ERROR;
	}

	fseek(f, 0, SEEK_END);
	long promLen = ftell(f) - extraSize;
	//if(promTag.tagAddress==0)
	//{
	//	promTag.tagAddress = (promLen>=0x70000) ? 0x000f7ffc : 0x00077ffc;
	//}

	fseek(f, 0, SEEK_SET);
	U08 c;
	U08 mycheck = 0;
	for(int i=0; i<promLen; ++i)
	{
		fread(&c, 1, 1, f);
		mycheck = mycheck + (c & 0xff);
	}

	//舊版FW在Loader上傳完畢後會丟出END，但是新版改由Loader自己丟。所以使用舊版FW搭配新版LOADER，
	//會收到兩次END
	bool bResendbin = true;
	while(bResendbin)
	{
		U32 checkCode = 0;
		CString strBinsizeCmd;
		if(m_DownloadMode == InternalLoaderV8AddTag)
		{
			U32 tagAddress = 0xAAA56556;	//V8 tag address is 0xFCFFC ^ 0xAAAAAAAA
			U32 tagValue = 0x55555F54;	//Skytraq tag is 0x0A01 ^ 0x55555555
			if(m_customerId==OlinkStar)
			{
				tagValue = 0x55555E54;	//OLinkStar tag is 0x0B01 ^ 0x55555555
			}
			checkCode = promLen + mycheck + m_nDownloadBaudIdx + tagAddress + tagValue;
			strBinsizeCmd.Format("BINSIZ2 = %d %d %u %u %u %u ", promLen, mycheck, m_nDownloadBaudIdx,
									tagAddress, tagValue, checkCode);
		}
		//else if(??)
		//{
		//	checkCode = promLen + mycheck + m_nDownloadBaudIdx;
		//	strBinsizeCmd.Format("BINSIZ3 = %d %d %u %u ", promLen, mycheck, m_nDownloadBaudIdx, checkCode);
		//}
		else
		{
			checkCode = promLen + mycheck;
			strBinsizeCmd.Format("BINSIZE = %d Checksum = %d %u ", promLen, mycheck, checkCode);
		}

		m_serial->SendData((LPCSTR)strBinsizeCmd, strBinsizeCmd.GetLength() + 1);	
		Utility::Log(__FUNCTION__, messages, __LINE__);
		bool isEnd = false;
		do 
		{
			//For Andrew Test EON Flash, wait for 1 minute.
			//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, 60 * 1000, &m_responseList))
			switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, BinSizeTimeout, &m_responseList))
			{
			case wlf_end:
				isEnd = true;
				break;
			case wlf_ok:
				bResendbin = false;
				isEnd = false;
				break;
			case wlf_timeout:
				Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
				fclose(f);
				AfxMessageBox("Timeout: GPS device no response.");						
				return RETURN_ERROR;
				break;
			case wlf_error1:
				AfxMessageBox("Flash Wrong!");	
				fclose(f);
				return RETURN_ERROR;
				break;
			case wlf_error3:
				AfxMessageBox("Format Error!");					
				fclose(f);
				return RETURN_ERROR;		        
				break;
			case wlf_error4:
				AfxMessageBox("Flash Test Failed!");				
				fclose(f);
				return RETURN_ERROR;		        
				break;
			case wlf_error5:
				AfxMessageBox("srec timeout!");	
				fclose(f);
				return RETURN_ERROR;		        
				break;
			case wlf_resendbin:
				Utility::Log(__FUNCTION__, messages, __LINE__);
				bResendbin = true;
				isEnd = false;
				break;
			default:
				Utility::LogFatal(__FUNCTION__, messages, __LINE__);
				fclose(f);
				return RETURN_ERROR;		        
				break;
			}	//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))	
		} while(isEnd);
	}	//while(bResendbin)
// Alex wait
	downloadTotalSize = promLen;
	downloadProgress = 0;
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_WAITROM_MSG);
//	sprintf_s(messages, sizeof(messages), "Waiting for Rom Writing...");
//	m_psoftImgDlDlg->m_msg.SetWindowText(messages);	
	
	Sleep(500);	//for GPSDO
	int result = RETURN_NO_ERROR;
	if(RESEND_DOWNLOAD)
	{
		int retryCount = 1;
		do
		{
			fseek(f, 0, SEEK_SET);
			result = SendRomBuffer3(NULL, 0, f, promLen, false, m_psoftImgDlDlg);
		} while(result == RETURN_RETRY && (retryCount--) > 0);
	}
	fclose(f);
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_ROMWRITINGOK_MSG);
	return result;		
}

void CGPSDlg::GetLoaderDownloadCmd(char* msg, int size)
{
////	if(IS_DEBUG && (GetAsyncKeyState(VK_LSHIFT) & 0x8000) && (GetAsyncKeyState(VK_LMENU)& 0x8000))
//	{	//In Internal Use version, user can press L-Shift + L-Alt to restore GG12A to standard version.
//		sprintf_s(msg, size, "#DOWNLOAD LOADER");		
//	}
//	else
//	{
//		sprintf_s(msg, size, "$LOADER DOWNLOAD");		
//	}
	if(customerCrc==0x00A0  || customerCrc==0x0724 || customerCrc==0x6469)
	{
			sprintf_s(msg, size, "$OLINKSTAR IMAGE");		
	}
	else
	{
			sprintf_s(msg, size, "$LOADER DOWNLOAD");		
	}

}

bool CGPSDlg::DownloadLoader()
{
	ScopeTimer t("DownloadLoader()");
	const int bufferSize = 256;
	char messages[100] = {0};

	if(m_DownloadMode != EnternalLoaderInBinCmd)
	{
		GetLoaderDownloadCmd(messages, sizeof(messages));

		const int retryCount = 3;
		for (int i=1; i<retryCount; i++)
		{
			SendToTargetNoAck((U08*)messages, (U16)strlen(messages) + 1);
			Sleep(300);

			switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))
			{
			case wlf_timeout:
				AfxMessageBox("Target doesn't reply, please power cycle the target!");				
				return false;
				break;
			case wlf_ok:
				i = retryCount;
				break;
			default:
				Utility::LogFatal(__FUNCTION__, messages, __LINE__);
				break;
			}	
		}
	}
	else
	{
		U08 msg[7] = {0};
		msg[0] = 0x64;
		msg[1] = 0x1B;
		msg[2] = (U08)m_nDownloadBaudIdx;
		msg[3] = (U08)0;
		msg[4] = (U08)0;
		msg[5] = (U08)0;
		msg[6] = (U08)m_nDownloadBufferIdx;
		int len = SetMessage(msg, sizeof(msg));

		//bool b = SendToTarget(m_inputMsg, len, "Send upload loader Successful");
		bool b = false;
		if(!b)
		{
			m_DownloadMode = EnternalLoader;
			BoostBaudrate(FALSE);
			//add_msgtolist("Upload Loader command NACK.");	
			return DownloadLoader();
		}
		else
		{
			CloseOpenUart();
			m_serial->ResetPort(m_nDownloadBaudIdx);
			m_BaudRateCombo.SetCurSel(m_nDownloadBaudIdx);
			Sleep(200);
		}
	}

// Alex wait
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_FLASHWRITINGOK_MSG);
//	sprintf_s(messages, sizeof(messages), "Waiting for Flash Loading...\r\n");
//	m_psoftImgDlDlg->m_msg.SetWindowText(messages);	
	
	BinaryData srec;
	CString externalSrecFile;
	theApp.CheckExternalSrec(externalSrecFile);

	if(EnternalLoaderInBinCmd==m_DownloadMode || EnternalLoader==m_DownloadMode)
	{
		srec.ReadFromFile(externalSrecFile);
	}
	else
	{
		UINT loaderID = GetSrecFromResource(m_nDownloadBaudIdx);
		if(!loaderID)
		{
			AfxMessageBox("Can not find the specified Loader!");
			return false;
		}
		srec.ReadFromResource(loaderID, "SREC");
	}

	const U08* sData = srec.Ptr();
	long leftSize = srec.Size();
	int needdelay = 0;
	int totalByte = 0;
	char buff[bufferSize] = {0};

	while(leftSize>0)
	{
		int ProgressPos = 0;
		int packetSize = 0;
		const U08 *tmp = sData;

		memset(buff, 0, sizeof(buff));
		while(1)
		{
			++packetSize;
			if(*tmp == '\n')
			{
				break;
			}
			++tmp;
		}
		memcpy(buff, sData, packetSize - 1);
		buff[packetSize - 2] = 0x0a;
		SendToTargetNoAck((U08*)buff, packetSize);
/*
		if(buad >= 6)
		{
			needdelay++;
			if(needdelay==5)
			{
				needdelay = 0;
				Sleep(1);
			}
		}
*/	
		leftSize -= packetSize;
		sData += packetSize;
		totalByte += packetSize;//deduct by end of string character in sending
//* Alex wait
		m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, totalByte, srec.Size());
	}			

	memset(buff, 0, sizeof(buff));
	switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))
	{
	case wlf_end:
		sprintf_s(messages, sizeof(messages), "The total bytes transferred = %d", totalByte);
		add_msgtolist(messages);	
		break;
	case wlf_timeout:
		Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
		return false;
		break;
	default:
		Utility::LogFatal(__FUNCTION__, messages, __LINE__);
		break;
	}	
	m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, 100, 100);


// Alex wait
//	sprintf_s(messages, sizeof(messages), "The total bytes transferred = %d", totalByte);
//	m_psoftImgDlDlg->m_msg.SetWindowText(messages);	
//	m_psoftImgDlDlg->m_msg.SetWindowText("Flash loading is OK!");
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_FLASHWRITINGOK_MSG);

	switch(m_DownloadMode)
	{
		case ParallelDownloadType0:
		case ParallelDownloadType1:
		case InternalLoaderV6Gps:
		case InternalLoaderV6Gnss:
		case InternalLoaderV6Gg12a:
		case InternalLoaderV6GpsAddTag:
		case InternalLoaderV6GpsDelTag:
		case InternalLoaderV6GnssAddTag:
		case InternalLoaderV6GnssDelTag:
		case HostBasedDownload:
		case HostBasedCmdOnly:
			Sleep(1000);
			break;
		case EnternalLoader:
		case EnternalLoaderInBinCmd:
		case InternalLoaderV8:
		case InternalLoaderV8AddTag:
		case InternalLoaderSpecial:
			break;
		default:
			ASSERT(FALSE);
	}

	return true;
}

BOOL VarifyPassword(const CString& pwd)
{
	if(!_CREATE_LICENSE_TAG_)
	{
		return true;
	}

	const char ConvertTabe[] = "0123456789abcdfghijkmnpqrstuvwyz";
	CString key, binPart, answer;
	CTime currentTime = CTime::GetCurrentTime();
	key.Format("SkytraqTagGPSViewer%d%d", currentTime.GetYear() - 2000, (currentTime.GetMonth() - 1) / 3);
	CString shaData = CryptHash::HashStringSHA1(key);

	for(int i = shaData.GetLength()-1; i>0; i-=3)
	{
		binPart = shaData[i - 1];
		binPart += shaData[i];
		answer += ConvertTabe[strtol(binPart, NULL, 16) % strlen(ConvertTabe)];
	}
	return 0 == answer.CompareNoCase(pwd);
}

bool CGPSDlg::QueryPassword()
{

	if(!_CREATE_LICENSE_TAG_)
	{
		return true;
	}
#ifdef _DEBUG
		return true;
#endif
	CInputPassword dlg;
	CString pwInput;
	INT_PTR nResponse = dlg.DoModal();
	if(nResponse==IDCANCEL)
	{
		return false;
	}

	if(!VarifyPassword(dlg.GetPassword()))
	{
		return false;
	}
	return true;
}

UINT ShowDownloadProgressThread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->m_psoftImgDlDlg->DoModal();
	delete CGPSDlg::gpsDlg->m_psoftImgDlDlg;
	CGPSDlg::gpsDlg->m_psoftImgDlDlg = NULL;
	return 0;	
}

const int SpiBaudIndex = 9;
void CGPSDlg::Download()
{
	if(!QueryPassword())
	{	//Password authentication failed!
		SetMode();
		CreateGPSThread();
		return;
	}

	do 
	{
		customerCrc = 0;
		if(_CREATE_LICENSE_TAG_ && m_DownloadMode==InternalLoaderV8)
		{
			m_DownloadMode = InternalLoaderV8AddTag;
		}

		U16 crcCode = 0;
		if(m_DownloadMode != EnternalLoader && CUSTOMER_DOWNLOAD && m_customerId==OlinkStar)
		{
			if(Ack == QuerySoftwareCrcSystemCode(Return, &crcCode))
			{
				if(crcCode==0xD3CF || crcCode==0x00A0  || crcCode==0x0724 || crcCode==0x6469)
				{	//The firmware which released in 2013/10/24, 2013/11/11, 2013/11/08, 2013/11/07 
					//for OLinkStar should be added tag.
					m_DownloadMode = InternalLoaderV8AddTag;
					customerCrc = crcCode;
				}
			}
		}
	//Test
	//m_DownloadMode = InternalLoaderV8AddTag;

		bool isSuccessful = true;
		ScopeTimer t("Download()");

		switch(m_DownloadMode)
		{
			case EnternalLoader:
			case InternalLoaderV6Gps:
			case InternalLoaderV6Gnss:
			case InternalLoaderV6Gg12a:
			case InternalLoaderV6GpsAddTag:
			case InternalLoaderV6GpsDelTag:
			case InternalLoaderV6GnssAddTag:
			case InternalLoaderV6GnssDelTag:
			case ParallelDownloadType0:
			case ParallelDownloadType1:
			case InternalLoaderV8AddTag:
			case InternalLoaderSpecial:
				BoostBaudrate(FALSE);
			case EnternalLoaderInBinCmd:
			{
				//1. Boost baud rate 2. Download Loader 
				m_psoftImgDlDlg = new CSoftImDwDlg;	
				AfxBeginThread(ShowDownloadProgressThread, 0); 
				if(!DownloadLoader())
				{
					add_msgtolist("Software Image Download Failed...");	 
					::AfxMessageBox("Load Loader into flash failed!");
					m_psoftImgDlDlg->SetFinish(true);	
					BoostBaudrate(TRUE);
					SetMode();
					CreateGPSThread();
					return;
				}
				break;
			}
			case HostBasedDownload:
			case HostBasedCmdOnly:
			case InternalLoaderV8:
			case CustomerDownload:
			{	//Using command to initial loader.
				U08 msg[9] = {0};
				int cmdSize = 0;	
				if(m_DownloadMode==InternalLoaderV8)
				{
					msg[0] = 0x0B;
					msg[1] = (U08)m_nDownloadBaudIdx;
					msg[2] = (U08)0;
					msg[3] = (U08)0;
					msg[4] = (U08)0;
					msg[5] = (U08)m_nDownloadBufferIdx;
					cmdSize = 6;
				}
				else if(m_DownloadMode==CustomerDownload)
				{
					msg[0] = 0x64;
					msg[1] = 0x7F;
					msg[2] = HIBYTE(m_customerId);
					msg[3] = LOBYTE(m_customerId);
					msg[4] = 0;
					msg[5] = (U08)m_nDownloadBaudIdx;
					msg[6] = 0;
					msg[7] = 0;
					msg[8] = 0;
					cmdSize = 9;

					m_DownloadMode = InternalLoaderV8;
				}
				else
				{
					msg[0] = 0x64;
					msg[1] = 0x10;
					msg[2] = (U08)m_nDownloadBaudIdx;
					msg[3] = (U08)m_nDownloadBufferIdx;
					cmdSize = 4;
				}

				int len = SetMessage(msg, cmdSize);
				if(!SendToTarget(CGPSDlg::m_inputMsg, len, "Loader start..."))
				{
					isSuccessful = false;
				}
				else if(m_nDownloadBaudIdx < SpiBaudIndex)
				{
					CloseOpenUart();
					m_serial->ResetPort(m_nDownloadBaudIdx);
					m_BaudRateCombo.SetCurSel(m_nDownloadBaudIdx);
					Sleep(200);
				}
				break;
			}
	/*
			case ParallelDownloadType1:
			{	//Using command to initial loader.
				U08 msg[6] = {0};
				int cmdSize = 0;	
	//			if(m_DownloadMode==InternalLoaderV8)
				{
					msg[0] = 0x0B;
					msg[1] = (U08)m_nDownloadBaudIdx;
					msg[2] = (U08)3;
					msg[3] = (U08)0x89;
					msg[4] = (U08)0x19;
					msg[5] = (U08)m_nDownloadBufferIdx;
					cmdSize = 6;
				}

				int len = SetMessage(msg, cmdSize);
				if(!SendToTarget(CGPSDlg::m_inputMsg, len, "Loader start..."))
				{
					isSuccessful = false;
				}
				else if(m_nDownloadBaudIdx!=SpiBaudIndex)
				{
					CloseOpenUart();
					m_serial->ResetPort(m_nDownloadBaudIdx);
					m_BaudRateCombo.SetCurSel(m_nDownloadBaudIdx);
					Sleep(200);
				}
				break;
			}
	*/
			default:
				ASSERT(FALSE);
		}

		if(m_DownloadMode==HostBasedCmdOnly)
		{
			return;
		} 

		if(isSuccessful)
		{
			if(NULL==m_psoftImgDlDlg)
			{
				m_psoftImgDlDlg = new CSoftImDwDlg;	
				AfxBeginThread(ShowDownloadProgressThread, 0); 
			}
	//		Sleep(5000);
			Sleep(g_setting.delayBeforeBinsize);
			isSuccessful = FirmwareUpdate();
			if(isSuccessful)
			{
				CString strMsg;
				strMsg.Format("Download total time %d seconds.", t.GetDuration() / 1000);
				add_msgtolist(strMsg);
				DeleteNmeaMemery();
				m_firstDataIn = false;
				m_inputMode = NMEA_Mode;	
				SetMsgType(NMEA_Mode);
			}
			else
			{
				add_msgtolist("Software Image Download Failed...");
				::AfxMessageBox("Download failed!");
			}

		}
		else
		{
			add_msgtolist("Software Image Download Failed...");
			::AfxMessageBox("Download failed!");
		}

	#if (WITH_CONFIG_USB_BAUDRATE && GNSS_VIEWER)
		if(m_rom_mode.GetCheck())
		{
			CP210x_SetBaudRateConfig(m_serial->GetHandle(), m_DefaultBaudConfigData);
			m_rom_mode.SetCheck(0);
		}
	#endif
		if(m_psoftImgDlDlg)
		{
			m_psoftImgDlDlg->SetFinish(true);	
		}


		BoostBaudrate(TRUE);


		if(g_setting.downloadTesting)
		{
			Sleep(3000);
		}
	} while(g_setting.downloadTesting);

	SetMode();
	m_firstDataIn = false;
	ClearInformation();
	CreateGPSThread();
}

