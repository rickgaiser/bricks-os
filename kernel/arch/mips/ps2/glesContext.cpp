#include "glesContext.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "gif.h"


DECLARE_GS_PACKET(dma_buf,1000);
extern uint16_t   gs_origin_x;
extern uint16_t   gs_origin_y;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPS2GLESContext::CPS2GLESContext()
 : CSoftGLESFloat()
{
  // Initialize the DMA buffer
  BEGIN_GS_PACKET(dma_buf);
  GIF_TAG_AD(dma_buf, 1, 0, 0, 0);
}

//-----------------------------------------------------------------------------
CPS2GLESContext::~CPS2GLESContext()
{
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glClear(GLbitfield mask)
{
  if(mask & GL_COLOR_BUFFER_BIT)
  {
    uint8_t r = 128;//clClear.r * 255;
    uint8_t g = 128;//clClear.r * 255;
    uint8_t b = 128;//clClear.r * 255;

    GIF_DATA_AD(dma_buf, prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
    GIF_DATA_AD(dma_buf, rgbaq, GS_RGBAQ(r, g, b, 0x80, 0));
    GIF_DATA_AD(dma_buf, xyz2, GS_XYZ2((gs_origin_x+0)<<4, (gs_origin_x+0)<<4, 0));
    GIF_DATA_AD(dma_buf, xyz2, GS_XYZ2((gs_origin_x+viewportWidth+1)<<4, (gs_origin_x+viewportHeight+1)<<4, 0));
  }
/*
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
  }
*/
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glFlush(void)
{
  // Wait vsync
  REG_GS_CSR = REG_GS_CSR & 8;
  while(!(REG_GS_CSR & 8));

  // Kick off the DMA transfer to the GS
  SEND_GS_PACKET(dma_buf);
  //while(true);

  // Initialize the DMA buffer
  BEGIN_GS_PACKET(dma_buf);
  GIF_TAG_AD(dma_buf, 1, 0, 0, 0);
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::rasterPoly(SPolygonF & poly)
{
  GIF_DATA_AD(dma_buf, prim, GS_PRIM(PRIM_TRI, SHADE_GOURAUD, TEXTURES_OFF, FOG_OFF, ALPHABLEND_OFF, ALIASING_ON, 0, 0, 0));
  for(int i(0); i < 3; i++)
  {
    GIF_DATA_AD(dma_buf, rgbaq, GS_RGBAQ((uint8_t)(poly.v[i]->c2.r*255), (uint8_t)(poly.v[i]->c2.g*255), (uint8_t)(poly.v[i]->c2.b*255), 100, 0));
    GIF_DATA_AD(dma_buf, xyz2,  GS_XYZ2((gs_origin_x+poly.v[i]->sx)<<4, (gs_origin_y+poly.v[i]->sy)<<4, (uint32_t)(poly.v[i]->v2[2] * 0xFFFF)));
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IGLESContext *
getGLESContext()
{
  return new CPS2GLESContext;
}
