#ifndef ARCH_DEBUGSCREEN_H
#define ARCH_DEBUGSCREEN_H


#include "kernel/video.h"
#include "inttypes.h"


#ifdef GBA
  #define DEFAULT_SCREEN 0
#else
  #define TOP_SCREEN     0
  #define BOTTOM_SCREEN  1
  #define DEFAULT_SCREEN TOP_SCREEN
#endif



class CGBADebugScreen
 : public CAVideo
{
public:
  CGBADebugScreen();
  virtual ~CGBADebugScreen();

  virtual int init(int screen = DEFAULT_SCREEN);

private:
  virtual void put(int x, int y, char c);
  virtual char get(int x, int y);

  uint16_t * pVideo_;
};


#endif
