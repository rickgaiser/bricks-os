#ifndef USER_SRR_H
#define USER_SRR_H


#define DIRECT_ACCESS_SRR
#ifdef DIRECT_ACCESS_SRR
  #include "kernel/srr_k.h"

  #define msgSend    k_msgSend
  #define msgReceive k_msgReceive
  #define msgReply   k_msgReply
#else
  #include "asm/syscall.h"

  // System Call Function numbers
  #define SC_ERROR        0
  #define SC_SRR_SEND     1
  #define SC_SRR_RECEIVE  2
  #define SC_SRR_REPLY    3

  #define msgSend(arg1, arg2, arg3, arg4, arg5)     sysCall5(SC_SRR_SEND,    arg1, arg2, arg3, arg4, arg5)
  #define msgReceive(arg1, arg2, arg3)              sysCall3(SC_SRR_RECEIVE, arg1, arg2, arg3)
  #define msgReply(arg1, arg2, arg3, arg4)          sysCall4(SC_SRR_REPLY,   arg1, arg2, arg3, arg4)
#endif


#endif
