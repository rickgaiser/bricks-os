#ifndef PSP_TASK_H
#define PSP_TASK_H


#include "kernel/task.h"


class CPSPThread
 : public CThread
{
public:
  CPSPThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CPSPThread();
};


#endif
