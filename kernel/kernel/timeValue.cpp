#include "kernel/timeValue.h"


// -----------------------------------------------------------------------------
CTimeValue::CTimeValue()
 : iSec_ (0)
 , iUSec_(0)
{
}

// -----------------------------------------------------------------------------
CTimeValue::CTimeValue(long sec, long usec)
 : iSec_ (sec)
 , iUSec_(usec)
{
}

// -----------------------------------------------------------------------------
void
CTimeValue::set(long sec, long usec)
{
  iSec_  = sec;
  iUSec_ = usec;
}

// -----------------------------------------------------------------------------
long
CTimeValue::msec() const
{
  return(iSec_ * 1000 + iUSec_ / 1000);
}

// -----------------------------------------------------------------------------
void
CTimeValue::msec(long msec)
{
  iSec_  = msec / 1000;
  iUSec_ = (msec - (iSec_ * 1000)) * 1000;
}

// -----------------------------------------------------------------------------
void
CTimeValue::normalize()
{
  if(iUSec_ >= 1000000)
  {
    do
    {
      iSec_++;
      iUSec_ -= 1000000;
    }
    while(iUSec_ >= 1000000);
  }
  else if(iUSec_ <= -1000000)
  {
    do
    {
      iSec_--;
      iUSec_ += 1000000;
    }
    while (iUSec_ <= -1000000);
  }

  if(iSec_ >= 1 && iUSec_ < 0)
  {
    iSec_--;
    iUSec_ += 1000000;
  }
  else if (iSec_ < 0 && iUSec_ > 0)
  {
    iSec_++;
    iUSec_ -= 1000000;
  }
}
