#include "task.h"
#include "descriptor.h"
#include "kernel/task.h"
#include "string.h"
#include "iostream"


STaskStateSegment tssTemp;
STaskStateSegment taskMain;
selector_t        selTaskMain;
uint8_t           stack_pl0[512];
uint8_t           stack_pl3[512];


// -----------------------------------------------------------------------------
void
init_task()
{
  // Create descriptor for main TSS
  selTaskMain = cGDT.createSegment(dtTSS, 3, (uint32_t)&taskMain, sizeof(STaskStateSegment));
  // Set the current running tasks TSS
  setTR(selTaskMain);
}

// -----------------------------------------------------------------------------
CPCTask::CPCTask()
 : CTask()
{
  cASpace_.init();

  // Locate virtual memory for TSS
  pTSS_ = &tssTemp;
  memset(pTSS_, 0, sizeof(STaskStateSegment));
  pTSS_->esp0 = (uint32_t)&stack_pl0 + 512;
  pTSS_->ss0  = selDataKernel;
  pTSS_->esp  = (uint32_t)&stack_pl3 + 512;
  pTSS_->cr3  = cASpace_.cr3();
  //pTSS_->eip  = (uint32_t)ip;
  pTSS_->eflags = 0x200;
  pTSS_->es   = selDataUserTmp;
  pTSS_->cs   = selCodeUserTmp;
  pTSS_->ss   = selDataUserTmp;
  pTSS_->ds   = selDataUserTmp;
  pTSS_->fs   = selDataUserTmp;
  pTSS_->gs   = selDataUserTmp;
  
  // Create descriptor for TSS
  selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)pTSS_, sizeof(STaskStateSegment));
}

// -----------------------------------------------------------------------------
CPCTask::~CPCTask()
{
}

// -----------------------------------------------------------------------------
void
CPCTask::entry(void * ip)
{
  pEntry_ = ip;
  pTSS_->eip  = (uint32_t)ip;
}
/*
// -----------------------------------------------------------------------------
void
CPCTask::stack(void * sp)
{
  pStack_ = sp;
  pTSS_->esp = (uint32_t)sp;
}
*/
// -----------------------------------------------------------------------------
void
CPCTask::run()
{
  // Jump to task
  jumpSelector(selTSS_);
}

// -----------------------------------------------------------------------------
void
CPCTask::kill()
{
  std::cout<<"suicide!"<<std::endl;
  CTaskManager::removeTask(CTaskManager::pCurrentTask_);
}

// -----------------------------------------------------------------------------
CTask *
getNewTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CPCTask();
}
