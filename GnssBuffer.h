#pragma once

class GnssBuffer
{
public:
  //enum { InitialSize = 128 * 1024 };
  enum { InitialSize = 128 * 1024 };
  enum { ReadError = -1 };
  GnssBuffer(void);
  GnssBuffer(int size);
  ~GnssBuffer(void);

  void ClearBuffer();
  bool IsFull() const;
  bool IsReadable() const;
  int ReadOneChar();
  int GetWritableSize() const;
  int GetTotalFreeSize() const;
  int GetDataSize() const;
  U08* GetWritePtr();
  void SetWritePtr(int writeBytes);

protected:
  U08* pBuffer;
  int nWritePtr;
  int nReadPtr;
  int nBufferSize;
  bool full;  
  CRITICAL_SECTION rwCs;

  void Initial(int size);
};
