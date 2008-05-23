#ifndef KERNEL_DRIVERMOUNTPOINT_H
#define KERNEL_DRIVERMOUNTPOINT_H


#include "kernel/driverKernel.h"


// -----------------------------------------------------------------------------
struct DIR_ITER
{
};

// -----------------------------------------------------------------------------
struct stat
{
};

// -----------------------------------------------------------------------------
// Mount point interface class
class IMountPoint
{
public:
  virtual ~IMountPoint(){}

  virtual int open(const char * path, int flags, int mode) = 0;
  virtual int close(int fd) = 0;
  virtual int read(int fd, char * ptr, int len) = 0;
  virtual int write(int fd, const char * ptr, int len) = 0;
/*
  virtual int seek(int fd, int pos, int dir) = 0;
  virtual int fstat(int fd, struct stat * st) = 0;
  virtual int stat(const char * file, struct stat * st) = 0;
  virtual int link(const char * existing, const char * newLink) = 0;
  virtual int unlink(const char * name) = 0;
  virtual int chdir(const char * name) = 0;

  virtual int rename(const char * oldName, const char * newName) = 0;
  virtual int mkdir(const char * path, int mode) = 0;
*/
  virtual DIR_ITER * diropen(const char * path) = 0;
  virtual int dirreset(DIR_ITER * dirState) = 0;
  virtual int dirnext(DIR_ITER * dirState, char * filename, struct stat * filestat) = 0;
  virtual int dirclose(DIR_ITER * dirState) = 0;
};

// -----------------------------------------------------------------------------
// Kernel driver for converting messages to IMountPoint calls
class CMountPointDriver
 : public IKernelDriver
{
public:
  CMountPointDriver(IMountPoint * mountPoint = 0);
  virtual ~CMountPointDriver();

  void setMountPoint(IMountPoint * mountPoint);

  virtual int msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);

private:
  IMountPoint * pMountPoint_;
};


#endif
