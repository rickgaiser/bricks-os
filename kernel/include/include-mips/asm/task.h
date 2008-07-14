#ifndef PS2_TASK_H
#define PS2_TASK_H


// -----------------------------------------------------------------------------
void task_init();

// -----------------------------------------------------------------------------
class CThreadImpl
{
public:
  CThreadImpl();
  ~CThreadImpl();

  void init(void * entry, int argc = 0, char * argv[] = 0);

  // Task switch #1: Jump to task immediately.
  //  - Used from caller context
  void runJump();
  // Task switch #2: Setup stack so interrupt return will couse this task to run.
  //  - Used from interrupt context
  //virtual void runReturn();
};


#endif
