#ifndef USER_PTHREAD_H
#define USER_PTHREAD_H


#include "kernel/syscall.h"
#include "kernel/pthread_k.h"
#include "sys/types.h"
#include "inttypes.h"
#include "time.h"


#ifdef __cplusplus
extern "C" {
#endif


// Threads
declareSysCallUser4r(int,  pthread_create,         pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
declareSysCallUser1 (      pthread_exit,           void *, status);
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


#ifdef __cplusplus
}
#endif


#endif
