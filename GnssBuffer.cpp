#include "StdAfx.h"
#include "GnssBuffer.h"

GnssBuffer::GnssBuffer(void)
{
  Initial(InitialSize);
}

GnssBuffer::GnssBuffer(int size)
{
  Initial(size);
}

GnssBuffer::~GnssBuffer(void)
{
  DeleteCriticalSection(&rwCs);
  SafelyDelArray(pBuffer);
}

void GnssBuffer::Initial(int size)
{
  InitializeCriticalSection(&rwCs);
  nBufferSize = size;
  pBuffer = new U08[size];
  ClearBuffer();
}

void GnssBuffer::ClearBuffer()
{
  EnterCriticalSection(&rwCs);
  nReadPtr = 0;
  nWritePtr = 0;
  full = false;
  LeaveCriticalSection(&rwCs);
}
bool GnssBuffer::IsFull() const
{
  return (full && (nReadPtr == nWritePtr));
}

bool GnssBuffer::IsReadable() const
{
  return (full || (nReadPtr != nWritePtr));
}

int GnssBuffer::ReadOneChar()
{
  if(!IsReadable())
  {
    return ReadError;
  }
  EnterCriticalSection(&rwCs);
  ASSERT(nWritePtr < nBufferSize && nReadPtr < nBufferSize);
  U08 c = pBuffer[nReadPtr++];
  nReadPtr = nReadPtr % nBufferSize;
  if(full)
  {
    full = false;
  }
  LeaveCriticalSection(&rwCs);
  return c;
}

int GnssBuffer::GetWritableSize() const
{
  if(full)
  {
    return 0;
  }
  return (nWritePtr >= nReadPtr) ? (nBufferSize - nWritePtr) : (nReadPtr - nWritePtr);
}

int GnssBuffer::GetTotalFreeSize() const
{
  if(full)
  {
    return 0;
  }
  return (nWritePtr >= nReadPtr) ? (nBufferSize - nWritePtr + nReadPtr) : (nReadPtr - nWritePtr);
}

int GnssBuffer::GetDataSize() const
{
  return nBufferSize - GetTotalFreeSize();
}

U08* GnssBuffer::GetWritePtr()
{
  EnterCriticalSection(&rwCs);
  return pBuffer + nWritePtr;
}

void GnssBuffer::SetWritePtr(int writeBytes)
{
  ASSERT(nWritePtr + writeBytes <= nBufferSize);
  ASSERT((nWritePtr >= nReadPtr) || (nWritePtr + writeBytes <= nReadPtr));
  nWritePtr = (nWritePtr + writeBytes) % nBufferSize;
  if(nWritePtr == nReadPtr)
  {
    full = true;
  }
  LeaveCriticalSection(&rwCs);
}