#ifndef ASM_ARCH_MACROS_H
#define ASM_ARCH_MACROS_H


#include "inttypes.h"
#include "string.h"


inline void
dmaCopy(const void * source, void * dest, uint32_t size)
{
  memcpy(dest, source, size);
}

inline void
dmaFill16(uint16_t data, void * dest, uint32_t count)
{
  for(uint32_t i(0); i < count; i++)
    ((uint16_t *)dest)[i] = data;
}

inline void
dmaFill32(uint32_t data, void * dest, uint32_t count)
{
  for(uint32_t i(0); i < count; i++)
    ((uint32_t *)dest)[i] = data;
}


#endif
