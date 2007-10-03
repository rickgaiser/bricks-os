//---------------------------------------------------------------------------
// File:    dma.h
// Author:  Tony Saveski, t_saveski@yahoo.com
// Notes:   Playstation2 DMA Convenience Routines and Macros
//---------------------------------------------------------------------------
// * Data is transfered in qword (128bit, 16bytes) units
// * Data should be aligned on qword boundary (.align 7, aligned(16))
//---------------------------------------------------------------------------
#ifndef DMA_H
#define DMA_H


#include "inttypes.h"
#include "asm/arch/registers.h"


//---------------------------------------------------------------------------
// CHCR Register - Channel Control Register
//---------------------------------------------------------------------------
#define SET_CHCR(WHICH,DIR,MOD,ASP,TTE,TIE,STR,TAG) \
  *WHICH = \
  ((uint32_t)(DIR) << 0) | \
  ((uint32_t)(MOD) << 2) | \
  ((uint32_t)(ASP) << 4) | \
  ((uint32_t)(TTE) << 6) | \
  ((uint32_t)(TIE) << 7) | \
  ((uint32_t)(STR) << 8) | \
  ((uint32_t)(TAG) << 16)

#define DMA_WAIT(WHICH) \
  while((*WHICH) & (1<<8))

//---------------------------------------------------------------------------
// MADR Register - Transfer Address Register
//---------------------------------------------------------------------------
#define SET_MADR(WHICH,ADDR,SPR) \
  *WHICH = \
  ((uint32_t)(ADDR) << 0) | \
  ((uint32_t)(SPR)  << 31)

//---------------------------------------------------------------------------
// TADR Register - Tag Address Register
//---------------------------------------------------------------------------
#define SET_TADR(WHICH,ADDR,SPR) \
  *WHICH = \
  ((uint32_t)(ADDR) << 0) | \
  ((uint32_t)(SPR)  << 31)

//---------------------------------------------------------------------------
// QWC Register - Transfer Data Size Register
//---------------------------------------------------------------------------
#define SET_QWC(WHICH,SIZE) \
  *WHICH = (uint32_t)(SIZE)


#ifdef __cplusplus
extern "C" {
#endif

extern void dma_reset(void);

#ifdef __cplusplus
}
#endif


#endif // DMA_H
