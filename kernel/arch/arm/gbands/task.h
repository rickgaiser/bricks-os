#ifndef GBANDS_TASK_H
#define GBANDS_TASK_H


#include "kernel/task.h"


struct pt_regs;


class CGBANDSTask
 : public CTask
{
public:
  CGBANDSTask(void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CGBANDSTask();

  virtual void run();

private:
  pt_regs  * pTaskState_;
  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};


#endif
