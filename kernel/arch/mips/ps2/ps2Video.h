#ifndef PS2VIDEO_H
#define PS2VIDEO_H


#include "kernel/fs.h"


class CPS2Video
 : public IFileIO
{
public:
  CPS2Video();
  virtual ~CPS2Video();

  // Inherited from IFileIO
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);

private:
  void cls();
};


#endif
