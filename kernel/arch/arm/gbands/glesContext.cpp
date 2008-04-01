#include "glesContext.h"
#include "asm/arch/macros.h"


#define BxColorFormat_FromFxRGB(format,r,g,b) \
  BxColorFormat_FromRGB(format, \
                        (r * 255) >> 16, \
                        (g * 255) >> 16, \
                        (b * 255) >> 16);


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
        uint32_t color = BxColorFormat_FromFxRGB(renderSurface->mode.format, clClear.r, clClear.g, clClear.b);
        color = (color << 24) | (color << 16) | (color << 8) | color;
        if(renderSurface->mode.xpitch == renderSurface->mode.width)
        {
          dmaFill32(color, renderSurface->p, viewportPixelCount>>2);
        }
        else
        {
          unsigned int iBase(viewportYOffset * renderSurface->mode.xpitch + viewportXOffset);
          for(unsigned int iY(0); iY < renderSurface->mode.height; iY++)
          {
            dmaFill16(color, &((uint8_t *)renderSurface->p)[iBase], renderSurface->mode.width);
            iBase += renderSurface->mode.xpitch;
          }
        }
        break;
      }
      case 16:
      {
        uint32_t color = BxColorFormat_FromFxRGB(renderSurface->mode.format, clClear.r, clClear.g, clClear.b);
        color = (color << 16) | color;
        if(renderSurface->mode.xpitch == renderSurface->mode.width)
        {
          dmaFill32(color, renderSurface->p, viewportPixelCount>>1);
        }
        else
        {
          unsigned int iBase(viewportYOffset * renderSurface->mode.xpitch + viewportXOffset);
          for(unsigned int iY(0); iY < renderSurface->mode.height; iY++)
          {
            dmaFill16(color, &((uint16_t *)renderSurface->p)[iBase], renderSurface->mode.width);
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
    dmaFill32(z, zbuffer, viewportPixelCount>>1);
  }
}
