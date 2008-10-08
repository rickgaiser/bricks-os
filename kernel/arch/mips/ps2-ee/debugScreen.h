#ifndef ARCH_DEBUGSCREEN_H
#define ARCH_DEBUGSCREEN_H


#include "videoDevice.h"
#include "kernel/videoManager.h"
#include "kernel/fs.h"


#define TEXT_WIDTH 45
#define TEXT_HEIGHT 17


//---------------------------------------------------------------------------
class CPS2DebugScreen
 : public IFileIO
{
public:
  CPS2DebugScreen();
  virtual ~CPS2DebugScreen();

  virtual int init();

  // Inherited from IFileIO
  virtual ssize_t  write(const void * buffer, size_t size, bool block = false);

private:
  void cls();
  void setMode(SPS2VideoMode * mode);
  void printLine(uint16_t x, uint16_t y, char * str);

  SPS2VideoMode * pCurrentPS2Mode_;
  uint16_t actualHeight_;
  uint32_t frameAddr_[2];

  // Text
  unsigned int iCurrentX_;
  unsigned int iCurrentY_;
  char pBuffer_[TEXT_HEIGHT][TEXT_WIDTH];

  // Font
  uint32_t   g2_fontbuf_addr;    // address of font buffer
  uint32_t   g2_fontbuf_w;       // font buffer width
  uint32_t   g2_fontbuf_h;       // font buffer height
  uint16_t * g2_font_tc;         // pointer to texture coordinate buffer

  // Primitive to Window coordinate mapping
  uint16_t   gs_origin_x;
  uint16_t   gs_origin_y;

  // GIF packet for DMA transmission to GS
  CGIFPacket packet_;
};


#endif
