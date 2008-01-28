#ifndef PC_TASK_H
#define PC_TASK_H


#include "kernel/task.h"
#include "asm/irq.h"
#include "aspace.h"
#include "hal.h"


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
  // Task switch #2: Setup stack so interrupt return will couse this task to run.
  //  - Used from interrupt context
  //virtual void runReturn();

  // Address Space
  CPCAddressSpace & aspace(){return cASpace_;}

private:
  // Addess space
  CPCAddressSpace cASpace_;

  // Task state
  STaskStateSegment tss_;
  selector_t selTSS_;

  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};


#endif
