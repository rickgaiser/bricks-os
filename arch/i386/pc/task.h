#ifndef PC_TASK_H
#define PC_TASK_H


#include "kernel/task.h"
#include "asm/irq.h"
#include "aspace.h"
#include "hal.h"


void init_task();


class CPCTask
 : public CTask
{
public:
  friend void init_task();

  CPCTask();
  virtual ~CPCTask();
  
  void init();

  // Entry Point
  void * entry(){return pEntry_;}
  void entry(void * ip);
  
  // Stack Pointer
  //void * stack(){return pStack_;}
  //void stack(void * sp);
  
  // Address Space
  CPCAddressSpace & aspace(){return cASpace_;}
  
  virtual void run();

private:
  bool bInitialized_;

  // Addess space
  CPCAddressSpace cASpace_;
  
  // Task state
  STaskStateSegment tss_;
  selector_t selTSS_;
  
  void * pEntry_;
  void * pStack_;
};


extern CPCTask taskMain;


#endif
