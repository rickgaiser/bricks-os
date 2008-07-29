#ifndef USER_UNISTD_H
#define USER_UNISTD_H


#include "sys/types.h"


#ifndef NULL
#define NULL (0)
#endif

#define STDIN   0
#define STDOUT  1
#define STDERR  2


#ifdef __cplusplus
extern "C" {
#endif


int     close(int fd);
ssize_t read (int fd, void * buffer, size_t size);
ssize_t write(int fd, const void * buffer, size_t size);


#ifdef __cplusplus
}
#endif


#endif
