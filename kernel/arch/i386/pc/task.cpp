#include "task.h"
#include "descriptor.h"
#include "kernel/task.h"
#include "kernel/debug.h"
#include "string.h"
#include "asm/arch/config.h"
#include "hal.h"
#include "mmap.h"


extern bool bPAEEnabled;


CTask      * pMainTask;
CPCThread  * pMainThread;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
CPCThread::init()
{
  pMainTask   = new CTask(0, 0, 0);
  pMainThread = (CPCThread *)pMainTask->thr_;

  pMainThread->selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)pMainThread->pTSS_, pMainThread->iTSSSize_);
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
  // Initialize TSS
  iTSSSize_ = sizeof(STaskStateSegment);
  pTSS_ = (STaskStateSegment *)new uint8_t[iTSSSize_];
  memset(pTSS_, 0, iTSSSize_);

  // Main thread?
  if(entry != NULL)
  {
    // Set default stack values
    if(stack == 0)    stack    = 512;
    if(svcstack == 0) svcstack = 512;

    // Allocate stacks
    pStack_ = new uint32_t[stack];
    pSvcStack_ = new uint32_t[svcstack];

    // SVC stack
    pTSS_->esp0 = (uint32_t)(pSvcStack_ + svcstack);
    pTSS_->ss0  = selDataKernel;
    // User stack
    ((uint32_t *)pStack_)[(512 >> 2) - 1] = (uint32_t)argv;
    ((uint32_t *)pStack_)[(512 >> 2) - 2] = (uint32_t)argc;
    ((uint32_t *)pStack_)[(512 >> 2) - 3] = 0; // Function call return address
    pTSS_->esp  = (uint32_t)pStack_ + stack - 12;
    // Segments
#ifdef CONFIG_DIRECT_ACCESS_KERNEL
    pTSS_->es   = selDataKernel;
    pTSS_->cs   = selCodeKernel;
    pTSS_->ss   = selDataKernel;
    pTSS_->ds   = selDataKernel;
    pTSS_->fs   = selDataKernel;
    pTSS_->gs   = selDataKernel;
#else
    pTSS_->es   = selDataUserTmp;
    pTSS_->cs   = selCodeUserTmp;
    pTSS_->ss   = selDataUserTmp;
    pTSS_->ds   = selDataUserTmp;
    pTSS_->fs   = selDataUserTmp;
    pTSS_->gs   = selDataUserTmp;
#endif
    // Other
    pTSS_->cr3  = cASpace_.cr3();
    pTSS_->eip  = (uint32_t)entry;
    pTSS_->eflags = I386_IOPL_VALUE(0) | I386_IE_FLAG | I386_ON_FLAGS;

    // Create descriptor for TSS
    selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)pTSS_, iTSSSize_);
    // Identity map bottom 4MiB
    cASpace_.addRange(pMainThread->aspace(), 0, 0x00400000);
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
// -----------------------------------------------------------------------------
CV86Thread::CV86Thread()
{
  // Stack sizes
  //size_t stack    = 512;
  size_t svcstack = 512;

  // Allocate stacks
  pStack_    = (uint32_t *)physAllocPage();
  pSvcStack_ = new uint32_t[svcstack];

  // Initialize TSS
  iTSSSize_ = sizeof(STaskStateSegment) + (64*1024/8);
  pTSS_ = (STaskStateSegment *)new uint8_t[iTSSSize_];
  memset(pTSS_, 0, iTSSSize_);
  // IO permission bitmap
  pTSS_->io_map_addr = sizeof(STaskStateSegment);
  // SVC stack
  pTSS_->esp0 = (uint32_t)(pSvcStack_ + svcstack);
  pTSS_->ss0  = selDataKernel;
  // User stack xxxx:0fff, 4KiB
  pTSS_->esp  = 0x0fff; //(uint32_t)pStack_ + stack;
  pTSS_->ss   = (uint32_t)pStack_ >> 4;
  // Segments
  pTSS_->es   = 0;
//  pTSS_->cs   = ...;
  pTSS_->ds   = 0;
  pTSS_->fs   = 0;
  pTSS_->gs   = 0;
  // Other
  pTSS_->cr3  = cASpace_.cr3();
//  pTSS_->eip  = ...;
  pTSS_->eflags = I386_VM_FLAG | I386_IOPL_VALUE(0) | I386_IE_FLAG | I386_ON_FLAGS;

  // Create descriptor for TSS
  selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)pTSS_, iTSSSize_);
  // Identity map bottom 4MiB
  cASpace_.addRange(pMainThread->aspace(), 0, 0x00400000);
}

// -----------------------------------------------------------------------------
CV86Thread::~CV86Thread()
{
}

// -----------------------------------------------------------------------------
void
CV86Thread::interrupt(uint8_t nr)
{
  uint16_t * base = 0;

  // Get interrupt vector
  pTSS_->cs  = base[(nr & 0xff) * 2 + 1];
  pTSS_->eip = base[(nr & 0xff) * 2 + 0];

  //printk("int 0x%x @ %x:%x = 0x%x\n", nr, pTSS_->cs, pTSS_->eip, from_v86_addr(pTSS_->cs, pTSS_->eip));

  // Jump to task
  jumpSelector(selTSS_);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CThread *
getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  CPCThread * pt = new CPCThread(task, entry, stack, svcstack, argc, argv);

  return pt;
}
