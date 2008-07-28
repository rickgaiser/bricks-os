#include "kernel/video.h"


// -----------------------------------------------------------------------------
CAVideo::CAVideo(unsigned int iX, unsigned int iY)
 : iX_(iX)
 , iY_(iY)
 , iCurrentX_(0)
 , iCurrentY_(0)
{
}

// -----------------------------------------------------------------------------
CAVideo::~CAVideo()
{
}

// -----------------------------------------------------------------------------
void
CAVideo::cls()
{
  for(unsigned int y(0); y < iY_; y++)
  {
    for(unsigned int x(0); x < iX_; x++)
    {
      put(x, y, ' ');
    }
  }
}

// -----------------------------------------------------------------------------
ssize_t
CAVideo::write(const void * buffer, size_t size, bool block)
{
  for(size_t i(0); i < size; i++)
  {
    this->put(((const char *)buffer)[i]);
  }

  return(0);
}

// -----------------------------------------------------------------------------
void
CAVideo::put(char c)
{
  switch(c)
  {
  case '\n':
    iCurrentX_ = 0;
    iCurrentY_++;
    break;
  case '\r':
    iCurrentX_ = 0;
    break;
  default:
    this->put(iCurrentX_, iCurrentY_, c);
    iCurrentX_++;
  }

  // Make sure our line/char point to a real char location
  if(iCurrentX_ >= iX_)
  {
    iCurrentX_ = 0;
    iCurrentY_++;
  }
  if(iCurrentY_ >= iY_)
  {
    this->scrollUp();
    iCurrentY_--;
  }
}

// -----------------------------------------------------------------------------
void
CAVideo::put(const char * s)
{
  for(; *s != 0; s++)
  {
    this->put(*s);
  }
}

// -----------------------------------------------------------------------------
void
CAVideo::scrollUp()
{
  // Copy lines
  for(unsigned int y(0); y != (iY_ - 1); y++)
    for(unsigned int x(0); x != iX_; x++)
      put(x, y, get(x, y + 1));

  // Clear last line
  for(unsigned int x(0); x != iX_; x++)
    put(x, iY_ - 1, ' ');
}
