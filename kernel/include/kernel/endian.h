#ifndef ENDIAN_H
#define ENDIAN_H


#include "asm/arch/config.h"


#define ENDIAN_SWAP_16(v) \
  ((((v) >>  8) & 0x00ff) | \
   (((v) <<  8) & 0xff00))
#define ENDIAN_SWAP_32(v) \
  ((((v) >> 24) & 0x000000ff) | \
   (((v) >>  8) & 0x0000ff00) | \
   (((v) <<  8) & 0x00ff0000) | \
   (((v) << 24) & 0xff000000))
#define ENDIAN_SWAP_64(v) \
  ((((v) >> 56) & 0x00000000000000ff) | \
   (((v) >> 40) & 0x000000000000ff00) | \
   (((v) >> 24) & 0x0000000000ff0000) | \
   (((v) >>  8) & 0x00000000ff000000) | \
   (((v) <<  8) & 0x000000ff00000000) | \
   (((v) << 24) & 0x0000ff0000000000) | \
   (((v) << 40) & 0x00ff000000000000) | \
   (((v) << 56) & 0xff00000000000000))

#if defined(__LITTLE_ENDIAN__)
  #define ENDIAN_LE_16(v) (v)
  #define ENDIAN_LE_32(v) (v)
  #define ENDIAN_LE_64(v) (v)
  #define ENDIAN_BE_16(v) ENDIAN_SWAP_16(v)
  #define ENDIAN_BE_32(v) ENDIAN_SWAP_32(v)
  #define ENDIAN_BE_64(v) ENDIAN_SWAP_64(v)
#elif defined(__BIG_ENDIAN__)
  #define ENDIAN_LE_16(v) ENDIAN_SWAP_16(v)
  #define ENDIAN_LE_32(v) ENDIAN_SWAP_32(v)
  #define ENDIAN_LE_64(v) ENDIAN_SWAP_64(v)
  #define ENDIAN_BE_16(v) (v)
  #define ENDIAN_BE_32(v) (v)
  #define ENDIAN_BE_64(v) (v)
#else
  #error BYTE ORDER NOT DEFINED!
#endif


#endif
