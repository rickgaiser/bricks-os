#ifndef PC_TASK_H
#define PC_TASK_H


#include "kernel/task.h"
#include "asm/irq.h"
#include "aspace.h"
#include "hal.h"


//#define PAGING_ENABLED


// -----------------------------------------------------------------------------
class CPCThread
 : public CThread
{
public:
  CPCThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CPCThread();

  static void init();

  // Task switch #1: Jump to task immediately.
  //  - Used from caller context
  virtual void runJump();
  void runCall();
  // Task switch #2: Setup stack so interrupt return will couse this task to run.
  //  - Used from interrupt context
  //virtual void runReturn();

#ifdef PAGING_ENABLED
  // Address Space
  CPCAddressSpace & aspace(){return cASpace_;}
#endif

public:
#ifdef PAGING_ENABLED
  // Addess space
  CPCAddressSpace cASpace_;
#endif

  // Task state
  STaskStateSegment * pTSS_;
  uint32_t iTSSSize_;
  selector_t selTSS_;

  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};

// -----------------------------------------------------------------------------
class CV86Thread
{
public:
  CV86Thread();
  ~CV86Thread();

  // Task switch #1: Jump to task immediately.
  //  - Used from caller context
  void runJump();
  void runCall();

  // Setup for v86 interrupt and runJump to the v86 task
  void interrupt(uint8_t nr);

public:
#ifdef PAGING_ENABLED
  // Addess space
  CPCAddressSpace cASpace_;
#endif

  // Task state
  STaskStateSegment * pTSS_;
  uint32_t iTSSSize_;
  selector_t selTSS_;

  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};


#endif
