#ifndef FATDRIVER_H
#define FATDRIVER_H


#include "fileSystem.h"


// -----------------------------------------------------------------------------
class CFATDriver
 : public IFileSystemDriver
{
public:
  CFATDriver();
  virtual ~CFATDriver();

  virtual bool init(IBlockDevice * device);
};

// -----------------------------------------------------------------------------
class CFATMountPoint
 : public IMountPoint
{
public:
  CFATMountPoint(IBlockDevice * device);
  virtual ~CFATMountPoint();

  virtual int open(void * fileStruct, const char * path, int flags, int mode);
  virtual int close(int fd);
  virtual int write(int fd, const char * ptr, int len);
  virtual int read(int fd, char * ptr, int len);

private:
  IBlockDevice * device_;
};


#endif
