#include "kernel/debug.h"
#include "string.h"


IFileIO * pDebug = 0;


void
printk(const char * s, ...)
{
  if(pDebug != 0)
    pDebug->write(s, strlen(s));
}
