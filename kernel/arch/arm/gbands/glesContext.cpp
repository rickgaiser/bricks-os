#include "glesContext.h"
#include "asm/arch/macros.h"


#define BxColorFormat_FromFxRGB(format,r,g,b) \
  BxColorFormat_FromRGB(format, \
                        (r.value * 255) >> 16, \
                        (g.value * 255) >> 16, \
                        (b.value * 255) >> 16);


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
    unsigned int iBase(viewportYOffset * renderSurface->mode.xpitch + viewportXOffset);

    switch(renderSurface->mode.bpp)
    {
      case 8:
      {
        // FIXME!
        uint32_t color = BxColorFormat_FromFxRGB(renderSurface->mode.format, clClear.r, clClear.g, clClear.b);
        color = (color << 24) | (color << 16) | (color << 8) | color;

        if((int)renderSurface->mode.xpitch == viewportWidth)
        {
          dmaFill32(color, &((uint8_t *)renderSurface->p)[iBase], viewportPixelCount>>2);
        }
        else
        {
          for(int iY(0); iY < viewportHeight; iY++)
          {
            dmaFill16(color, &((uint8_t *)renderSurface->p)[iBase], viewportWidth);
            iBase += renderSurface->mode.xpitch;
          }
        }
        break;
      }
      case 16:
      {
        uint32_t color = BxColorFormat_FromFxRGB(renderSurface->mode.format, clClear.r, clClear.g, clClear.b);
        color = (color << 16) | color;
        if((int)renderSurface->mode.xpitch == viewportWidth)
        {
          dmaFill32(color, &((uint8_t *)renderSurface->p)[iBase], viewportPixelCount>>1);
        }
        else
        {
          for(int iY(0); iY < viewportHeight; iY++)
          {
            dmaFill16(color, &((uint16_t *)renderSurface->p)[iBase], viewportWidth);
            iBase += renderSurface->mode.xpitch;
          }
        }
        break;
      }
    };
  }
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    uint32_t z = (zClearValue_ << 16) | zClearValue_;
    dmaFill32(z, pZBuffer_, viewportPixelCount>>1);
  }
}
