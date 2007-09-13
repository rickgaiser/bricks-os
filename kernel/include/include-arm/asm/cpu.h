#ifndef CPU_H
#define CPU_H


#include "asm/arch/registers.h"


// -----------------------------------------------------------------------------
static inline unsigned long
local_save_flags(void)
{
  return REG_IME;
}

// -----------------------------------------------------------------------------
static inline void
local_irq_restore(unsigned long flags)
{
  REG_IME = flags;
}

// -----------------------------------------------------------------------------
static inline void
local_irq_disable(void)
{
  REG_IME = 0;
}

// -----------------------------------------------------------------------------
static inline void
local_irq_enable(void)
{
  REG_IME = 1;
}

// -----------------------------------------------------------------------------
static inline void halt(){while(true){}}


#endif
