#include "kernel/file.h"
#include "kernel/task.h"
#include "kernel/taskManager.h"
#include "fcntl.h"
#include "unistd.h"


IFileIO * pKeyboard(0);
IFileIO * pVideo(0);


// -----------------------------------------------------------------------------
extern "C" int
open(const char * pPath, int iOflags, ...)
{
  int iFD(-1);
/*
  CFileHandle * pFile(0);
  
  // Find free file handle entry
  for(iFD = 0; iFD != MAX_FILES; iFD++)
  {
    if(CTaskManager::instance()->getCurrentTask()->pFiles_[iFD] == 0)
    {
      break;
    }
  }

  // Validate file descriptor
  if(iFD == MAX_FILES)
  {
    return(-1);
  }


  // Get file pointer
//  pFile = pFileManager->openFile(pPath, iOflags, ...);


  // Validate file pointer
  if(pFile == 0)
  {
    return(-1);
  }

  // Add file handle pointer to current task
  CTaskManager::instance()->getCurrentTask()->pFiles_[iFD] = pFile;
*/
  return(iFD);
}

// -----------------------------------------------------------------------------
extern "C" int
close(int iFD)
{
//  return(CTaskManager::instance()->getCurrentTask()->fileClose(iFD));

  return(-1);
}

// -----------------------------------------------------------------------------
extern "C" ssize_t
read(int iFD, void * pBuf, size_t size)
{
//  return(CTaskManager::instance()->getCurrentTask()->file_[iFD].read(pBuf, size));

  if(iFD == STDIN)
  {
    if(pKeyboard != 0)
      return(pKeyboard->read(pBuf, size));
    else
      return(-1);
  }
  else
  {
    return(-1);
  }
}

// -----------------------------------------------------------------------------
extern "C" ssize_t
write(int iFD, const void * pBuf, size_t size)
{
//  return(CTaskManager::instance()->getCurrentTask()->file_[iFD].write(pBuf, size));

  if(iFD == STDOUT || iFD == STDERR)
  {
    if(pVideo != 0)
      return(pVideo->write(pBuf, size));
    else
      return(-1);
  }
  else
  {
    return(-1);
  }
}
