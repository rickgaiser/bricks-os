#include "bios.h"
#include "gs.h"
#include "gif.h"
#include "dma.h"


uint64_t gifData_Data[50 * 2 + 2] __attribute__((aligned(64)));
CGIFPacket gifData(50, gifData_Data);


//---------------------------------------------------------------------------
void
ee_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source)
{
  // Initialize GIF packet
  gifData.reset();

  // BitBlt from EE to GS
  gifData.tagAd(1, 0, 0, 0);
  gifData.data(bitbltbuf, GS_BITBLTBUF(0, 0, 0, daddr >> 8, dw >> 6, dpsm));
  gifData.data(trxpos,    GS_TRXPOS(0, 0, dx, dy, 0));
  gifData.data(trxreg,    GS_TRXREG(w, h));
  gifData.data(trxdir,    GS_TRXDIR(XDIR_EE_GS));
  gifData.send();

  // Send image data
  switch(dpsm)
  {
    case GRAPH_PSM_32:  // Framebuffer, Z-Buffer & Textures
      gifData.sendImage(source, w * h * 4);
      break;
    case GRAPH_PSM_24:  // Framebuffer, Z-Buffer & Textures
      gifData.sendImage(source, w * h * 3);
      break;
    case GRAPH_PSM_16:  // Framebuffer, Z-Buffer & Textures
    case GRAPH_PSM_16S: // Framebuffer, Z-Buffer & Textures
      gifData.sendImage(source, w * h * 2);
      break;
    case GRAPH_PSM_8:   // Textures only (palettized)
    case GRAPH_PSM_8H:  // Textures only (palettized)
      gifData.sendImage(source, w * h);
      break;
    case GRAPH_PSM_4:   // Textures only (palettized)
    case GRAPH_PSM_4HH: // Textures only (palettized)
    case GRAPH_PSM_4HL: // Textures only (palettized)
      gifData.sendImage(source, (w * h) >> 1);
      break;
  };

  // Access the TEXFLUSH register with anything to flush the texture
  gifData.tagAd(1, 0, 0, 0);
  gifData.data(texflush, 0x42);
  gifData.send();
}

//---------------------------------------------------------------------------
void
gs_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source, int sw, int spsm, int sx, int sy)
{
  gifData.reset();
  gifData.tagAd(1, 0, 0, 0);
  gifData.data(bitbltbuf, GS_BITBLTBUF(source>>8, sw>>6, spsm, daddr>>8, dw>>6, dpsm));
  gifData.data(trxpos,    GS_TRXPOS(sx, sy, dx, dy, 0));
  gifData.data(trxreg,    GS_TRXREG(w, h));
  gifData.data(trxdir,    GS_TRXDIR(XDIR_GS_GS));
  gifData.send();
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