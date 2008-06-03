#include "rect.h"


namespace twl
{


//---------------------------------------------------------------------------
bool
CRect::valid()
{
  return ((iWidth_ > 0) && (iHeight_ > 0));
}

//---------------------------------------------------------------------------
bool
CRect::contains(const CRect & rect)
{
  return (contains(rect.x(), rect.y()) && contains(rect.right() - 1, rect.bottom() - 1));
}

//---------------------------------------------------------------------------
bool
CRect::contains(int x, int y)
{
  return ((x >= iX_) && (x < iX_ + iWidth_) && (y >= iY_) && (y < iY_ + iHeight_));
}

//---------------------------------------------------------------------------
bool
CRect::intersects(const CRect & rect)
{
  return (contains(rect.x(), rect.y()) || contains(rect.right() - 1, rect.bottom() - 1));
}

//---------------------------------------------------------------------------
void
CRect::clip(const CRect & rect)
{
//  if(intersects(rect) == false)
//  {
    // Invalidate rect
//    *this = CRect(0, 0, 0, 0);
//  }
//  else
  {
    // Clip left
    if(iX_ < rect.iX_)
    {
      iWidth_ -= (rect.iX_ - iX_);
      iX_ = rect.iX_;
    }
    // Clip top
    if(iY_ < rect.iY_)
    {
      iHeight_ -= (rect.iY_ - iY_);
      iY_ = rect.iY_;
    }
    // Clip right
    if((iX_ + iWidth_) > rect.right())
      iWidth_ = rect.right() - iX_;
    // Clip bottom
    if((iY_ + iHeight_) > rect.bottom())
      iHeight_ = rect.bottom() - iY_;
  }
}


};
