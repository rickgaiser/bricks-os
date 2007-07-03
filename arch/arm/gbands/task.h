#ifndef GBANDS_TASK_H
#define GBANDS_TASK_H


#include "kernel/task.h"
#include "asm/irq.h"


extern "C" void isr(pt_regs * regs);


class CGBANDSTask
 : public CTask
{
public:
  friend void isr(pt_regs * regs);

  CGBANDSTask(void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CGBANDSTask();

private:
  static void kill();  // Return function, kills the current task

  pt_regs  * pTaskState_;
  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};


#endif
