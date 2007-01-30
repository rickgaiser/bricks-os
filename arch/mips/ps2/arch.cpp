#include "asm/arch.h"
#include "ps2Video.h"


IFileIO          cDummy;
CPS2Video        cVideo;

extern IFileIO * pKeyboard;
extern IFileIO * pVideo;


// -----------------------------------------------------------------------------
int
arch::init()
{
  int iRetVal(0);

  pVideo    = &cVideo;
  pKeyboard = &cDummy;

  if(cVideo.init() == -1)
    iRetVal = -1;

  return iRetVal;
}
