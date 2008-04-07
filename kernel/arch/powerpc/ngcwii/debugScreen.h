#ifndef ARCH_DEBUGSCREEN_H
#define ARCH_DEBUGSCREEN_H


#include "kernel/video.h"
#include "inttypes.h"


class CNGCDebugScreen
 : public IFileIO
{
public:
  CNGCDebugScreen();
  virtual ~CNGCDebugScreen();

  virtual int init();

  // Inherited from IFileIO
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);
};


#endif
