#ifndef GBANDS_TASK_H
#define GBANDS_TASK_H


#include "kernel/task.h"
#include "asm/irq.h"


class CGBANDSTask
 : public CTask
{
public:
  CGBANDSTask(void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CGBANDSTask();

private:
  static void kill();  // Return function, kills the current task

//private:
public:
  pt_regs * pTaskState_;
};


#endif
