#ifndef TYPES_H
#define TYPES_H


typedef long long int    loff_t;
typedef long             off_t;
typedef int              pid_t;

typedef struct pthread_attr_t
{
} pthread_attr_t;

typedef struct pthread_cond_t
{
  volatile int iLock;
} pthread_cond_t;

typedef struct pthread_condattr_t
{
} pthread_condattr_t;

typedef struct pthread_mutex_t
{
  volatile int iLock;
} pthread_mutex_t;

typedef struct pthread_mutexattr_t
{
} pthread_mutexattr_t;

typedef struct pthread_t
{
  void * pThread;
} pthread_t;

#ifndef __SIZE_T__
#define __SIZE_T__
typedef unsigned int     size_t;
#endif
typedef int              ssize_t;
typedef long             suseconds_t;
typedef long             time_t;
typedef unsigned long    useconds_t;


#endif
