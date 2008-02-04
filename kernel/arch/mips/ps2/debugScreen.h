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
  virtual ssize_t  write(const void * buffer, size_t size, loff_t * = 0);

private:
  void cls();
  void setMode(SPS2VideoMode * mode);

  void g2_set_visible_frame(uint8_t frame);
  void g2_set_active_frame(uint8_t frame);

  void g2_set_font(uint32_t *data, uint16_t w, uint16_t h, uint16_t *tc);
  void g2_out_text(uint16_t x, uint16_t y, char *str);
  void g2_set_font_spacing(uint16_t s);
  uint16_t g2_get_font_spacing(void);
  void g2_set_font_mag(uint16_t m);
  uint16_t g2_get_font_mag(void);
  void g2_set_font_color(uint8_t red, uint8_t green, uint8_t blue);

  SPS2VideoMode * pCurrentPS2Mode_;
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
  uint16_t   g2_font_spacing;    // pixels between each drawn character
  uint16_t   g2_font_mag;        // magnification factor

  // Primitive to Window coordinate mapping
  uint16_t   gs_origin_x;
  uint16_t   gs_origin_y;

  // DMA Packet
  DECLARE_GS_PACKET(gs_dma_buf,50);
};


#endif
