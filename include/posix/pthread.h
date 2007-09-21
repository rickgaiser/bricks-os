#ifndef PTHREAD_H
#define PTHREAD_H


#include "inttypes.h"
#include "time.h"


#ifdef __cplusplus
extern "C" {
#endif


//#define PTHREAD_CANCEL_ASYNCHRONOUS
//#define PTHREAD_CANCEL_ENABLE
//#define PTHREAD_CANCEL_DEFERRED
//#define PTHREAD_CANCEL_DISABLE
//#define PTHREAD_CANCELED
#define PTHREAD_COND_INITIALIZER {1}
//#define PTHREAD_CREATE_DETACHED
//#define PTHREAD_CREATE_JOINABLE
//#define PTHREAD_EXPLICIT_SCHED
//#define PTHREAD_INHERIT_SCHED
//#define PTHREAD_MUTEX_DEFAULT
//#define PTHREAD_MUTEX_ERRORCHECK
//#define PTHREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_INITIALIZER {0}
//#define PTHREAD_MUTEX_RECURSIVE
//#define PTHREAD_ONCE_INIT
//#define PTHREAD_PRIO_INHERIT
//#define PTHREAD_PRIO_NONE
//#define PTHREAD_PRIO_PROTECT
//#define PTHREAD_PROCESS_SHARED
//#define PTHREAD_PROCESS_PRIVATE
//#define PTHREAD_RWLOCK_INITIALIZER
//#define PTHREAD_SCOPE_PROCESS
//#define PTHREAD_SCOPE_SYSTEM


// Threads
int   pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void *), void * arg);
void  pthread_exit(void * status);

// Condition
int   pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
int   pthread_cond_destroy(pthread_cond_t *);
int   pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int   pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);
int   pthread_cond_signal(pthread_cond_t *);
int   pthread_cond_broadcast(pthread_cond_t *);

// Mutex
int   pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
int   pthread_mutex_destroy(pthread_mutex_t *);
int   pthread_mutex_lock(pthread_mutex_t *);
int   pthread_mutex_trylock(pthread_mutex_t *);
int   pthread_mutex_unlock(pthread_mutex_t *);


#ifdef __cplusplus
}
#endif


#endif
