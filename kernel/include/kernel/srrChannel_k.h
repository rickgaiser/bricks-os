#ifndef KERNEL_SRRCHANNEL_H
#define KERNEL_SRRCHANNEL_H


#include "kernel/syscall.h"
#include "sys/types.h"
#include "inttypes.h"


// Channels
declareSysCallKernel1(int, channelCreate, unsigned, iFlags);
declareSysCallKernel1(int, channelDestroy, int, iChannelID);
declareSysCallKernel4(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
declareSysCallKernel1(int, channelConnectDetach, int, iConnectionID);


#endif
