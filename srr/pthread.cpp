#include "kernel/syscall.h"
#include "pthread.h"
#include "sys/types.h"
#include "inttypes.h"
#include "time.h"
#include "kernel/srr.h"
#include "kernel/srr_s.h"
#include "kernel/srrChannel_s.h"


// Threads
sysCallUser4(int,  pthread_create,         pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
sysCallUser1(void, pthread_exit,           void *, status);
// Condition
sysCallUser2(int,  pthread_cond_init,      pthread_cond_t *, cond, const pthread_condattr_t *, attr);
sysCallUser1(int,  pthread_cond_destroy,   pthread_cond_t *, cond);
sysCallUser2(int,  pthread_cond_wait,      pthread_cond_t *, cond, pthread_mutex_t *, mutex);
sysCallUser3(int,  pthread_cond_timedwait, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
sysCallUser1(int,  pthread_cond_signal,    pthread_cond_t *, cond);
sysCallUser1(int,  pthread_cond_broadcast, pthread_cond_t *, cond);
// Mutex
sysCallUser2(int,  pthread_mutex_init,     pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
sysCallUser1(int,  pthread_mutex_destroy,  pthread_mutex_t *, mutex);
sysCallUser1(int,  pthread_mutex_lock,     pthread_mutex_t *, mutex);
sysCallUser1(int,  pthread_mutex_trylock,  pthread_mutex_t *, mutex);
sysCallUser1(int,  pthread_mutex_unlock,   pthread_mutex_t *, mutex);

