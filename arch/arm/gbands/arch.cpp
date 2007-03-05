#include "kernel/task.h"
#include "asm/arch.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#ifdef GBA
#include "video.h"
#include "keyboard.h"
#endif
#ifdef NDS7
#include "keyboard.h"
#include "nds/dsIPC.h"
#endif
#ifdef NDS9
#include "video.h"
#include "nds/dsIPC.h"
#endif
#include "timer.h"
//#include "serial.h"
#include "iostream"


CIRQ           cIRQ;
#ifdef GBA
CGBAVideo      cVideo;
CGBAKeyboard   cKeyboard;
#endif
#ifdef NDS7
CGBAKeyboard   cKeyboard;
CDSIPC         cIPC;
#endif
#ifdef NDS9
CGBAVideo      cVideo;
CDSIPC         cIPC;
#endif
//CGBASerial     cSerial;


// -----------------------------------------------------------------------------
int
arch::init()
{
  int iRetVal(0);

  if(cIRQ.init() == -1)
    iRetVal = -1;

#ifdef GBA
  if(cVideo.init() == -1)
    iRetVal = -1;
  CTask::setStandardOutput(&cVideo);

  if(cKeyboard.init() == -1)
    iRetVal = -1;
  CTask::setStandardInput(&cKeyboard);
#endif

#ifdef NDS7
  if(cIPC.init() == -1)
    iRetVal = -1;
  CTask::setStandardOutput(&cIPC);

  if(cKeyboard.init() == -1)
    iRetVal = -1;
  CTask::setStandardInput(&cKeyboard);
#endif

#ifdef NDS9
  if(cVideo.init() == -1)
    iRetVal = -1;
  CTask::setStandardOutput(&cVideo);

  if(cIPC.init() == -1)
    iRetVal = -1;
  CTask::setStandardInput(&cIPC);
#endif

//  pSerial = new CGBASerial;
//  if(pSerial->init() == -1)
//    iRetVal = -1;

  // Create task structure
  CTask cTask(0, 0, 0);
  cTask.eState_ = TS_RUNNING;
  CTask::addTask(&cTask);

  return iRetVal;
}
