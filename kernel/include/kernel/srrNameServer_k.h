#ifndef KERNEL_SRRNAMESERVER_H
#define KERNEL_SRRNAMESERVER_H


#include "kernel/syscall.h"
#include "sys/types.h"
#include "inttypes.h"


#ifdef __cplusplus
extern "C" {
#endif


declareSysCallKernel2r(int, registerName, int, channelID, const char *, name);
declareSysCallKernel3r(int, lookupName,   const char *, name, int &, pid, int &, channelID);


#ifdef __cplusplus
}
#endif


#endif
