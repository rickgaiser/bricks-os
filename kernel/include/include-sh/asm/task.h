#ifndef ARCH_TASK_H
#define ARCH_TASK_H


#include "kernel/task.h"


class CDCThread
 : public CThread
{
public:
  CDCThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CDCThread();
};


#endif
