#ifndef FCNTL_H
#define FCNTL_H


#ifdef __cplusplus
extern "C" {
#endif


//int creat(const char * path, mode_t mode);
//int fcntl(int fd, int cmd, ...);
int open(const char * path, int oflag, ...);


#ifdef __cplusplus
}
#endif


#endif
