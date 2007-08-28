#ifndef UNISTD_H
#define UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif


#ifndef NULL
#define NULL 0
#endif


typedef unsigned int size_t;
typedef int ssize_t;


#define STDIN   0
#define STDOUT  1
#define STDERR  2


//int brk(void * addr);
//int close(int iFD);
//void _exit(int iStatus);
//ssize_t read(int iFD, void * pBuf, size_t size);
//char * sbrk(intptr_t increment);
//unsigned int sleep(unsigned int iSeconds);
//ssize_t write(int iFD, const void * pBuf, size_t size);


#ifdef __cplusplus
}
#endif

#endif
