#ifndef USER_SRRCHANNEL_H
#define USER_SRRCHANNEL_H


#include "kernel/syscall.h"
#include "kernel/srrChannel_k.h"
#include "sys/types.h"
#include "inttypes.h"


// Channels
declareSysCallUser1(int, channelCreate, unsigned, iFlags);
declareSysCallUser1(int, channelDestroy, int, iChannelID);
declareSysCallUser4(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
declareSysCallUser1(int, channelConnectDetach, int, iConnectionID);


#endif
