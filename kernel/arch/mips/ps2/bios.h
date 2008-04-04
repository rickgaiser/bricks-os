#ifndef PS2_BIOS_H
#define PS2_BIOS_H


#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

void      resetEE(uint32_t init_bitfield);
void      setGsCrt(int16_t interlace, int16_t pal_ntsc, int16_t field);
void      flushCache(int32_t operation);
uint64_t  getGsIMR();
uint64_t  setGsIMR(uint64_t imr);

#ifdef __cplusplus
}
#endif


#endif
