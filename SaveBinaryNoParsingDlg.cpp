// SaveBinaryNoParsingDlg.cpp
//

#include "stdafx.h"
#include "SaveBinaryNoParsingDlg.h"
#include "Resource.h"
#include "GPSDlg.h"
#include "Serial.h"

// BufferBlock 
bool BufferBlock::WriteBlock(U08* ptr, int size)
{
  bool changeBlock = false;
  DWORD t = ::GetTickCount() - timeTick;
  if(BufferSize - bufferIndex[writePtr] < size || ( t > 1000))
  {
    timeTick = ::GetTickCount();
    NextWritePtr();
    changeBlock = true;
    ASSERT(bufferIndex[writePtr] == 0);
  }

  memcpy(bufferBlcok[writePtr] + bufferIndex[writePtr], ptr, size);
  bufferIndex[writePtr] += size;
  return changeBlock;
}

// SaveBinaryNoParsingDlg 
IMPLEMENT_DYNAMIC(SaveBinaryNoParsingDlg, CDialog)

SaveBinaryNoParsingDlg::SaveBinaryNoParsingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SAVE_BINARY_NOPARSING, pParent)
{
  stopThreadBufferWrite = FALSE;
  stopThreadComRead = FALSE;
  threadBufferWrite = NULL;
  threadComRead = NULL;
  noAckAlerm = 3;
}

SaveBinaryNoParsingDlg::~SaveBinaryNoParsingDlg()
{
}

void SaveBinaryNoParsingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SaveBinaryNoParsingDlg, CDialog)
  ON_WM_TIMER()
  ON_WM_DESTROY()
END_MESSAGE_MAP()

UINT SaveBinaryNoParsingDlg::ThreadComRead(LPVOID pParam)
{
  return ((SaveBinaryNoParsingDlg*)pParam)->ComRead();
}

UINT SaveBinaryNoParsingDlg::ThreadBufferWrite(LPVOID pParam)
{
  return ((SaveBinaryNoParsingDlg*)pParam)->BufferWrite();
}
const UINT DisplayInfoTimerId = 1;

// SaveBinaryNoParsingDlg 
BOOL SaveBinaryNoParsingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
  GetDlgItem(IDC_FILENAME)->SetWindowText(filePath);
  CFile f;
  if(!f.Open(filePath, CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite))
  {
    ASSERT(FALSE);
    ::AfxMessageBox("File open fail!");
    CDialog::OnOK();
  }
  f.Write("\r\n", 2);
  f.Close();

  threadComRead = AfxBeginThread(ThreadComRead, (LPVOID) this, THREAD_PRIORITY_ABOVE_NORMAL);
  threadBufferWrite = AfxBeginThread(ThreadBufferWrite, (LPVOID) this, THREAD_PRIORITY_NORMAL);

  startTime = CTime::GetCurrentTime();
  SetTimer(DisplayInfoTimerId, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void SaveBinaryNoParsingDlg::OnOK()
{

	CDialog::OnOK();
}

BOOL ParsingHotStartAck(U08* buf, DWORD size)
{
  //Ack: a0 a1 00 02 83 01 82 0d 0a 
  const U08 hotStartAck[] = { 0xA0, 0xA1, 0x00, 0x02, 0x83, 0x01, 0x82, 0x0d, 0x0a };
  static int findIndex = 0;

  if(buf == NULL)
  {
    findIndex = 0;
    return FALSE;
  }

  for(DWORD i = 0; i < size; ++i)
  {
    if(buf[i] == hotStartAck[findIndex])
    {
      ++findIndex;
    }
    else
    {
      findIndex = 0;
    }
    if(findIndex == sizeof(hotStartAck))
    {
      return TRUE;
    }
  }
  return FALSE;
}

UINT SaveBinaryNoParsingDlg::ComRead()
{
	CString portName;
	portName.Format("\\\\.\\COM%d", comPort);
  HANDLE comHandle = ::CreateFile(portName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
  if(comHandle == INVALID_HANDLE_VALUE)
  {
    DWORD err = GetLastError();
  }

  DCB config = {0};
  BOOL b = ::GetCommState(comHandle, &config);
  config.BaudRate = baudrate;
  config.StopBits = ONESTOPBIT;
  config.Parity = NOPARITY; 
  config.ByteSize = 8;
  config.fDtrControl = 0;
  config.fRtsControl = 0;
  b = ::SetCommState(comHandle, &config);

  COMMTIMEOUTS ct = {0};
  b = ::GetCommTimeouts(comHandle, &ct);
  ct.ReadIntervalTimeout = MAXDWORD;
  ct.ReadTotalTimeoutMultiplier = 0;  
  ct.ReadTotalTimeoutConstant = 0; 
  b = ::SetCommTimeouts(comHandle, &ct);
  SetupComm(comHandle, 128 * 1024, 1024);
  //Send HotStart
  BinaryData binData(15);
  *(binData.GetBuffer(0)) = 1;
  *(binData.GetBuffer(1)) = 1;
  BinaryCommand binCmd(binData);
  DWORD dwBytesRead = 0;
  b = ::WriteFile(comHandle, binCmd.GetBuffer(), 22, &dwBytesRead, 0);  
  bool hotStartAck = false;
  static U08 readBuffer[1024 * 16] = { 0 };
  while(!stopThreadComRead)
  {
    b = ::ReadFile(comHandle, readBuffer, sizeof(readBuffer), &dwBytesRead, NULL);
    if(b == FALSE)
    {
      break;
    }
    if(dwBytesRead == 0)
    {
      ++buffer.sleepCount;
      Sleep(1);
      continue;
    }
    ++buffer.readCount;
    if(buffer.maxReadSize < (int)dwBytesRead)
    {
      buffer.maxReadSize = (int)dwBytesRead;
    }
    if(!hotStartAck)
    {
      if(ParsingHotStartAck(readBuffer, dwBytesRead))
      {
        ParsingHotStartAck(NULL, 0);
        hotStartAck = TRUE;
        buffer.alreadyAck = TRUE;
      }
    }
    buffer.WriteBlock(readBuffer, dwBytesRead);
  }
	CloseHandle(comHandle);
	return 0;
}

UINT SaveBinaryNoParsingDlg::BufferWrite()
{
  CFile f;
  while(!stopThreadBufferWrite)
  {
    if(!buffer.NeedWrite())
    {
      Sleep(10);
      continue;
    }
    int size;
    const U08* wPtr = buffer.ReadBlock(buffer.GetReadPtr(), &size);
    char dbgStr[256];
    if(size == 0)
    {
      buffer.NextReadPtr();
      continue;
    }
    sprintf_s(dbgStr, 256, "bufferIndex:%d,%d,%d,%d\r\n", 
      buffer.bufferIndex[0], buffer.bufferIndex[1], buffer.bufferIndex[2], buffer.bufferIndex[3]);
    ::OutputDebugString(dbgStr);
    sprintf_s(dbgStr, 256, "W/R:%d/%d,mW:%d,mB:%d,mR:%d,sC:%d,rC:%d,Ta:%llu\r\n", buffer.GetWritePtr(), buffer.GetReadPtr(),
      buffer.maxWriteSize, buffer.maxBlockSize, buffer.maxReadSize, buffer.sleepCount, buffer.readCount, buffer.totalWriteSize);
    ::OutputDebugString(dbgStr);

    if(!f.Open(filePath, CFile::modeReadWrite | CFile::shareDenyWrite))
    {
      ASSERT(FALSE);
    }
    f.SeekToEnd();
    f.Write(wPtr, size);
    buffer.NextReadPtr();
    f.Close();
  }
  return 0;
}


void SaveBinaryNoParsingDlg::OnDestroy()
{
  if(threadComRead != NULL)
  {
    stopThreadComRead = TRUE;
    ::WaitForSingleObject(threadComRead, INFINITE);
  }
  if(threadBufferWrite != NULL)
  {
    stopThreadBufferWrite = TRUE;
    ::WaitForSingleObject(threadBufferWrite, INFINITE);
  }

}

void SaveBinaryNoParsingDlg::ShowSize(ULONGLONG size, UINT id)
{
  char buf[128];
  if(size > 1024 * 1024 * 1024)
  {
    sprintf_s(buf, 128, "%llu / %.3f GB", size, ((double)size) / (1024 * 1024 * 1024));
  }
  else if(size > 1024 * 1024)
  {
    sprintf_s(buf, 128, "%llu / %.2f MB", size, ((double)size) / (1024 * 1024));
  }
  else if(size > 1024)
  {
    sprintf_s(buf, 128, "%llu / %.1f kB", size, ((double)size) / (1024));
  }
  else
  {
    sprintf_s(buf, 128, "%d Bytes", (int)size);
  }
  GetDlgItem(id)->SetWindowText(buf);
}

void SaveBinaryNoParsingDlg::ShowDuration(CTimeSpan ts, UINT id)
{
  char buf[128];
  //CTimeSpan ts = CTime::GetCurrentTime() - startTime;
  if(ts.GetDays() > 0)
  {
    sprintf_s(buf, 128, "%d day(s) and %02d:%02d:%02d", (int)ts.GetDays(), (int)ts.GetHours(), (int)ts.GetMinutes(), (int)ts.GetSeconds());
  }
  else
  {
    sprintf_s(buf, 128, "%02d:%02d:%02d", (int)ts.GetHours(), (int)ts.GetMinutes(), (int)ts.GetSeconds());
  }
  GetDlgItem(id)->SetWindowText(buf);
}

void SaveBinaryNoParsingDlg::OnTimer(UINT_PTR nIDEvent)
{
  char drive[3] = {0};
  drive[0] = filePath[0];
  drive[1] = filePath[1];
  CTimeSpan ts = CTime::GetCurrentTime() - startTime;
  ShowDuration(ts, IDC_DURATION);
  ShowSize(buffer.totalWriteSize, IDC_TOTAL_SIZE);

  ULARGE_INTEGER p1;
  if(GetDiskFreeSpaceEx(drive, &p1, NULL, NULL))
  {
    ShowSize(p1.QuadPart, IDC_TOTAL_FREE);
    if(ts.GetTotalSeconds() > 5 && buffer.totalWriteSize > 0)
    {
      int estSec = (int)(p1.QuadPart / (buffer.totalWriteSize / ts.GetTotalSeconds()));
      //IDC_EST_TIME
      CTimeSpan estTs(0, 0, 0, estSec);
      ShowDuration(estTs, IDC_EST_TIME);
    }
  }

  if(buffer.alreadyAck)
  {
    GetDlgItem(IDC_PROMPT)->SetWindowText("HostLog starts.");
  }
  else
  {
    if(--noAckAlerm == 0)
    {
      KillTimer(DisplayInfoTimerId);
      ::AfxMessageBox("HostLog restart timeout!");
      SetTimer(DisplayInfoTimerId, 1000, NULL);
    }
  }
  CDialog::OnTimer(nIDEvent);
}