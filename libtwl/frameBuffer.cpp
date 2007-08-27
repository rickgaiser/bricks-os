#include "frameBuffer.h"


extern const unsigned short font3x5[];


namespace twl
{


//---------------------------------------------------------------------------
CSurface::CSurface()
 : eType_(btDOUBLE)
 , pVideoBuffer_(0)
 , pBackBuffer_(0)
 , pTrippleBuffer_(0)
 , pDrawBuffer_(0)
{
}

//---------------------------------------------------------------------------
CSurface::~CSurface()
{
}

//---------------------------------------------------------------------------
const CRect &
CSurface::rect() const
{
  return cScreenRect_;
}

//---------------------------------------------------------------------------
pixel_t &
CSurface::pixel(int x, int y)
{
  if(x >= (int)iWidth_)
    x = iWidth_ - 1;
  if(y >= (int)iHeight_)
    y = iHeight_ - 1;

  return pDrawBuffer_[x + (y * iWidth_)];
}

//---------------------------------------------------------------------------
void
CSurface::fill(color_t color)
{
  fillRect(0, 0, iWidth_, iHeight_, color);
}

//---------------------------------------------------------------------------
void
CSurface::fillRect(int x, int y, int width, int height, color_t color)
{
  CRect fillRect(x, y, width, height);
  fillRect.clip(cScreenRect_);

  if(fillRect.valid() == true)
  {
    int iBottom(fillRect.bottom());
    int iIndex (fillRect.top() * iWidth_);

    for(int y(fillRect.top()); y <= iBottom; y++)
    {
      // Fill line
      fastFill(color, &(pDrawBuffer_[iIndex + fillRect.left()]), fillRect.width());
      // Advance to next line
      iIndex += iWidth_;
    }
  }
}

//---------------------------------------------------------------------------
void
CSurface::drawHLine(int y, int left, int right, color_t color)
{
  fillRect(left, y, right - left + 1, 1, color);
}

//---------------------------------------------------------------------------
void
CSurface::drawVLine(int x, int top, int bottom, color_t color)
{
  fillRect(x, top, 1, bottom - top + 1, color);
}

//---------------------------------------------------------------------------
void
CSurface::drawText(int x, int y, const char * string, color_t color)
{
  for(int i(0); string[i] != 0; i++)
  {
    int iFontOffset(string[i] * 4 * 6);

    for(int xpos(0); xpos < 4; xpos++)
      for(int ypos(0); ypos < 6; ypos++)
        if(font3x5[ypos * 4 + xpos + iFontOffset] != 0)
          pDrawBuffer_[((ypos + y) * iWidth_) + xpos + x] = color;

    x += 4;
  }
}

//---------------------------------------------------------------------------
void
CSurface::swap(bool forceCopy)
{
  // No need to check "forceCopy", we always copy
  switch(eType_)
  {
  case btSINGLE:
    break;
  case btDOUBLE:
    fastCopy(pBackBuffer_, pVideoBuffer_, iWidth_ * iHeight_);
  };
}

//---------------------------------------------------------------------------
void
CSurface::fastFill(color_t color, pixel_t * dest, uint32_t size)
{
  for(uint32_t i(0); i < size; i++)
    dest[i] = color;
}

//---------------------------------------------------------------------------
void
CSurface::fastCopy(pixel_t * src, pixel_t * dest, uint32_t size)
{
  for(uint32_t i(0); i < size; i++)
    dest[i] = src[i];
}


};
