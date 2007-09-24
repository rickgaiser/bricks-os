#ifndef USER_PTHREAD_H
#define USER_PTHREAD_H


#include "kernel/syscall.h"
#include "kernel/pthread_k.h"
#include "sys/types.h"
#include "inttypes.h"
#include "time.h"


// Threads
declareSysCallUser4(int,  pthread_create,         pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
declareSysCallUser1(void, pthread_exit,           void *, status);
// Condition
declareSysCallUser2(int,  pthread_cond_init,      pthread_cond_t *, cond, const pthread_condattr_t *, attr);
declareSysCallUser1(int,  pthread_cond_destroy,   pthread_cond_t *, cond);
declareSysCallUser2(int,  pthread_cond_wait,      pthread_cond_t *, cond, pthread_mutex_t *, mutex);
declareSysCallUser3(int,  pthread_cond_timedwait, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
declareSysCallUser1(int,  pthread_cond_signal,    pthread_cond_t *, cond);
declareSysCallUser1(int,  pthread_cond_broadcast, pthread_cond_t *, cond);
// Mutex
declareSysCallUser2(int,  pthread_mutex_init,     pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
declareSysCallUser1(int,  pthread_mutex_destroy,  pthread_mutex_t *, mutex);
declareSysCallUser1(int,  pthread_mutex_lock,     pthread_mutex_t *, mutex);
declareSysCallUser1(int,  pthread_mutex_trylock,  pthread_mutex_t *, mutex);
declareSysCallUser1(int,  pthread_mutex_unlock,   pthread_mutex_t *, mutex);


#endif
