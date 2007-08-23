#include "video.h"


// -----------------------------------------------------------------------------
CI386Video::CI386Video()
 : CAVideo(CHARS_PER_LINE, LINE_COUNT)
 , pVideo_(VIDEO_START)
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CI386Video::~CI386Video()
{
}

// -----------------------------------------------------------------------------
int
CI386Video::init()
{
  this->cls();

  return 0;
}

// -----------------------------------------------------------------------------
void
CI386Video::put(int iX, int iY, char c)
{
  pVideo_[(iY * CHARS_PER_LINE + iX) * 2] = c;
}

// -----------------------------------------------------------------------------
char
CI386Video::get(int iX, int iY)
{
  return pVideo_[(iY * CHARS_PER_LINE + iX) * 2];
}
