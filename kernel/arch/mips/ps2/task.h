#ifndef PS2_TASK_H
#define PS2_TASK_H


#include "kernel/task.h"


class CPS2Thread
 : public CThread
{
public:
  CPS2Thread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CPS2Thread();

  // Task switch #1: Jump to task immediately.
  //  - Used from caller context
  //virtual void runJump();
  // Task switch #2: Setup stack so interrupt return will couse this task to run.
  //  - Used from interrupt context
  //virtual void runReturn();
};


#endif
