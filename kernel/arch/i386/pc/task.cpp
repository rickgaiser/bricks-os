#include "asm/task.h"
#include "descriptor.h"
#include "kernel/task.h"
#include "kernel/debug.h"
#include "string.h"
#include "asm/arch/config.h"
#include "asm/hal.h"
#include "mmap.h"


#ifdef CONFIG_DIRECT_ACCESS_KERNEL
  // Segments (Ring0 Privilege)
  #define USER_CODE_SEGMENT_SEL selCodeKernel
  #define USER_DATA_SEGMENT_SEL selDataKernel
#else
  // Segments (Ring3 Privilege)
  #define USER_CODE_SEGMENT_SEL selCodeUserTmp
  #define USER_DATA_SEGMENT_SEL selDataUserTmp
#endif


extern bool bPAEEnabled;


CTask             * pMainTask;
CThread           * pMainThread;
CThreadImpl       * pMainThreadImpl;
STaskStateSegment * pCurrentTSS;
#ifdef CONFIG_MMU
CAddressSpace FIXME;
#endif


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
task_init()
{
  pMainTask       = new CTask;
  pMainThread     = &pMainTask->thread();
  pMainThreadImpl = &pMainTask->thread().impl();

#ifdef CONFIG_MMU
  // Enable PAE (36 bit physical addressing)
  if(bPAEEnabled == true)
    setCR4(getCR4() | CR4_PAE);

  // Set CR3 (Page-Table Base Address)
  setCR3(pMainTask->aspace().cr3());

  // Enable paging
  setCR0(getCR0() | CR0_PG);
#endif

  // Set the current running tasks TSS
  setTR(pMainThreadImpl->selTSS_);

  // Add task to taskmanagers list
  pMainThread->state(TS_RUNNING);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#ifndef CONFIG_MMU
CThreadImpl::CThreadImpl()
 : pStack_(NULL)
#else
CThreadImpl::CThreadImpl(CAddressSpace * pASpace)
 : pASpace_(pASpace)
 , pStack_(NULL)
#endif
 , pSvcStack_(NULL)
{
  // Initialize TSS
  iTSSSize_ = sizeof(STaskStateSegment);
  pTSS_ = (STaskStateSegment *)new uint8_t[iTSSSize_];
  memset(pTSS_, 0, iTSSSize_);
  // IO permission bitmap
  pTSS_->io_map_addr = sizeof(STaskStateSegment);
  // Create descriptor for TSS
  selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)pTSS_, iTSSSize_);
}

// -----------------------------------------------------------------------------
CThreadImpl::~CThreadImpl()
{
  if(pStack_ != NULL)
    delete pStack_;
  if(pSvcStack_ != NULL)
    delete pSvcStack_;
  if(pTSS_ != NULL)
    delete pTSS_;
  // FIXME: Free TSS selector
}

// -----------------------------------------------------------------------------
void
CThreadImpl::init(void * entry, int argc, char * argv[])
{
  size_t stack(512);
  size_t svcstack(512);

  // Allocate stacks
  pStack_    = new uint32_t[stack];
  pSvcStack_ = new uint32_t[svcstack];

#ifndef CONFIG_DIRECT_ACCESS_KERNEL
  // SVC stack
  pTSS_->esp0 = (uint32_t)(pSvcStack_ + svcstack);
  pTSS_->ss0  = selDataKernel;
#endif
  // User stack
  ((uint32_t *)pStack_)[(stack >> 2) - 1] = (uint32_t)argv;
  ((uint32_t *)pStack_)[(stack >> 2) - 2] = (uint32_t)argc;
  ((uint32_t *)pStack_)[(stack >> 2) - 3] = 0; // Function call return address
  pTSS_->esp  = (uint32_t)pStack_ + stack - 12;
  pTSS_->es   = USER_DATA_SEGMENT_SEL;
  pTSS_->cs   = USER_CODE_SEGMENT_SEL;
  pTSS_->ss   = USER_DATA_SEGMENT_SEL;
  pTSS_->ds   = USER_DATA_SEGMENT_SEL;
  pTSS_->fs   = USER_DATA_SEGMENT_SEL;
  pTSS_->gs   = USER_DATA_SEGMENT_SEL;
  // Other
#ifdef CONFIG_MMU
  pTSS_->cr3  = pASpace_->cr3();
#endif
  pTSS_->eip  = (uint32_t)entry;
  pTSS_->eflags = I386_IOPL_VALUE(0) | I386_IE_FLAG | I386_ON_FLAGS;
}

// -----------------------------------------------------------------------------
void
CThreadImpl::runJump()
{
  if(cGDT.desc_[selTSS_ >> 3].access & (1<<1))
    panic("Can't jump to busy task!\n");

  pCurrentTSS = pTSS_;

  // Jump to task
  jumpSelector(selTSS_);
}

// -----------------------------------------------------------------------------
void
CThreadImpl::runCall()
{
  if(cGDT.desc_[selTSS_ >> 3].access & (1<<1))
    panic("Can't call busy task!\n");

  pCurrentTSS = pTSS_;

  // Jump to task
  callSelector(selTSS_);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#ifndef CONFIG_MMU
CV86Thread::CV86Thread()
 : pStack_(NULL)
#else
CV86Thread::CV86Thread(CAddressSpace * pASpace)
 : pASpace_(pASpace)
 , pStack_(NULL)
#endif
 , pSvcStack_(NULL)
{
  // Initialize TSS
  iTSSSize_ = sizeof(STaskStateSegment) + (64*1024/8);
  pTSS_ = (STaskStateSegment *)new uint8_t[iTSSSize_];
  memset(pTSS_, 0, iTSSSize_);
  // IO permission bitmap
  ((uint8_t *)pTSS_)[iTSSSize_ - 1] = 0xff;
  pTSS_->io_map_addr = sizeof(STaskStateSegment);
  // Create descriptor for TSS
  selTSS_ = cGDT.createSegment(dtTSS, 0, (uint32_t)pTSS_, iTSSSize_);
}

// -----------------------------------------------------------------------------
CV86Thread::~CV86Thread()
{
  if(pStack_ != NULL)
    physFreePage((uint64_t)pStack_);
  if(pSvcStack_ != NULL)
    delete pSvcStack_;
  if(pTSS_ != NULL)
    delete pTSS_;
  // FIXME: Free TSS selector
}

// -----------------------------------------------------------------------------
void
CV86Thread::init()
{
  //size_t stack(512);
  size_t svcstack(512);

  // Allocate stacks
  pStack_    = (uint32_t *)physAllocPageLow();
  pSvcStack_ = new uint32_t[svcstack];

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
#ifdef CONFIG_MMU
  pTSS_->cr3  = pASpace_->cr3();
#endif
//  pTSS_->eip  = ...;
  pTSS_->eflags = I386_VM_FLAG | I386_IOPL_VALUE(0) | I386_IE_FLAG | I386_ON_FLAGS;
}

// -----------------------------------------------------------------------------
void
CV86Thread::runJump()
{
  if(cGDT.desc_[selTSS_ >> 3].access & (1<<1))
    panic("Can't jump to busy task!\n");

  pCurrentTSS = pTSS_;

  // Jump to task
  jumpSelector(selTSS_);
}

// -----------------------------------------------------------------------------
void
CV86Thread::runCall()
{
  if(cGDT.desc_[selTSS_ >> 3].access & (1<<1))
    panic("Can't call busy task!\n");

  pCurrentTSS = pTSS_;

  // Jump to task
  callSelector(selTSS_);
}

// -----------------------------------------------------------------------------
void
CV86Thread::interrupt(uint8_t nr)
{
  uint16_t * base = 0;

  // Get interrupt vector
  pTSS_->cs     = base[(nr & 0xff) * 2 + 1];
  pTSS_->eip    = base[(nr & 0xff) * 2 + 0];
  // Reset eflags
  pTSS_->eflags = I386_VM_FLAG | I386_IOPL_VALUE(0) | I386_ON_FLAGS;
  // Reset busy flag
  cGDT.desc_[selTSS_ >> 3].access &= ~(1<<1);

  //printk("int 0x%x @ %x:%x = 0x%x\n", nr, pTSS_->cs, pTSS_->eip, from_v86_addr(pTSS_->cs, pTSS_->eip));

  runCall();
}
