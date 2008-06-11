#ifndef PC_TASK_H
#define PC_TASK_H


#include "asm/arch/config.h"
#include "asm/irq.h"
#include "asm/hal.h"
#include "asm/aspace.h"
#include "kernel/task.h"


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

public:
  // Task state
  STaskStateSegment * pTSS_;
  uint32_t iTSSSize_;
  selector_t selTSS_;

  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};

// -----------------------------------------------------------------------------
class CV86Thread
 : public CThread
{
public:
  CV86Thread(CTask * task);
  virtual ~CV86Thread();

  // Task switch #1: Jump to task immediately.
  //  - Used from caller context
  void runJump();
  void runCall();

  // Setup for v86 interrupt and runJump to the v86 task
  void interrupt(uint8_t nr);

public:
#ifdef CONFIG_MMU
  // Addess space
  CAddressSpace FIXME;
#endif

  // Task state
  STaskStateSegment * pTSS_;
  uint32_t iTSSSize_;
  selector_t selTSS_;

  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};


extern CTask     * pMainTask;
extern CPCThread * pMainThread;


#endif
