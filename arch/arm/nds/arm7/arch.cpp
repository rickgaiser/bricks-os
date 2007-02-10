#include "asm/arch.h"
#include "asm/irq.h"
#include "../common/dsIPC.h"
#include "../common/dsKeyboard.h"


//CIRQ             cIRQ;
//CDSIPC           cIPC;
//CDSKeyboard      cKeyboard;

extern IFileIO * pKeyboard;
extern IFileIO * pVideo;


// -----------------------------------------------------------------------------
int
arch::init()
{
  int iRetVal(0);

//  pVideo    = &cIPC;
//  pKeyboard = &cKeyboard;

//  if(cIRQ.init() == -1)
//    iRetVal = -1;
//  if(cIPC.init() == -1)
//    iRetVal = -1;
//  if(cKeyboard.init() == -1)
//    iRetVal = -1;

  return iRetVal;
}
