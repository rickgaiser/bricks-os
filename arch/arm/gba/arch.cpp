#include "kernel/task.h"
#include "asm/arch.h"
#include "asm/irq.h"
#include "gbaVideo.h"
#include "gbaKeyboard.h"
//#include "serial.h"


CIRQ           * pIRQ;
CGBAVideo      * pVideo;
CGBAKeyboard   * pKeyboard;
//CGBASerial     * pSerial;



// -----------------------------------------------------------------------------
int
arch::init()
{
  int iRetVal(0);

  pIRQ = new CIRQ;
  if(pIRQ->init() == -1)
    iRetVal = -1;

  pVideo = new CGBAVideo;
  if(pVideo->init() == -1)
    iRetVal = -1;

  pKeyboard = new CGBAKeyboard;
  if(pKeyboard->init() == -1)
    iRetVal = -1;

//  pSerial = new CGBASerial;
//  if(pSerial->init() == -1)
//    iRetVal = -1;

  // Set standard in/out for tasks
  CTask::setStandardOutput(pVideo);
  CTask::setStandardInput(pKeyboard);
  // Create task structure
  CTask * pTask = new CTask(0, 0, 0);
  pTask->eState_ = TS_RUNNING;
  CTask::addTask(pTask);

  return iRetVal;
}
