#ifndef PS2VIDEO_H
#define PS2VIDEO_H


#include "kernel/file.h"


class CPS2Video
 : public IFileIO
{
public:
  CPS2Video();
  virtual ~CPS2Video();

  virtual int init();

  virtual int write(const void * data, size_t size);

private:
  void cls();
};


#endif
