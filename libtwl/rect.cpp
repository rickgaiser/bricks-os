#include "rect.h"


namespace twl
{


//---------------------------------------------------------------------------
CRect::CRect()
 : iLeft_  (0)
 , iTop_   (0)
 , iWidth_ (1)
 , iHeight_(1)
{
}

//---------------------------------------------------------------------------
CRect::CRect(int left, int top, int width, int height)
 : iLeft_  (left)
 , iTop_   (top)
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
CRect::left() const
{
  return iLeft_;
}

//---------------------------------------------------------------------------
int
CRect::top() const
{
  return iTop_;
}

//---------------------------------------------------------------------------
int
CRect::right() const
{
  return iLeft_ + iWidth_ - 1;
}

//---------------------------------------------------------------------------
int
CRect::bottom() const
{
  return iTop_ + iHeight_ - 1;
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
void
CRect::left(int left)
{
  iWidth_ -= left - iLeft_;

  iLeft_ = left;
}

//---------------------------------------------------------------------------
void
CRect::top(int top)
{
  iHeight_ -= top - iTop_;

  iTop_ = top;
}

//---------------------------------------------------------------------------
void
CRect::right(int right)
{
  iWidth_ = right - iLeft_ + 1;
}

//---------------------------------------------------------------------------
void
CRect::bottom(int bottom)
{
  iHeight_ = bottom - iTop_ + 1;
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
CRect::move(int right, int down)
{
  iLeft_ += right;
  iTop_ += down;
}

//---------------------------------------------------------------------------
void
CRect::moveUp(int pixels)
{
  iTop_ -= pixels;
}

//---------------------------------------------------------------------------
void
CRect::moveDown(int pixels)
{
  iTop_ += pixels;
}

//---------------------------------------------------------------------------
void
CRect::moveLeft(int pixels)
{
  iLeft_ -= pixels;
}

//---------------------------------------------------------------------------
void
CRect::moveRight(int pixels)
{
  iLeft_ += pixels;
}

//---------------------------------------------------------------------------
void
CRect::rect(int left, int top, int width, int height)
{
  iLeft_   = left;
  iTop_    = top;
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
  return (contains(rect.left(), rect.top()) && contains(rect.right(), rect.bottom()));
}

//---------------------------------------------------------------------------
bool
CRect::contains(int x, int y)
{
  return ((x >= iLeft_) && (x < iLeft_ + iWidth_) && (y >= iTop_) && (y < iTop_ + iHeight_));
}

//---------------------------------------------------------------------------
bool
CRect::intersects(const CRect & rect)
{
  return (contains(rect.left(), rect.top()) || contains(rect.right(), rect.bottom()));
}

//---------------------------------------------------------------------------
void
CRect::clip(const CRect & rect)
{
  if((iLeft_ > rect.right()) || (iTop_ > rect.bottom()))
  {
    // Invalidate rect
    *this = CRect(0, 0, 0, 0);
  }
  else
  {
    // Clip left
    if(iLeft_ < rect.iLeft_)
      left(rect.iLeft_);
    // Clip top
    if(iTop_ < rect.iTop_)
      top(rect.iTop_);
    // Clip right
    if(right() > rect.right())
      right(rect.right());
    // Clip bottom
    if(bottom() > rect.bottom())
      bottom(rect.bottom());
  }
}


};
