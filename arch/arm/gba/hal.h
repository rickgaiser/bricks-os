#ifndef HAL_H
#define HAL_H


#include "gba.h"


namespace HAL
{
  inline void cli(){REG_IME = 0;}
  inline void sti(){REG_IME = 1;}
  inline void halt(){while(true){}}
};


#endif
