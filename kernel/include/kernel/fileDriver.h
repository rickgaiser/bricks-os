#ifndef KERNEL_FILEDRIVER_H
#define KERNEL_FILEDRIVER_H


#include "kernel/srrChannel.h"


#define MAX_OPEN_FILES 10


// -----------------------------------------------------------------------------
enum EFileCommand
{
  FC_OPEN  = 1,
  FC_CLOSE = 2,
  FC_READ  = 3,
  FC_WRITE = 4,
};

// -----------------------------------------------------------------------------
struct SFileHeader
{
  uint32_t command;
};

// -----------------------------------------------------------------------------
struct SFileOpenHeader
{
  SFileHeader commonHeader;
  uint32_t flags;
};

// -----------------------------------------------------------------------------
struct SFileCloseHeader
{
  SFileHeader commonHeader;
};

// -----------------------------------------------------------------------------
struct SFileReadHeader
{
  SFileHeader commonHeader;
  uint32_t size; // Max Read Size
};

// -----------------------------------------------------------------------------
struct SFileWriteHeader
{
  SFileHeader commonHeader;
  uint32_t size; // Max Write Size
};

// -----------------------------------------------------------------------------
class CKernelFileDriver
 : public CAChannelKernel
{
public:
  CKernelFileDriver(IFileIO & file, const char * name);
  virtual ~CKernelFileDriver();

  virtual int msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);

private:
  IFileIO & file_;
  uint32_t flags_[MAX_OPEN_FILES];
};


#endif