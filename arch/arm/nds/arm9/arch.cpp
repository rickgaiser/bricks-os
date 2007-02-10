#include "asm/arch.h"
#include "asm/irq.h"
#include "ds9Video.h"
#include "dsKeyboard.h"
#include "dsCart.h"
//#include "dsTimer.h"
#include "dsIPC.h"


CIRQ              cIRQ;
//CDSTimerManager   cTimerManager;
CDS9Video         cVideo;
CDSKeyboard       cKeyboard;
CDSCart           cCart;
CDSIPC            cIPC;

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
//  if(cTimerManager.init() == -1)
//    iRetVal = -1;
  if(cVideo.init() == -1)
    iRetVal = -1;
  if(cKeyboard.init() == -1)
    iRetVal = -1;
  if(cCart.init() == -1)
    iRetVal = -1;
  if(cIPC.init() == -1)
    iRetVal = -1;

  return iRetVal;
}
