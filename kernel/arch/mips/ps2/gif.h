//---------------------------------------------------------------------------
// File:    gif.h
// Author:  Tony Saveski, t_saveski@yahoo.com
// Notes:   Macros to create and populate DMA buffers to send to the PS2 GIF
//---------------------------------------------------------------------------
#ifndef GIF_H
#define GIF_H


#include "inttypes.h"
#include "asm/arch/registers.h"
#include "bios.h"


#define GIF_AD   0x0e
#define GIF_NOP  0x0f


//---------------------------------------------------------------------------
// GS_PACKET macros
//---------------------------------------------------------------------------
#define DECLARE_GS_PACKET(NAME,ITEMS) \
  uint64_t __attribute__((aligned(64))) NAME[ITEMS*2+2]; \
  uint64_t NAME##_nloop; \
  uint64_t NAME##_cur; \
  uint64_t NAME##_dma_size; \
  uint64_t NAME##_max_items

#define INIT_GS_PACKET(NAME,ITEMS) \
  NAME##_max_items=ITEMS

#define DECLARE_EXTERN_GS_PACKET(NAME) \
  extern uint64_t __attribute__((aligned(64))) NAME[]; \
  extern uint64_t NAME##_nloop; \
  extern uint64_t NAME##_cur; \
  extern uint64_t NAME##_dma_size; \
  extern uint64_t NAME##_max_items

#define BEGIN_GS_PACKET(NAME) \
  NAME##_nloop = 0; \
  NAME##_dma_size = 0; \
  NAME##_cur = 0

#define GIF_TAG(NAME,EOP,PRE,PRIM,FLG,NREG,REGS) \
  NAME[NAME##_cur++] = \
      ((uint64_t)(EOP)  << 15) | \
      ((uint64_t)(PRE)  << 46) | \
      ((uint64_t)(PRIM) << 47) | \
      ((uint64_t)(FLG)  << 58) | \
      ((uint64_t)(NREG) << 60); \
  NAME[NAME##_cur++] = (uint64_t)REGS; \
  NAME##_dma_size++

#define GIF_TAG_AD(NAME,EOP,PRE,PRIM,FLG) \
  GIF_TAG(NAME,EOP,PRE,PRIM,FLG,1,GIF_AD)

#define GIF_TAG_IMG(NAME,QSIZE) \
  GIF_TAG(NAME,1,0,0,2,0,0); \
  NAME##_nloop = QSIZE; \
  NAME##_dma_size = 1

#define GIF_DATA_AD(NAME,REG,DAT) \
  NAME[NAME##_cur++] = (uint64_t)DAT; \
  NAME[NAME##_cur++] = (uint64_t)REG; \
  NAME##_dma_size++; \
  NAME##_nloop++; \
  if(NAME##_nloop == NAME##_max_items) \
  { \
      SEND_GS_PACKET(NAME); \
      BEGIN_GS_PACKET(NAME); \
      GIF_TAG_AD(NAME, 1, 0, 0, 0); \
  }

#define SEND_GS_PACKET(NAME) \
  NAME[0] |= NAME##_nloop; \
  flushCache(0); \
  SET_QWC(&REG_GIF_QWC, NAME##_dma_size);  \
  SET_MADR(&REG_GIF_MADR, NAME, 0); \
  SET_CHCR(&REG_GIF_CHCR, 1, 0, 0, 0, 0, 1, 0);  \
  DMA_WAIT(&REG_GIF_CHCR)


#endif // GIF_H
