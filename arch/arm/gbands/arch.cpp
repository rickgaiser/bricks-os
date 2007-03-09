#include "kernel/task.h"
#include "asm/arch.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#ifdef GBA
#include "video.h"
#include "videoDevice.h"
#include "keyboard.h"
#include "gba/serial.h"
#endif // GBA
#ifdef NDS7
#include "keyboard.h"
#include "nds/dsIPC.h"
#endif // NDS7
#ifdef NDS9
#include "video.h"
#include "videoDevice.h"
#include "nds/dsIPC.h"
#endif // NDS 9
#include "timer.h"
#include "iostream"


CIRQ           cIRQ;
#ifdef GBA
CGBAVideo      cVideo;
CGBAVideoDevice * pVideoDevice;
CGBAKeyboard   cKeyboard;
CGBASerial     cSerial;
#endif // GBA
#ifdef NDS7
CGBAKeyboard   cKeyboard;
CDSIPC         cIPC;
#endif // NDS7
#ifdef NDS9
CGBAVideo      cVideo;
CGBAVideoDevice * pVideoDevice;
CDSIPC         cIPC;
#endif // NDS9


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

//  if(cSerial.init() == -1)
//    iRetVal = -1;

  pVideoDevice = new CGBAVideoDevice;
#endif // GBA

#ifdef NDS7
  if(cIPC.init() == -1)
    iRetVal = -1;
  CTask::setStandardOutput(&cIPC);

  if(cKeyboard.init() == -1)
    iRetVal = -1;
  CTask::setStandardInput(&cKeyboard);
#endif // NDS7

#ifdef NDS9
  if(cVideo.init() == -1)
    iRetVal = -1;
  CTask::setStandardOutput(&cVideo);

  if(cIPC.init() == -1)
    iRetVal = -1;
  CTask::setStandardInput(&cIPC);

  pVideoDevice  = new CGBAVideoDevice;
#endif // NDS9

  // Create task structure
  CTask * pTask = new CTask(0, 0, 0);
  pTask->eState_ = TS_RUNNING;
  CTask::addTask(pTask);

  setTimerFrequency(0, 100.0f);
  cIRQ.enable(3);

  return iRetVal;
}
