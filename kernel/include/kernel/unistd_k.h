#ifndef UNISTD_K_H
#define UNISTD_K_H


#include "kernel/syscall.h"
#include "sys/types.h"
#include "inttypes.h"


#ifdef __cplusplus
extern "C" {
#endif


//declareSysCallKernel1r(int,             brk, void *, addr);
//declareSysCallKernel1 (                 _exit, int, iStatus);
declareSysCallKernel0r(pid_t,           getpid);
//declareSysCallKernel1r(char *,          sbrk, intptr_t, increment);
declareSysCallKernel1r(unsigned int,    sleep, unsigned int, iSeconds);
declareSysCallKernel1r(int,             usleep, useconds_t, useconds);


#ifdef __cplusplus
}
#endif


#endif
