#ifndef UNISTD_H
#define UNISTD_H


#include "sys/types.h"


#ifndef NULL
  #define NULL 0
#endif

#define STDIN   0
#define STDOUT  1
#define STDERR  2

// FIXME
#ifndef DIRECT_ACCESS_KERNEL
  #define DIRECT_ACCESS_KERNEL
#endif

#ifdef DIRECT_ACCESS_KERNEL
  #include "kernel/task.h"

  //#define brk        k_brk
  //#define close      k_close
  //#define _exit      k__exit
  #define getpid     k_getpid
  //#define read       k_read
  //#define sbrk       k_sbrk
  //#define sleep      k_sleep
  #define usleep     k_usleep
  //#define write      k_write
#else

  #ifdef __cplusplus
  extern "C" {
  #endif

  //int              brk(void * addr);
  //int              close(int iFD);
  //void             _exit(int iStatus);
  pid_t            getpid(void);
  //ssize_t          read(int iFD, void * pBuf, size_t size);
  //char           * sbrk(intptr_t increment);
  //unsigned int     sleep(unsigned int iSeconds);
  int              usleep(useconds_t useconds);
  //ssize_t          write(int iFD, const void * pBuf, size_t size);

  #ifdef __cplusplus
  }
  #endif

#endif


#endif
