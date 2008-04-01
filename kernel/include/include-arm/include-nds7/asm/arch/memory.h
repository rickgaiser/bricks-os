#ifndef ASM_ARCH_MEMORY_H
#define ASM_ARCH_MEMORY_H


// IWRAM: 64KiB (default)
#define IWRAM_CODE __attribute__((section(".iwram"), long_call))
#define IWRAM_DATA __attribute__((section(".iwram")))
#define INTERRUPT_CODE IWRAM_CODE
#define FAST_CODE      INTERRUPT_CODE
#define FAST_DATA      IWRAM_DATA


#endif
