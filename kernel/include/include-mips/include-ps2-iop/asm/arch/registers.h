#ifndef ASM_ARCH_REGISTERS_H
#define ASM_ARCH_REGISTERS_H


#include "inttypes.h"


//
#define REG_INT_STAT          (*(vuint64_t *)(0x1000f000)) // Interrupt status
#define REG_INT_MASK          (*(vuint64_t *)(0x1000f010)) // Interrupt mask
#define REG_SBUS_SMFLG        (*(vuint64_t *)(0x1000f230))


#endif
