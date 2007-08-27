#ifndef TWL_FRAMEBUFFER_H
#define TWL_FRAMEBUFFER_H


// Screen resolutions:
//  - GBA: 240x160
//  - NDS: 256x192 (2x)
//  - PSP: 480x272


#include "rect.h"
#include "color.h"


namespace twl
{


enum EBufferType
{
    btSINGLE  // Draw in videobuffer directly
  , btDOUBLE  // Draw in backbuffer, synced swap needs to wait
//  , btTRIPPLE // Draw in tripple buffer, allows synced swap without waiting for sync.
};


class CSurface
{
public:
  CSurface();
  virtual ~CSurface();

  // Geometry functions
  virtual const CRect & rect() const;

  // Fill entire surface
  virtual void fill(color_t color);
  // Fill rect on surface
  virtual void fillRect(int x, int y, int width, int height, color_t color);
  // Copy another surface onto this surface
//  virtual void copy(int dstx, int dsty, const CSurface & src);
  // Copy part of the surface to another part of the surface
//  virtual void copyRect(int srcx, int srcy, int dstx, int dsty, int width, int height);

  virtual pixel_t & pixel(int x, int y);
  virtual void drawHLine(int y, int left, int right, color_t color);
  virtual void drawVLine(int x, int top, int bottom, color_t color);
  virtual void drawText(int x, int y, const char * string, color_t color = clBlack);

  // Screen functions
  virtual void swap(bool forceCopy = false);

protected:
  virtual void fastFill(color_t color, pixel_t * dest, uint32_t size);
  virtual void fastCopy(pixel_t * src, pixel_t * dest, uint32_t size);

  // Do not forget to initialize when subclassing

  // Frame buffers
  EBufferType eType_;
  pixel_t * pVideoBuffer_;
  pixel_t * pBackBuffer_;
  pixel_t * pTrippleBuffer_;
  pixel_t * pDrawBuffer_;     // Points to any of the above three buffers

  // Geometry
  CRect   cScreenRect_;
  uint32_t  iWidth_;
  uint32_t  iHeight_;
};


extern CSurface * pFB;


};


#endif
