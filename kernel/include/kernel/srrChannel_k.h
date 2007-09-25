#ifndef KERNEL_SRRCHANNEL_H
#define KERNEL_SRRCHANNEL_H


#include "kernel/syscall.h"
#include "sys/types.h"
#include "inttypes.h"


#ifdef __cplusplus
extern "C" {
#endif


// Channels
declareSysCallKernel1r(int, channelCreate, unsigned, iFlags);
declareSysCallKernel1r(int, channelDestroy, int, iChannelID);
declareSysCallKernel4r(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
declareSysCallKernel1r(int, channelConnectDetach, int, iConnectionID);


#ifdef __cplusplus
}
#endif


#endif
