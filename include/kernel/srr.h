#ifndef USER_SRR_H
#define USER_SRR_H


#include "asm/arch/config.h"


#ifdef CONFIG_DIRECT_ACCESS_KERNEL
  #include "kernel/srr_k.h"

  #define msgSend    k_msgSend
  #define msgReceive k_msgReceive
  #define msgReply   k_msgReply
#else
  #include "asm/syscall.h"

  static inline _syscall5(int, msgSend,    int, iConnectionID, const void *, pSndMsg, int, iSndSize, void *, pRcvMsg, int, iRcvSize);
  static inline _syscall3(int, msgReceive, int, iChannelID, void *, pRcvMsg, int, iRcvSize);
  static inline _syscall4(int, msgReply,   int, iReceiveID, int, iStatus, const void *, pReplyMsg, int, iReplySize);
#endif


#endif
