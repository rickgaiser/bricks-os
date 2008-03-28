#include "rect.h"


namespace twl
{


//---------------------------------------------------------------------------
CRect::CRect()
 : iX_     (0)
 , iY_     (0)
 , iWidth_ (1)
 , iHeight_(1)
{
}

//---------------------------------------------------------------------------
CRect::CRect(int x, int y, int width, int height)
 : iX_     (x)
 , iY_     (y)
 , iWidth_ (width)
 , iHeight_(height)
{
}

//---------------------------------------------------------------------------
CRect::~CRect()
{
}

//---------------------------------------------------------------------------
int
CRect::x() const
{
  return iX_;
}

//---------------------------------------------------------------------------
int
CRect::y() const
{
  return iY_;
}

//---------------------------------------------------------------------------
int
CRect::width() const
{
  return iWidth_;
}

//---------------------------------------------------------------------------
int
CRect::height() const
{
  return iHeight_;
}

//---------------------------------------------------------------------------
int
CRect::left() const
{
  return iX_;
}

//---------------------------------------------------------------------------
int
CRect::top() const
{
  return iY_;
}

//---------------------------------------------------------------------------
int
CRect::right() const
{
  return iX_ + iWidth_;
}

//---------------------------------------------------------------------------
int
CRect::bottom() const
{
  return iY_ + iHeight_;
}

//---------------------------------------------------------------------------
void
CRect::x(int x)
{
  iX_ = x;
}

//---------------------------------------------------------------------------
void
CRect::y(int y)
{
  iY_ = y;
}

//---------------------------------------------------------------------------
void
CRect::width(int width)
{
  iWidth_ = width;
}

//---------------------------------------------------------------------------
void
CRect::height(int height)
{
  iHeight_ = height;
}

//---------------------------------------------------------------------------
void
CRect::rect(int x, int y, int width, int height)
{
  iX_      = x;
  iY_      = y;
  iWidth_  = width;
  iHeight_ = height;
}

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
