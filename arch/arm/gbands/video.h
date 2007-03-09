#ifndef ARCH_VIDEO_H
#define ARCH_VIDEO_H


#include "kernel/video.h"
#include "inttypes.h"


// The GBA has 6 display modes:
//  - Mode0: 4 text backgrounds
//  - Mode1: 2 text backgrounds + 1 rotating/scaling background
//  - Mode2: 2 rotating/scaling backgrounds
//  - Mode3: 1 240x160x16bpp bitmapped
//  - Mode4: 2 240x160x8bpp paletted
//  - Mode5: 2 160x128x16bpp bitmapped


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
