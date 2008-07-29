#ifndef UNISTD_K_H
#define UNISTD_K_H


#include "kernel/syscall.h"
#include "sys/types.h"
#include "inttypes.h"


enum EFileCommand
{
  FC_OPEN  = 1,
  FC_CLOSE = 2,
  FC_READ  = 3,
  FC_WRITE = 4,
};

struct SFileHeader
{
  uint32_t command;
};

struct SFileOpenHeader
{
  SFileHeader commonHeader;
  uint32_t flags;
};

struct SFileCloseHeader
{
  SFileHeader commonHeader;
};

struct SFileReadHeader
{
  SFileHeader commonHeader;
  uint32_t size; // Max Read Size
};

struct SFileWriteHeader
{
  SFileHeader commonHeader;
  uint32_t size; // Max Write Size
};


#ifdef __cplusplus
extern "C" {
#endif


//declareSysCallKernel1r(int,             brk, void *, addr);
//declareSysCallKernel1r(int,             close, int, iFD);
//declareSysCallKernel1 (                 _exit, int, iStatus);
declareSysCallKernel0r(pid_t,           getpid);
//declareSysCallKernel3r(ssize_t,         read, int, iFD, void *, pBuf, size_t, size);
//declareSysCallKernel1r(char *,          sbrk, intptr_t, increment);
declareSysCallKernel1r(unsigned int,    sleep, unsigned int, iSeconds);
declareSysCallKernel1r(int,             usleep, useconds_t, useconds);
//declareSysCallKernel3r(ssize_t,         write, int, iFD, const void *, pBuf, size_t, size);


#ifdef __cplusplus
}
#endif


#endif
