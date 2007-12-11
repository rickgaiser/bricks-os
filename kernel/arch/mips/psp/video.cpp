#include "video.h"


// -----------------------------------------------------------------------------
CPSPVideo::CPSPVideo()
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CPSPVideo::~CPSPVideo()
{
}

// -----------------------------------------------------------------------------
int
CPSPVideo::init()
{
  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CPSPVideo::write(const void * buffer, size_t size, loff_t *)
{
  return 0;
}
