#include "unistd.h"
#include "kernel/syscall.h"
#include "kernel/srr.h"


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
