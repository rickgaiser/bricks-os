#include "task.h"
#include "descriptor.h"
#include "kernel/task.h"
#include "string.h"
#include "iostream"


extern bool bPAEEnabled;

CPCTask    taskMain;
uint8_t    stack_pl0[512];
uint8_t    stack_pl3[512];


// -----------------------------------------------------------------------------
void
init_task()
{
  taskMain.cASpace_.init();
  taskMain.selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)&taskMain.tss_, sizeof(STaskStateSegment));
  taskMain.bInitialized_ = true;
  taskMain.cASpace_.identityMap(0, 0x00400000);  // Identity Map first 4MiB

  // Enable PAE
  if(bPAEEnabled == true)
    setCR4(getCR4() | CR4_PAE);

  // Load CR3
  setCR3(taskMain.cASpace_.cr3());

  // Enable paging
  setCR0(getCR0() | CR0_PG);

  // Set the current running tasks TSS
  setTR(taskMain.selTSS_);
  
  // Add task to taskmanagers list
  taskMain.eState_ = TS_READY;
  CTaskManager::addTask(&taskMain);
}

// -----------------------------------------------------------------------------
CPCTask::CPCTask()
 : CTask()
 , bInitialized_(false)
{
}

// -----------------------------------------------------------------------------
CPCTask::~CPCTask()
{
}

// -----------------------------------------------------------------------------
void
CPCTask::init()
{
  if(bInitialized_ == false)
  {
    cASpace_.init();

    // Locate virtual memory for TSS
    memset(&tss_, 0, sizeof(STaskStateSegment));
    tss_.esp0 = (uint32_t)&stack_pl0 + 512;
    tss_.ss0  = selDataKernel;
    tss_.esp  = (uint32_t)&stack_pl3 + 512;
    tss_.cr3  = cASpace_.cr3();
    //tss_.eip  = (uint32_t)ip;
    tss_.eflags = 0x200;
    tss_.es   = selDataUserTmp;
    tss_.cs   = selCodeUserTmp;
    tss_.ss   = selDataUserTmp;
    tss_.ds   = selDataUserTmp;
    tss_.fs   = selDataUserTmp;
    tss_.gs   = selDataUserTmp;
  
    // Create descriptor for TSS
    selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)&tss_, sizeof(STaskStateSegment));

    bInitialized_ = true;
  }
}

// -----------------------------------------------------------------------------
void
CPCTask::entry(void * ip)
{
  pEntry_ = ip;
  tss_.eip  = (uint32_t)ip;
}
/*
// -----------------------------------------------------------------------------
void
CPCTask::stack(void * sp)
{
  pStack_ = sp;
  tss_.esp = (uint32_t)sp;
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
CTask *
getNewTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CPCTask();
}
