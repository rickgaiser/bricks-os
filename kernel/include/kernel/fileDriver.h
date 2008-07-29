#ifndef KERNEL_FILEDRIVER_H
#define KERNEL_FILEDRIVER_H


#include "kernel/srrChannel.h"


#define MAX_OPEN_FILES 10


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
