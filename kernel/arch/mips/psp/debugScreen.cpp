#include "debugScreen.h"
#include <pspdebug.h>


// -----------------------------------------------------------------------------
CPSPDebugScreen::CPSPDebugScreen()
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CPSPDebugScreen::~CPSPDebugScreen()
{
}

// -----------------------------------------------------------------------------
int
CPSPDebugScreen::init()
{
  pspDebugScreenInit();

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CPSPDebugScreen::write(const void * buffer, size_t size, bool block)
{
  for(size_t i(0); i < size; i++)
    pspDebugScreenPrintf("%c", ((const char *)buffer)[i]);

  return 0;
}
