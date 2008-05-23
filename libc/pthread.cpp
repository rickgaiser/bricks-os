#include "pthread.h"
#include "kernel/syscall.h"
#include "kernel/srr.h"


// Threads
sysCallUser4r(int,             pthread_create,         pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
sysCallUser1 (                 pthread_exit,           void *, status);
sysCallUser2r(int,             pthread_join,           pthread_t, thread, void **, value_ptr);
// Condition
sysCallUser2r(int,             pthread_cond_init,      pthread_cond_t *, cond, const pthread_condattr_t *, attr);
sysCallUser1r(int,             pthread_cond_destroy,   pthread_cond_t *, cond);
sysCallUser2r(int,             pthread_cond_wait,      pthread_cond_t *, cond, pthread_mutex_t *, mutex);
sysCallUser3r(int,             pthread_cond_timedwait, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
sysCallUser1r(int,             pthread_cond_signal,    pthread_cond_t *, cond);
sysCallUser1r(int,             pthread_cond_broadcast, pthread_cond_t *, cond);
// Mutex
sysCallUser2r(int,             pthread_mutex_init,     pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
sysCallUser1r(int,             pthread_mutex_destroy,  pthread_mutex_t *, mutex);
sysCallUser1r(int,             pthread_mutex_lock,     pthread_mutex_t *, mutex);
sysCallUser1r(int,             pthread_mutex_trylock,  pthread_mutex_t *, mutex);
sysCallUser1r(int,             pthread_mutex_unlock,   pthread_mutex_t *, mutex);
