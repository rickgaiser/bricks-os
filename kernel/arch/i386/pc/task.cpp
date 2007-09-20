#include "task.h"
#include "descriptor.h"
#include "kernel/task.h"
#include "string.h"


extern bool bPAEEnabled;


CTask      * pMainTask;
CPCThread  * pMainThread;


// -----------------------------------------------------------------------------
void
init_task()
{
  pMainTask   = new CTask(0, 0, 0);
  pMainThread = (CPCThread *)pMainTask->thr_;

  pMainThread->selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)&pMainThread->tss_, sizeof(STaskStateSegment));
  pMainThread->cASpace_.identityMap(0, 0x00400000);  // Identity Map first 4MiB

  // Enable PAE
  if(bPAEEnabled == true)
    setCR4(getCR4() | CR4_PAE);

  // Load CR3
  setCR3(pMainThread->cASpace_.cr3());

  // Enable paging
  setCR0(getCR0() | CR0_PG);

  // Set the current running tasks TSS
  setTR(pMainThread->selTSS_);

  // Add task to taskmanagers list
  pMainThread->state(TS_RUNNING);
}

// -----------------------------------------------------------------------------
CPCThread::CPCThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CThread(task)
{
  // Main thread?
  if(entry != NULL)
  {
    // Locate virtual memory for TSS
    memset(&tss_, 0, sizeof(STaskStateSegment));
    if(svcstack != 0)
      tss_.esp0 = (uint32_t)(new uint8_t[svcstack] + svcstack);
    else
      tss_.esp0 = (uint32_t)(new uint8_t[512] + 512);
    tss_.ss0  = selDataKernel;
    if(stack != 0)
      tss_.esp  = (uint32_t)(new uint8_t[stack] + stack);
    else
      tss_.esp  = (uint32_t)(new uint8_t[512] + 512);
    tss_.cr3  = cASpace_.cr3();
    tss_.eip  = (uint32_t)entry;
    tss_.eflags = 0x200;
    tss_.es   = selDataUserTmp;
    tss_.cs   = selCodeUserTmp;
    tss_.ss   = selDataUserTmp;
    tss_.ds   = selDataUserTmp;
    tss_.fs   = selDataUserTmp;
    tss_.gs   = selDataUserTmp;

    // Create descriptor for TSS
    selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)&tss_, sizeof(STaskStateSegment));
    // Identity map bottom 4MiB
    aspace().addRange(pMainThread->aspace(), 0, 0x00400000);
  }
}

// -----------------------------------------------------------------------------
CPCThread::~CPCThread()
{
}

// -----------------------------------------------------------------------------
void
CPCThread::runJump()
{
  // Jump to task
  jumpSelector(selTSS_);
}

// -----------------------------------------------------------------------------
CThread *
getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  CPCThread * pt = new CPCThread(task, entry, stack, svcstack, argc, argv);

  return pt;
}
