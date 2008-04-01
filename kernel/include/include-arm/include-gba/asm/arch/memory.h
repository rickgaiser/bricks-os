#ifndef ASM_ARCH_MEMORY_H
#define ASM_ARCH_MEMORY_H


// EWRAM: 256KiB (default)
// IWRAM:  32KiB
#define IWRAM_CODE __attribute__((section(".iwram"), long_call))
#define EWRAM_CODE __attribute__((section(".ewram"), long_call))
#define IWRAM_DATA __attribute__((section(".iwram")))
#define EWRAM_DATA __attribute__((section(".ewram")))
#define INTERRUPT_CODE IWRAM_CODE
#define FAST_CODE      INTERRUPT_CODE
#define FAST_DATA      IWRAM_DATA


#endif
