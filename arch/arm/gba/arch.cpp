#include "asm/arch.h"
#include "asm/irq.h"
#include "gbaVideo.h"
#include "gbaKeyboard.h"


CIRQ           * pGBAIRQ;
CGBAVideo      * pGBAVideo;
CGBAKeyboard   * pGBAKeyboard;

extern IFileIO * pKeyboard;
extern IFileIO * pVideo;


// -----------------------------------------------------------------------------
int
arch::init()
{
  int iRetVal(0);

  pGBAVideo = new CGBAVideo;
  if(pGBAVideo->init() == -1)
    iRetVal = -1;
  pVideo = pGBAVideo;

  pGBAIRQ = new CIRQ;
  if(pGBAIRQ->init() == -1)
    iRetVal = -1;

  pGBAKeyboard = new CGBAKeyboard;
  if(pGBAKeyboard->init() == -1)
    iRetVal = -1;
  pKeyboard = pGBAKeyboard;

  return iRetVal;
}
