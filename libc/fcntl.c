#include "fcntl.h"
#include "stdarg.h"
#include "kernel/srr.h"


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
