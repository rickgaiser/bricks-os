#include "unistd.h"
#include "kernel/syscall.h"
#include "kernel/srr.h"
#include "string.h"


//sysCallUser1r(int,             brk, void *, addr);
//sysCallUser1r(int,             close, int, iFD);
//sysCallUser1 (                 _exit, int, iStatus);
sysCallUser0r(pid_t,           getpid);
//sysCallUser3r(ssize_t,         read, int, iFD, void *, pBuf, size_t, size);
//sysCallUser1r(char *,          sbrk, intptr_t, increment);
sysCallUser1r(unsigned int,    sleep, unsigned int, iSeconds);
sysCallUser1r(int,             usleep, useconds_t, useconds);
//sysCallUser3r(ssize_t,         write, int, iFD, const void *, pBuf, size_t, size);


// -----------------------------------------------------------------------------
int
close(int fd)
{
  return channelConnectDetach(fd);
}

// -----------------------------------------------------------------------------
ssize_t
read(int fd, void * buffer, size_t size)
{
  ssize_t iRetVal(0);

  uint32_t iMsgSize = sizeof(SFileReadHeader);
  uint8_t * pMessage = new uint8_t[iMsgSize];
  SFileReadHeader * pHeader = (SFileReadHeader *)pMessage;

  pHeader->commonHeader.command = FC_READ;
  pHeader->size = size;

  iRetVal = msgSend(fd, pMessage, iMsgSize, buffer, size);

  delete pMessage;

  return iRetVal;
}

// -----------------------------------------------------------------------------
ssize_t
write(int fd, const void * buffer, size_t size)
{
  ssize_t iRetVal(0);

  uint32_t iMsgSize = size + sizeof(SFileWriteHeader);
  uint8_t * pMessage = new uint8_t[iMsgSize];
  SFileWriteHeader * pHeader = (SFileWriteHeader *)pMessage;
  uint8_t * pData = pMessage + sizeof(SFileWriteHeader);

  pHeader->commonHeader.command = FC_WRITE;
  pHeader->size = size;
  memcpy(pData, buffer, size);

  iRetVal = msgSend(fd, pMessage, iMsgSize, 0, 0);

  delete pMessage;

  return iRetVal;
}
