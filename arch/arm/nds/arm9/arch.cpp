#include "kernel/task.h"
#include "asm/arch.h"
#include "asm/irq.h"
#include "ds9Video.h"
#include "dsKeyboard.h"
//#include "dsCart.h"
//#include "dsTimer.h"
//#include "dsIPC.h"


CIRQ           * pIRQ;
//CDSTimerManager * cTimerManager;
CDS9Video      * pVideo;
CDSKeyboard    * pKeyboard;
//CDSCart        * pCart;
//CDSIPC         * pIPC;


// -----------------------------------------------------------------------------
int
arch::init()
{
  int iRetVal(0);

  pIRQ = new CIRQ;
  if(pIRQ->init() == -1)
    iRetVal = -1;

  pVideo = new CDS9Video;
  if(pVideo->init() == -1)
    iRetVal = -1;

  pKeyboard = new CDSKeyboard;
  if(pKeyboard->init() == -1)
    iRetVal = -1;

//  if(cTimerManager.init() == -1)
//    iRetVal = -1;
//  if(cCart.init() == -1)
//    iRetVal = -1;
//  if(cIPC.init() == -1)
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
