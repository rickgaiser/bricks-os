#include "bios.h"
#include "gs.h"
#include "gif.h"
#include "dma.h"


uint64_t gifData_Data[50 * 2] __attribute__((aligned(64)));
CGIFPacket gifData(gifData_Data, 50, DMAC::Channel::GIF);


//---------------------------------------------------------------------------
void
ee_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source)
{
  // BitBlt from EE to GS
  gifData.scTagOpenEnd();
  {
    gifData.gifTagOpenPacked();
    {
      gifData.gifAddPackedAD(GIF::REG::bitbltbuf, GS_BITBLTBUF(0, 0, 0, daddr >> 8, dw >> 6, dpsm));
      gifData.gifAddPackedAD(GIF::REG::trxpos,    GS_TRXPOS(0, 0, dx, dy, 0));
      gifData.gifAddPackedAD(GIF::REG::trxreg,    GS_TRXREG(w, h));
      gifData.gifAddPackedAD(GIF::REG::trxdir,    GS_TRXDIR(XDIR_EE_GS));
    }
    gifData.gifTagClose();
  }
  gifData.scTagClose();

  // Send image data
  switch(dpsm)
  {
    case GRAPH_PSM_32:  // Framebuffer, Z-Buffer & Textures
      gifData.gifAddSendImage((void *)source, w * h * 4);
      break;
    case GRAPH_PSM_24:  // Framebuffer, Z-Buffer & Textures
      gifData.gifAddSendImage((void *)source, w * h * 3);
      break;
    case GRAPH_PSM_16:  // Framebuffer, Z-Buffer & Textures
    case GRAPH_PSM_16S: // Framebuffer, Z-Buffer & Textures
      gifData.gifAddSendImage((void *)source, w * h * 2);
      break;
    case GRAPH_PSM_8:   // Textures only (palettized)
    case GRAPH_PSM_8H:  // Textures only (palettized)
      gifData.gifAddSendImage((void *)source, w * h);
      break;
    case GRAPH_PSM_4:   // Textures only (palettized)
    case GRAPH_PSM_4HH: // Textures only (palettized)
    case GRAPH_PSM_4HL: // Textures only (palettized)
      gifData.gifAddSendImage((void *)source, (w * h) >> 1);
      break;
  };

  // Access the TEXFLUSH register with anything to flush the texture
  gifData.scTagOpenEnd();
  {
    gifData.gifTagOpenPacked();
    {
      gifData.gifAddPackedAD(GIF::REG::texflush, 0x42);
    }
    gifData.gifTagClose();
  }
  gifData.scTagClose();

  // Start data transfer and reset packet
  gifData.send();
  gifData.reset();
}

//---------------------------------------------------------------------------
void
gs_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source, int sw, int spsm, int sx, int sy)
{
  gifData.scTagOpenEnd();
    gifData.gifTagOpenPacked();
      gifData.gifAddPackedAD(GIF::REG::bitbltbuf, GS_BITBLTBUF(source>>8, sw>>6, spsm, daddr>>8, dw>>6, dpsm));
      gifData.gifAddPackedAD(GIF::REG::trxpos,    GS_TRXPOS(sx, sy, dx, dy, 0));
      gifData.gifAddPackedAD(GIF::REG::trxreg,    GS_TRXREG(w, h));
      gifData.gifAddPackedAD(GIF::REG::trxdir,    GS_TRXDIR(XDIR_GS_GS));
    gifData.gifTagClose();
  gifData.scTagClose();
  gifData.send();
  gifData.reset();
}

//---------------------------------------------------------------------------
// 0x0001 ->  0
// 0x0002 ->  1
// 0x0004 ->  3
// 0x0008 ->  4
// ...... -> ..
// 0x8000 -> 15
uint8_t
getBitNr(uint32_t value)
{
  uint8_t iBitNr;

  value--;
  for(iBitNr = 0; value > 0; iBitNr++, value >>= 1)
    ;

  return iBitNr;
}
