#ifndef USER_SRRCHANNEL_H
#define USER_SRRCHANNEL_H


#include "kernel/syscall.h"
#include "kernel/srrChannel_k.h"
#include "sys/types.h"
#include "inttypes.h"


// Channels
declareSysCallUser1r(int, channelCreate, unsigned, iFlags);
declareSysCallUser1r(int, channelDestroy, int, iChannelID);
declareSysCallUser4r(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
declareSysCallUser1r(int, channelConnectDetach, int, iConnectionID);


#endif
