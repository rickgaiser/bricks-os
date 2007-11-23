#ifndef TYPES_H
#define TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


typedef long long int    loff_t;
typedef long             off_t;
typedef int              pid_t;

struct pthread_attr_t
{
};

struct pthread_cond_t
{
  volatile int iLock;
};

struct pthread_condattr_t
{
};

struct pthread_mutex_t
{
  volatile int iLock;
};

struct pthread_mutexattr_t
{
};

struct pthread_t
{
  void * pThread;
};

#ifndef __SIZE_T__
#define __SIZE_T__
typedef unsigned int     size_t;
#endif
typedef int              ssize_t;
typedef long             suseconds_t;
typedef long             time_t;
typedef unsigned long    useconds_t;


#ifdef __cplusplus
}
#endif


#endif
