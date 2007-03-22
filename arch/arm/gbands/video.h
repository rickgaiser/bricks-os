#ifndef ARCH_VIDEO_H
#define ARCH_VIDEO_H


#include "kernel/video.h"
#include "inttypes.h"


class CGBAVideo
 : public CAVideo
{
public:
  CGBAVideo();
  virtual ~CGBAVideo();

  virtual int init();

private:
  virtual void put(int x, int y, char c);
  virtual char get(int x, int y);

  uint16_t * pVideo_;
};


#endif
