#include "fcntl.h"
#include "kernel/srr.h"


#define va_list         __builtin_va_list
//#define va_start(v,l)   __builtin_va_start(v,l)
#define va_start(v,l)   __builtin_stdarg_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)


// -----------------------------------------------------------------------------
int
vopen(const char * path, int oflag, va_list ap)
{
  int iRetVal, iNID = 0, iPID, iChannelID;

  iRetVal = lookupName(path, &iPID, &iChannelID);

  if(iRetVal >= 0)
    iRetVal = channelConnectAttach(iNID, iPID, iChannelID, 0);

  return iRetVal;
}

// -----------------------------------------------------------------------------
int
open(const char * path, int oflag, ...)
{
  va_list ap;
  int     fd;

  va_start(ap, oflag);
  fd = vopen(path, oflag, ap);
  va_end(ap);

  return fd;
}
