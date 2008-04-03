//---------------------------------------------------------------------------
// File:    gs.c
// Author:  Tony Saveski, t_saveski@yahoo.com
//---------------------------------------------------------------------------
#include "bios.h"
#include "gs.h"
#include "gif.h"
#include "dma.h"


#define MAX_TRANSFER  16384


uint64_t gifData_Data[50 * 2 + 2] __attribute__((aligned(64)));
CGIFPacket gifData(50, gifData_Data);


//---------------------------------------------------------------------------
void
gs_load_texture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t data_adr, uint32_t dest_adr, uint16_t dest_w)
{
  uint32_t i;                   // DMA buffer loop counter
  uint32_t frac;                // flag for whether to run a fractional buffer or not
  uint32_t current;             // number of pixels to transfer in current DMA
  uint32_t qtotal;              // total number of qwords of data to transfer

  gifData.reset();
  gifData.tag(1, 0, 0, 0);
  gifData.data(bitbltbuf, GS_BITBLTBUF(0, 0, 0, dest_adr >> 8, dest_w >> 6, 0));
  gifData.data(trxpos,    GS_TRXPOS(0, 0, x, y, 0));
  gifData.data(trxreg, GS_TRXREG(w, h));
  gifData.data(trxdir, GS_TRXDIR(XDIR_EE_GS));
  gifData.send();

  qtotal = w*h/4;               // total number of quadwords to transfer.
  current = qtotal % MAX_TRANSFER;// work out if a partial buffer transfer is needed.
  frac=1;                       // assume yes.
  if(!current)                  // if there is no need for partial buffer
  {
    current = MAX_TRANSFER;     // start with a full buffer
    frac=0;                     // and don't do extra partial buffer first
  }

  for(i=0; i < (qtotal / MAX_TRANSFER) + frac; i++)
  {
    DECLARE_GS_PACKET(GsCmdBuffer,50);
    INIT_GS_PACKET(GsCmdBuffer,50);

    BEGIN_GS_PACKET(GsCmdBuffer);
    GIF_TAG_IMG(GsCmdBuffer, current);
    SEND_GS_PACKET(GsCmdBuffer);

    SET_QWC(&REG_GIF_QWC, current);
    SET_MADR(&REG_GIF_MADR, data_adr, 0);
    SET_CHCR(&REG_GIF_CHCR, 1, 0, 0, 0, 0, 1, 0);
    DMA_WAIT(&REG_GIF_CHCR);

    data_adr += current*16;
    current = MAX_TRANSFER;     // after the first one, all are full buffers
  }

  // Access the TEXFLUSH register with anything to flush the texture
  gifData.reset();
  gifData.tag(1, 0, 0, 0);
  gifData.data(texflush, 0x42);
  gifData.send();
}

//---------------------------------------------------------------------------
uint16_t
gs_texture_wh(uint16_t n)
{
  uint16_t l(0);

  n--;
  while(n > 0)
  {
    n >>= 1;
    l++;
  }

  return l;
}
