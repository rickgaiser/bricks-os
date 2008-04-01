#ifndef ASM_ARCH_MEMORY_H
#define ASM_ARCH_MEMORY_H


// EWRAM:  4MiB (default)
// ITCM:  32KiB (instruction cache)
// DTCM:  16KiB (data cache)
#define EWRAM_CODE __attribute__((section(".ewram"), long_call))
#define EWRAM_DATA __attribute__((section(".ewram")))
#define ITCM_CODE  __attribute__((section(".itcm"), long_call))
#define DTCM_DATA  __attribute__((section(".dtcm")))
#define INTERRUPT_CODE ITCM_CODE
#define FAST_CODE      INTERRUPT_CODE
#define FAST_DATA      DTCM_DATA


#endif
