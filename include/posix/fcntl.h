#ifndef FCNTL_H
#define FCNTL_H


#ifdef __cplusplus
extern "C" {
#endif


#define O_RDONLY    0x0001 // open for reading only
#define O_WRONLY    0x0002 // open for writing only
#define O_RDWR      0x0000 // open for reading and writing
#define O_ACCMODE   0x0003 // mask for above modes

#define O_NONBLOCK  0x0004 // no delay
#define O_APPEND    0x0008 // set append mode
#define O_CREAT     0x0200 // create if nonexistant
#define O_TRUNC     0x0400 // truncate to zero length
#define O_EXCL      0x0800 // error if already exists


//int creat(const char * path, mode_t mode);
//int fcntl(int fd, int cmd, ...);
int open(const char * path, int oflag, ...);


#ifdef __cplusplus
}
#endif


#endif
