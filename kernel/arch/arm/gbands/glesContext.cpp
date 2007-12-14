#include "glesContext.h"
#include "asm/arch/macros.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGBAGLESContext::CGBAGLESContext()
 : CSoftGLESFixed()
{
}

//-----------------------------------------------------------------------------
CGBAGLESContext::~CGBAGLESContext()
{
}

//-----------------------------------------------------------------------------
void
CGBAGLESContext::glClear(GLbitfield mask)
{
  if(mask & GL_COLOR_BUFFER_BIT)
  {
    switch(renderSurface->mode.bpp)
    {
      case 8:
      {
        // FIXME!
        uint32_t color = 0x00000000;
        if(renderSurface->mode.xpitch == renderSurface->mode.width)
        {
          dmaFill32(color, renderSurface->p, viewportPixelCount>>2);
        }
        else
        {
          unsigned int iBase(0);
          for(unsigned int iY(0); iY < renderSurface->mode.height; iY++)
          {
            dmaFill16(color, &((uint8_t *)renderSurface->p)[iBase], renderSurface->mode.width);
            iBase += renderSurface->mode.xpitch;
          }
        }
      }
      case 16:
      {
        uint32_t color = (fpRGB(clClear.r, clClear.g, clClear.b) << 16) | fpRGB(clClear.r, clClear.g, clClear.b);
        if(renderSurface->mode.xpitch == renderSurface->mode.width)
        {
          dmaFill32(color, renderSurface->p, viewportPixelCount>>1);
        }
        else
        {
          unsigned int iBase(0);
          for(unsigned int iY(0); iY < renderSurface->mode.height; iY++)
          {
            dmaFill16(color, &((uint16_t *)renderSurface->p)[iBase], renderSurface->mode.width);
            iBase += renderSurface->mode.xpitch;
          }
        }
      }
    };
  }
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    uint32_t z = (zClearValue_ << 16) | zClearValue_;
    dmaFill32(z, zbuffer, viewportPixelCount>>1);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IGLESRenderer *
getGLESContext()
{
  return new CGBAGLESContext;
}
