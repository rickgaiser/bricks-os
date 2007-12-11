#ifndef ARCH_VIDEO_H
#define ARCH_VIDEO_H


#include "kernel/video.h"
#include "inttypes.h"


class CPSPVideo
 : public IFileIO
{
public:
  CPSPVideo();
  virtual ~CPSPVideo();

  virtual int init();

  // Inherited from IFileIO
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);
};


#endif
