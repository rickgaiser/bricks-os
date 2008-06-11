#ifndef CPU_H
#define CPU_H


#include "hal.h"


// -----------------------------------------------------------------------------
static inline unsigned long
local_save_flags(void)
{
  unsigned long flags;
  __asm__ __volatile__("pushfl ; popl %0" : "=g" (flags) :);
  return flags;
}

// -----------------------------------------------------------------------------
static inline void
local_irq_restore(unsigned long flags)
{
  __asm__ __volatile__("pushl %0 ; popfl" :: "g" (flags) :"memory", "cc");
}

// -----------------------------------------------------------------------------
static inline void
local_irq_disable(void)
{
  __asm__ __volatile__("cli" ::: "memory");
}

// -----------------------------------------------------------------------------
static inline void
local_irq_enable(void)
{
  __asm__ __volatile__("sti" ::: "memory");
}

// -----------------------------------------------------------------------------
static inline void halt(){__asm__ __volatile__("hlt"::: "memory");}


#endif
