#ifndef ARCH_DEBUGSCREEN_H
#define ARCH_DEBUGSCREEN_H


#include "kernel/video.h"
#include "inttypes.h"


class CDCDebugScreen
 : public IFileIO
{
public:
  CDCDebugScreen();
  virtual ~CDCDebugScreen();

  virtual int init();

  // Inherited from IFileIO
  virtual ssize_t write(const void * buffer, size_t size, bool block = false);
};


#endif
