/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "kernel/2dRenderer.h"
#include "string.h"
#include "stdlib.h"


//---------------------------------------------------------------------------
#define SET_PIXEL(x,y,c) \
{ \
  switch(pSurface_->mode.bpp) \
  { \
    case 8: \
      ((uint8_t  *)pSurface_->p)[y * pSurface_->mode.xpitch + x] = c; \
      break; \
    case 16: \
      ((uint16_t *)pSurface_->p)[y * pSurface_->mode.xpitch + x] = c; \
      break; \
    case 32: \
      ((uint32_t *)pSurface_->p)[y * pSurface_->mode.xpitch + x] = c; \
      break; \
    default: \
      ; \
  }; \
}

//---------------------------------------------------------------------------
// Get clipping flags
#define GET_CLIP_FLAGS_X(x) \
  (((x < clipper_.xmin)     ) | \
   ((x > clipper_.xmax) << 1))
#define GET_CLIP_FLAGS_Y(y) \
  (((y < clipper_.ymin) << 2) | \
   ((y > clipper_.ymax) << 3))
#define GET_CLIP_FLAGS(x,y) \
  (GET_CLIP_FLAGS_X(x) | GET_CLIP_FLAGS_Y(y))

//---------------------------------------------------------------------------
// Clip point to clipper
#define CLIP_POINT_X(x) \
{ \
  if(x < clipper_.xmin) x = clipper_.xmin; \
  else if(x > clipper_.xmax) x = clipper_.xmax; \
}
#define CLIP_POINT_Y(y) \
{ \
  if(y < clipper_.ymin) y = clipper_.ymin; \
  else if(y > clipper_.ymax) y = clipper_.ymax; \
}
#define CLIP_POINT(x,y) \
{ \
  CLIP_POINT_X(x); \
  CLIP_POINT_Y(y); \
}

//---------------------------------------------------------------------------
// Clip point to surface
#define CLIP_POINT_SURF_X(x) \
{ \
  if(x < 0) x = 0; \
  else if((unsigned int)x >= pSurface_->mode.width)  x = pSurface_->mode.width  - 1; \
}
#define CLIP_POINT_SURF_Y(y) \
{ \
  if(y < 0) y = 0; \
  else if((unsigned int)y >= pSurface_->mode.height) y = pSurface_->mode.height - 1; \
}
#define CLIP_POINT_SURF(x,y) \
{ \
  CLIP_POINT_SURF_X(x); \
  CLIP_POINT_SURF_Y(y); \
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
C2DRenderer::C2DRenderer(CSurface * surf)
{
  setSurface(surf);
  setColor(0, 0, 0);
}

//---------------------------------------------------------------------------
C2DRenderer::~C2DRenderer()
{
}

//---------------------------------------------------------------------------
void
C2DRenderer::setSurface(CSurface * surface)
{
  pSurface_ = surface;

  if(pSurface_ != NULL)
  {
    // (Re)set clipper
    clipper_.xmin = 0;
    clipper_.ymin = 0;
    clipper_.xmax = pSurface_->mode.width  - 1;
    clipper_.ymax = pSurface_->mode.height - 1;
  }
}

//---------------------------------------------------------------------------
CSurface *
C2DRenderer::getSurface()
{
  return pSurface_;
}

//---------------------------------------------------------------------------
void
C2DRenderer::flush()
{
}

//---------------------------------------------------------------------------
void
C2DRenderer::setColor(color_t rgb)
{
  color_ = rgb;

  if(pSurface_ != NULL)
    fmtColor_ = BxColorFormat_FromRGB(pSurface_->mode.format,
                                      BxColorFormat_GetR(cfA8R8G8B8, color_),
                                      BxColorFormat_GetG(cfA8R8G8B8, color_),
                                      BxColorFormat_GetB(cfA8R8G8B8, color_));
}

//---------------------------------------------------------------------------
void
C2DRenderer::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  color_ = BxColorFormat_FromRGB(cfA8R8G8B8, r, g, b);

  if(pSurface_ != NULL)
    fmtColor_ = BxColorFormat_FromRGB(pSurface_->mode.format, r, g, b);
}

//---------------------------------------------------------------------------
void
C2DRenderer::setPixel(int x, int y)
{
  if(pSurface_ == NULL)
    return;

  if(GET_CLIP_FLAGS(x, y) == 0)
      setPixel_i(x, y);
}

//---------------------------------------------------------------------------
// Fill the entire surface
void
C2DRenderer::fill()
{
  if(pSurface_ == NULL)
    return;

  fillRect_i(clipper_.xmin, clipper_.ymin, clipper_.xmax - clipper_.xmin + 1, clipper_.ymax - clipper_.ymin + 1);
}

//---------------------------------------------------------------------------
// Fill specified rectangle
void
C2DRenderer::fillRect(int x, int y, unsigned int width, unsigned int height)
{
  if(pSurface_ == NULL)
    return;

  int x2 = x + width  - 1;
  int y2 = y + height - 1;

  // Get clipping flags
  uint32_t clip1 = GET_CLIP_FLAGS(x, y);
  uint32_t clip2 = GET_CLIP_FLAGS(x2, y2);

  // Not visible
  if(clip1 & clip2)
    return;

  // Clip points
  if(clip1)
  {
    CLIP_POINT_X(x);
    CLIP_POINT_Y(y);
  }
  if(clip2)
  {
    CLIP_POINT_X(x2);
    CLIP_POINT_Y(y2);
  }

  // Call drawing implementation
  fillRect_i(x, y, x2 - x + 1, y2 - y + 1);
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawLine(int x1, int y1, int x2, int y2)
{
  if(pSurface_ == NULL)
    return;

  // Get clipping flags
  uint32_t clip1 = GET_CLIP_FLAGS(x1, y1);
  uint32_t clip2 = GET_CLIP_FLAGS(x2, y2);

  // Not visible
  if(clip1 & clip2)
    return;

  // Clip points
  // FIXME:
  //  - Clipping x means interpolating y
  //  - Clipping y means interpolating x
  if(clip1 | clip2)
    return;

  // Call drawing implementation
  drawLine_i(x1, y1, x2, y2);
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawHLine(int x, int y, unsigned int width)
{
  if(pSurface_ == NULL)
    return;

  // Not visible
  if(GET_CLIP_FLAGS_Y(y))
    return;

  int x2 = x + width - 1;

  // Get clipping flags
  uint32_t clip1 = GET_CLIP_FLAGS_X(x);
  uint32_t clip2 = GET_CLIP_FLAGS_X(x2);

  // Not visible
  if(clip1 & clip2)
    return;

  // Clip points
  if(clip1)
    CLIP_POINT_X(x);
  if(clip2)
    CLIP_POINT_X(x2);

  // Call drawing implementation
  drawHLine_i(x, y, x2 - x + 1);
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawVLine(int x, int y, unsigned int height)
{
  if(pSurface_ == NULL)
    return;

  // Not visible
  if(GET_CLIP_FLAGS_X(x))
    return;

  int y2 = y + height - 1;

  // Get clipping flags
  uint32_t clip1 = GET_CLIP_FLAGS_Y(y);
  uint32_t clip2 = GET_CLIP_FLAGS_Y(y2);

  // Not visible
  if(clip1 & clip2)
    return;

  // Clip points
  if(clip1)
    CLIP_POINT_Y(y);
  if(clip2)
    CLIP_POINT_Y(y2);

  // Call drawing implementation
  drawVLine_i(x, y, y2 - y + 1);
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  if(pSurface_ == NULL)
    return;

  drawHLine(x, y, width);                      // Top
  drawHLine(x, y + height - 1, width);         // Bottom
  drawVLine(x, y + 1, height - 2);             // Left
  drawVLine(x + width - 1, y + 1, height - 2); // Right
}

//---------------------------------------------------------------------------
void
C2DRenderer::setClipRect(int x, int y, unsigned int width, unsigned int height)
{
  if(pSurface_ == NULL)
    return;

  int x2 = x + width  - 1;
  int y2 = y + height - 1;

  // Clip to surface
  CLIP_POINT_SURF(x,  y);
  CLIP_POINT_SURF(x2, y2);

  clipper_.xmin = x;
  clipper_.ymin = y;
  clipper_.xmax = x2;
  clipper_.ymax = y2;
}

//---------------------------------------------------------------------------
void
C2DRenderer::setPixel_i(int x, int y)
{
  SET_PIXEL(x, y, fmtColor_);
}

//---------------------------------------------------------------------------
void
C2DRenderer::fillRect_i(int x, int y, unsigned int width, unsigned int height)
{
  for(uint32_t i(0); i < height; i++)
    drawHLine_i(x, y + i, width);
}

//---------------------------------------------------------------------------
class CDDA
{
public:
  CDDA(){}
  CDDA(int num, int den)
  {
    set(num, den);
  }

  void set(int num, int den)
  {
    iNum_ = num;
    iDen_ = den;

    if(iNum_ >= 0)
    {
      // Positive case, C is okay
      iDiv_    = iNum_ / iDen_;
      iModulo_ = iNum_ % iDen_;
    }
    else
    {
      // Negative case, do the right thing
      iDiv_    = -((-iNum_) / iDen_);
      iModulo_ =   (-iNum_) % iDen_;
      if(iModulo_)
      {
        // There is a remainder
        iDiv_--;
        iModulo_ = iDen_ - iModulo_;
      }
    }

    // Set error to half a result value
    iError_ = iDen_ >> 1;
  }

  void increment()
  {
    iCurrent_ += iDiv_;
    iError_ += iModulo_;
    if(iError_ >= iDen_)
    {
      iError_ -= iDen_;
      iCurrent_++;
    }
  }

  int numerator(){return iNum_;}
  int denominator(){return iDen_;}

  void current(int i){iCurrent_ = i;}
  int  current(){return iCurrent_;}

private:
  int iNum_;
  int iDen_;
  int iDiv_;
  int iModulo_;
  int iError_;
  int iCurrent_;
};

//---------------------------------------------------------------------------
void
C2DRenderer::drawLine_i(int x1, int y1, int x2, int y2)
{
  if(y1 == y2)
  {
    drawHLine_i(x1, y1, x2 - x1 + 1);
  }
  else if(x1 == x2)
  {
    drawVLine_i(x1, y1, y2 - y1 + 1);
  }
  else
  {
    int dx = x2 - x1;
    int dy = y2 - y1;

    if(abs(dx) >= abs(dy))
    {
      CDDA dda(dy, dx);
      dda.current(y1);
      for(; x1 <= x2; x1++)
      {
        SET_PIXEL(x1, dda.current(), fmtColor_);
        dda.increment();
      }
    }
    else
    {
      CDDA dda(dx, dy);
      dda.current(x1);
      for(; y1 <= y2; y1++)
      {
        SET_PIXEL(dda.current(), y1, fmtColor_);
        dda.increment();
      }
    }
  }
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawHLine_i(int x, int y, unsigned int width)
{
  while(width > 0)
  {
    SET_PIXEL(x, y, fmtColor_);
    x++;
    width--;
  }
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawVLine_i(int x, int y, unsigned int height)
{
  while(height > 0)
  {
    SET_PIXEL(x, y, fmtColor_);
    y++;
    height--;
  }
}
