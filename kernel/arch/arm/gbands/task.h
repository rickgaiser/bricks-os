#ifndef GBANDS_TASK_H
#define GBANDS_TASK_H


#include "kernel/task.h"


struct pt_regs;


class CGBANDSThread
 : public CThread
{
public:
  CGBANDSThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CGBANDSThread();

  // Task switch #1: Jump to task immediately.
  //  - Used from caller context
  //virtual void runJump();
  // Task switch #2: Setup stack so interrupt return will couse this task to run.
  //  - Used from interrupt context
  virtual void runReturn();

private:
  pt_regs  * pThreadState_;
  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};


#endif
