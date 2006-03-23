#ifndef GBAVIDEO_H
#define GBAVIDEO_H


#include "kernel/video.h"


// The GBA has 6 display modes:
//  - Mode0: 4 text backgrounds
//  - Mode1: 2 text backgrounds + 1 rotating/scaling background
//  - Mode2: 2 rotating/scaling backgrounds
//  - Mode3: 1 240x160x16bpp bitmapped
//  - Mode4: 2 240x160x8bpp paletted
//  - Mode5: 2 160x128x16bpp bitmapped


#define VIDC_BASE       *(volatile unsigned short*)0x4000000
#define DISPCNT         *(unsigned long *)(VIDC_BASE)
#define DISP_MODE(n)    (n & 7)
#define DISP_BG2        0x400
#define VRAM_BASE       0x6000000
#define BG_PAL          0x5000000

#define REG_DISPCNT     *(volatile unsigned int*)0x4000000    // Display Control (Mode)
#define REG_BG0CNT      *(volatile unsigned short*)0x4000008  // Background 0
#define REG_BG0HOFS     *(volatile unsigned short*)0x4000010  // Background 0 Horizontal Offset
#define REG_BG0VOFS     *(volatile unsigned short*)0x4000012  // Background 0 Vertical Offset
#define s_bgPalette     ((volatile unsigned short*)0x05000000)
#define s_objPalette    ((volatile unsigned short*)0x05000200)

#define CHARS_PER_LINE 30
#define LINE_COUNT     20
#define VIDEO_START    ((short*)0x6000000)


class CGBAVideo
 : public CAVideo
{
public:
  CGBAVideo();
  virtual ~CGBAVideo();

  virtual int init();

private:
  virtual void put(int iX, int iY, char c);
  virtual char get(int iX, int iY);

  short * pVideo_;
};


#endif
