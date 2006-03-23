#include "ps2Arch.h"


extern IFileIO * pKeyboard;
extern IFileIO * pVideo;


// -----------------------------------------------------------------------------
CPS2Arch::CPS2Arch()
{
  pVideo    = &cVideo_;
  pKeyboard = &cDummy_;
}

// -----------------------------------------------------------------------------
CPS2Arch::~CPS2Arch()
{
}

// -----------------------------------------------------------------------------
int
CPS2Arch::init()
{
  int iRetVal(0);

  if(cVideo_.init() == -1)
    iRetVal = -1;

  return(iRetVal);
}
