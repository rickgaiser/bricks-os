#include "kernel/srr.h"
#include "kernel/syscall.h"
#include "string.h"


//---------------------------------------------------------------------------
// Channels
//---------------------------------------------------------------------------
sysCallUser1r(int, channelCreate, unsigned, iFlags);
sysCallUser1r(int, channelDestroy, int, iChannelID);
sysCallUser4r(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
sysCallUser1r(int, channelConnectDetach, int, iConnectionID);
