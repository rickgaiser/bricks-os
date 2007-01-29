#include "asm/arch.h"
#include "asm/irq.h"
#include "gbaVideo.h"
#include "gbaKeyboard.h"


CIRQ             cIRQ;
CGBAVideo        cVideo;
CGBAKeyboard     cKeyboard;

extern IFileIO * pKeyboard;
extern IFileIO * pVideo;


// -----------------------------------------------------------------------------
int
arch::init()
{
  int iRetVal(0);

  pVideo    = &cVideo;
  pKeyboard = &cKeyboard;

  if(cIRQ.init() == -1)
    iRetVal = -1;
  if(cVideo.init() == -1)
    iRetVal = -1;
  if(cKeyboard.init() == -1)
    iRetVal = -1;

  return iRetVal;
}
