#include "kernel/srr.h"
#include "kernel/srr_s.h"
#include "kernel/srrChannel.h"
#include "kernel/srrChannel_s.h"  // Shared kernel header, defines packing/unpacking
#include "kernel/syscall.h"
#include "string.h"


//---------------------------------------------------------------------------
// Channels
//---------------------------------------------------------------------------
sysCallUser1(int, channelCreate, unsigned, iFlags);
sysCallUser1(int, channelDestroy, int, iChannelID);
sysCallUser4(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
sysCallUser1(int, channelConnectDetach, int, iConnectionID);

