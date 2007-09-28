#ifndef UNISTD_H
#define UNISTD_H


#include "kernel/syscall.h"
#include "kernel/task.h"
#include "sys/types.h"
#include "inttypes.h"


#ifndef NULL
  #define NULL 0
#endif

#define STDIN   0
#define STDOUT  1
#define STDERR  2


//declareSysCallUser1r(int,             brk, void *, addr);
//declareSysCallUser1r(int,             close, int, iFD);
//declareSysCallUser1 (                 _exit, int, iStatus);
declareSysCallUser0r(pid_t,           getpid);
//declareSysCallUser3r(ssize_t,         read, int, iFD, void *, pBuf, size_t, size);
//declareSysCallUser1r(char *,          sbrk, intptr_t, increment);
declareSysCallUser1r(unsigned int,    sleep, unsigned int, iSeconds);
declareSysCallUser1r(int,             usleep, useconds_t, useconds);
//declareSysCallUser3r(ssize_t,         write, int, iFD, const void *, pBuf, size_t, size);


#endif
