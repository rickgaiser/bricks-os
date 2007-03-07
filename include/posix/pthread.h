#ifndef PTHREAD_H
#define PTHREAD_H


#include "inttypes.h"


#ifdef __cplusplus
extern "C" {
#endif


struct pthread_t
{
  void * pThread;
};

struct pthread_attr_t
{
};

struct pthread_mutex_t
{
  volatile int iLock;
};

struct pthread_mutexattr_t
{
};

// Threads
int   pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void *), void * arg);
void  pthread_exit(void * status);

// Mutex
int   pthread_mutex_destroy(pthread_mutex_t *);
//int   pthread_mutex_getprioceiling(const pthread_mutex_t *, int *);
int   pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
int   pthread_mutex_lock(pthread_mutex_t *);
//int   pthread_mutex_setprioceiling(pthread_mutex_t *, int, int *);
int   pthread_mutex_trylock(pthread_mutex_t *);
int   pthread_mutex_unlock(pthread_mutex_t *);


#ifdef __cplusplus
}
#endif


#endif
