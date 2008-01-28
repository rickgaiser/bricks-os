#include "task.h"
#include "descriptor.h"
#include "kernel/task.h"
#include "string.h"
#include "asm/arch/config.h"


extern bool bPAEEnabled;


CTask      * pMainTask;
CPCThread  * pMainThread;


// -----------------------------------------------------------------------------
void
CPCThread::init()
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
 , pStack_(NULL)
 , pSvcStack_(NULL)
{
  // Main thread?
  if(entry != NULL)
  {
    // Set default stack values
    if(stack == 0)    stack    = 512;
    if(svcstack == 0) svcstack = 512;

    // Allocate stacks
    pStack_ = new uint32_t[stack];
    pSvcStack_ = new uint32_t[svcstack];

    // Locate virtual memory for TSS
    memset(&tss_, 0, sizeof(STaskStateSegment));
    tss_.esp0 = (uint32_t)(pSvcStack_ + svcstack);
    tss_.ss0  = selDataKernel;
    ((uint32_t *)pStack_)[(512 >> 2) - 1] = (uint32_t)argv;
    ((uint32_t *)pStack_)[(512 >> 2) - 2] = (uint32_t)argc;
    ((uint32_t *)pStack_)[(512 >> 2) - 3] = 0; // Function call return address
    tss_.esp  = (uint32_t)pStack_ + stack - 12;
    tss_.cr3  = cASpace_.cr3();
    tss_.eip  = (uint32_t)entry;
    tss_.eflags = 0x200;
#ifdef CONFIG_DIRECT_ACCESS_KERNEL
    tss_.es   = selDataKernel;
    tss_.cs   = selCodeKernel;
    tss_.ss   = selDataKernel;
    tss_.ds   = selDataKernel;
    tss_.fs   = selDataKernel;
    tss_.gs   = selDataKernel;
#else
    tss_.es   = selDataUserTmp;
    tss_.cs   = selCodeUserTmp;
    tss_.ss   = selDataUserTmp;
    tss_.ds   = selDataUserTmp;
    tss_.fs   = selDataUserTmp;
    tss_.gs   = selDataUserTmp;
#endif

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
