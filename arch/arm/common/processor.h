#ifndef PROCESSOR_H
#define PROCESSOR_H


#include "kernel/arch.h"


class CProcessor
 : public arch::IProcessor
{
public:
  void cli();
  void sti();
  void halt();
};


#endif
