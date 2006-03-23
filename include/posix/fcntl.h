#ifndef FCNTL_H
#define FCNTL_H


#ifdef __cplusplus
extern "C" {
#endif


//int creat(const char * pPath, Mode_t mode);
//int fcntl(int iFd, int iCMD, ...);
int open(const char * pPath, int iOflags, ...);


#ifdef __cplusplus
}
#endif


#endif
