#ifndef USER_SRRNAMESERVER_H
#define USER_SRRNAMESERVER_H


#include "kernel/syscall.h"
#include "kernel/srrNameServer_k.h"
#include "sys/types.h"
#include "inttypes.h"


declareSysCallUser2r(int, registerName, int, channelID, const char *, name);
declareSysCallUser3r(int, lookupName,   const char *, name, int &, pid, int &, channelID);


#endif
