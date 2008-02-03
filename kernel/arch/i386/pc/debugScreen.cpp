#include "debugScreen.h"


// -----------------------------------------------------------------------------
CI386DebugScreen::CI386DebugScreen()
 : CAVideo(CHARS_PER_LINE, LINE_COUNT)
 , pVideo_(VIDEO_START)
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CI386DebugScreen::~CI386DebugScreen()
{
}

// -----------------------------------------------------------------------------
int
CI386DebugScreen::init()
{
  this->cls();

  return 0;
}

// -----------------------------------------------------------------------------
void
CI386DebugScreen::put(int iX, int iY, char c)
{
  pVideo_[(iY * CHARS_PER_LINE + iX) * 2] = c;
}

// -----------------------------------------------------------------------------
char
CI386DebugScreen::get(int iX, int iY)
{
  return pVideo_[(iY * CHARS_PER_LINE + iX) * 2];
}
