#ifndef ARCH_DEBUGSCREEN_H
#define ARCH_DEBUGSCREEN_H


#include "kernel/video.h"
#include "inttypes.h"


class CGBADebugScreen
 : public CAVideo
{
public:
  CGBADebugScreen();
  virtual ~CGBADebugScreen();

  virtual int init();

private:
  virtual void put(int x, int y, char c);
  virtual char get(int x, int y);

  uint16_t * pVideo_;
};


#endif
