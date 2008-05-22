#include "kernel/srr.h"
#include "kernel/syscall.h"


//---------------------------------------------------------------------------
// Channels
//---------------------------------------------------------------------------
sysCallUser1r(int, channelCreate, unsigned, iFlags);
sysCallUser1r(int, channelDestroy, int, iChannelID);
sysCallUser4r(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
sysCallUser1r(int, channelConnectDetach, int, iConnectionID);

//---------------------------------------------------------------------------
// Names
//---------------------------------------------------------------------------
sysCallUser2r(int, registerName, int, channelID, const char *, name);
sysCallUser3r(int, lookupName, const char *, name, int *, pid, int *, channelID);
