#include "i386Arch.h"


extern IFileIO * pKeyboard;
extern IFileIO * pVideo;


// -----------------------------------------------------------------------------
CI386Arch::CI386Arch()
{
  pVideo    = &cVideo_;
  pKeyboard = &cKeyboard_;
}

// -----------------------------------------------------------------------------
CI386Arch::~CI386Arch()
{
}

// -----------------------------------------------------------------------------
int
CI386Arch::init()
{
  int iRetVal(0);

  if(cGDT_.init() == -1)
    iRetVal = -1;
  if(cIDT_.init() == -1)
    iRetVal = -1;
  if(cIRQ_.init() == -1)
    iRetVal = -1;
  if(cVideo_.init() == -1)
    iRetVal = -1;
  if(cKeyboard_.init() == -1)
    iRetVal = -1;

  return(iRetVal);
}
