#ifndef NGC_TASK_H
#define NGC_TASK_H


#include "kernel/task.h"


class CNGCThread
 : public CThread
{
public:
  CNGCThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CNGCThread();
};


#endif
