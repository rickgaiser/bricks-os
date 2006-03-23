#ifndef UNISTD_H
#define UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned int size_t;
typedef int ssize_t;


#define STDIN   0
#define STDOUT  1
#define STDERR  2


//void _exit(int iStatus);
int close(int iFD);
ssize_t read(int iFD, void * pBuf, size_t size);
//unsigned int sleep(unsigned int iSeconds);
ssize_t write(int iFD, const void * pBuf, size_t size);
//int brk(char * addr);
//char * sbrk(int increment);


#ifdef __cplusplus
}
#endif

#endif
