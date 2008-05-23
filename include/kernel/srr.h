#ifndef USER_SRR_H
#define USER_SRR_H


#include "kernel/srr_k.h"
#include "kernel/syscall.h"
#include "asm/arch/config.h"
#include "sys/types.h"
#include "inttypes.h"


#ifdef __cplusplus
extern "C" {
#endif


// SRR
#ifdef CONFIG_DIRECT_ACCESS_KERNEL
  #define msgSend    k_msgSend
  #define msgReceive k_msgReceive
  #define msgReply   k_msgReply
#else
  static inline _syscall5(int, msgSend,    int, iConnectionID, const void *, pSndMsg, int, iSndSize, void *, pRcvMsg, int, iRcvSize);
  static inline _syscall3(int, msgReceive, int, iChannelID, void *, pRcvMsg, int, iRcvSize);
  static inline _syscall4(int, msgReply,   int, iReceiveID, int, iStatus, const void *, pReplyMsg, int, iReplySize);
#endif
// Channels
declareSysCallUser1r(int, channelCreate, unsigned, iFlags);
declareSysCallUser1r(int, channelDestroy, int, iChannelID);
declareSysCallUser4r(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
declareSysCallUser1r(int, channelConnectDetach, int, iConnectionID);
// Names
declareSysCallUser2r(int, registerName, int, channelID, const char *, name);
declareSysCallUser3r(int, lookupName, const char *, name, int *, pid, int *, channelID);


#ifdef __cplusplus
}
#endif


#endif
