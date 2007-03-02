#include "kernel/fs.h"
#include "kernel/task.h"
#include "fcntl.h"
#include "unistd.h"


// -----------------------------------------------------------------------------
extern "C" int
open(const char * pPath, int iOflags, ...)
{
  return -1;
}

// -----------------------------------------------------------------------------
extern "C" int
close(int iFD)
{
  return -1;
}

// -----------------------------------------------------------------------------
extern "C" ssize_t
read(int iFD, void * pBuf, size_t size)
{
  int iRetVal(-1);

  if((iFD >= 0) && (iFD < MAX_FILE_COUNT))
  {
    IFileIO * pFile = CTask::pCurrentTask_->pFiles_[iFD];
    if(pFile != 0)
      iRetVal = pFile->read(pBuf, size);
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
extern "C" ssize_t
write(int iFD, const void * pBuf, size_t size)
{
  int iRetVal(-1);

  if((iFD >= 0) && (iFD < MAX_FILE_COUNT))
  {
    IFileIO * pFile = CTask::pCurrentTask_->pFiles_[iFD];
    if(pFile != 0)
      iRetVal = pFile->write(pBuf, size);
  }

  return iRetVal;
}
