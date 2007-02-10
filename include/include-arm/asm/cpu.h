#ifndef CPU_H
#define CPU_H


#include "asm/arch/registers.h"


class CCPU
{
public:
  static inline void cli(){REG_IME = 0;}
  static inline void sti(){REG_IME = 1;}
  static inline void halt(){while(true){}}

private:
  CCPU(){}
};


#endif
