#ifndef CPU_H
#define CPU_H


// -----------------------------------------------------------------------------
static inline unsigned long
local_save_flags(void)
{
  return 0;
}

// -----------------------------------------------------------------------------
static inline void
local_irq_restore(unsigned long flags)
{
}

// -----------------------------------------------------------------------------
static inline void
local_irq_disable(void)
{
}

// -----------------------------------------------------------------------------
static inline void
local_irq_enable(void)
{
}

// -----------------------------------------------------------------------------
static inline void halt(){while(true){}}


#endif
