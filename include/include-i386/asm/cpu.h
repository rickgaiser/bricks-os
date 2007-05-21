#ifndef CPU_H
#define CPU_H


#include "../../../arch/i386/pc/hal.h"


class CCPU
{
public:
  static inline void cli(){::cli();}
  static inline void sti(){::sti();}
  static inline void halt(){::halt();}

private:
  CCPU(){}
};


#endif
