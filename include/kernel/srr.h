#ifndef USER_SRR_H
#define USER_SRR_H


#include "kernel/pthread_k.h"
#include "kernel/srr_k.h"
#include "kernel/syscall.h"
#include "kernel/unistd_k.h"
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
declareSysCallUser1r(int, channelCreate,           unsigned, iFlags);
declareSysCallUser1r(int, channelDestroy,          int, iChannelID);
declareSysCallUser4r(int, channelConnectAttach,    uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
declareSysCallUser1r(int, channelConnectDetach,    int, iConnectionID);
// Names
declareSysCallUser2r(int, registerName,            int, channelID, const char *, name);
declareSysCallUser3r(int, lookupName,              const char *, name, int *, pid, int *, channelID);
// Threads
declareSysCallUser4r(int,  pthread_create,         pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
declareSysCallUser1 (      pthread_exit,           void *, status);
declareSysCallUser2r(int,  pthread_join,           pthread_t, thread, void **, value_ptr);
// Condition
declareSysCallUser2r(int,  pthread_cond_init,      pthread_cond_t *, cond, const pthread_condattr_t *, attr);
declareSysCallUser1r(int,  pthread_cond_destroy,   pthread_cond_t *, cond);
declareSysCallUser2r(int,  pthread_cond_wait,      pthread_cond_t *, cond, pthread_mutex_t *, mutex);
declareSysCallUser3r(int,  pthread_cond_timedwait, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
declareSysCallUser1r(int,  pthread_cond_signal,    pthread_cond_t *, cond);
declareSysCallUser1r(int,  pthread_cond_broadcast, pthread_cond_t *, cond);
// Mutex
declareSysCallUser2r(int,  pthread_mutex_init,     pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
declareSysCallUser1r(int,  pthread_mutex_destroy,  pthread_mutex_t *, mutex);
declareSysCallUser1r(int,  pthread_mutex_lock,     pthread_mutex_t *, mutex);
declareSysCallUser1r(int,  pthread_mutex_trylock,  pthread_mutex_t *, mutex);
declareSysCallUser1r(int,  pthread_mutex_unlock,   pthread_mutex_t *, mutex);
// Other
//declareSysCallUser1r(int,             brk, void *, addr);
//declareSysCallUser1 (                 _exit, int, iStatus);
declareSysCallUser0r(pid_t,           getpid);
//declareSysCallUser1r(char *,          sbrk, intptr_t, increment);
declareSysCallUser1r(unsigned int,    sleep, unsigned int, iSeconds);
declareSysCallUser1r(int,             usleep, useconds_t, useconds);


#ifdef __cplusplus
}
#endif


#endif
