#ifndef ASM_ARCH_MEMORY_H
#define ASM_ARCH_MEMORY_H


#define IWRAM_CODE __attribute__((section(".iwram"), long_call))
#define EWRAM_CODE __attribute__((section(".ewram"), long_call))

#define IWRAM_DATA __attribute__((section(".iwram")))
#define EWRAM_DATA __attribute__((section(".ewram")))

#define EWRAM_BSS  __attribute__((section(".sbss")))


#endif
