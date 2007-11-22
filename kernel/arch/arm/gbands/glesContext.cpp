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
    switch(renderSurface->bpp_)
    {
      case 8:
      {
        // FIXME!
        uint32_t color = 0x00000000;
        dmaFill32(color, renderSurface->p, viewportPixelCount>>2);
      }
      case 16:
      {
        uint32_t color = (fpRGB(clClear.r, clClear.g, clClear.b) << 16) | fpRGB(clClear.r, clClear.g, clClear.b);
        dmaFill32(color, renderSurface->p, viewportPixelCount>>1);
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
IGLESContext *
getGLESContext()
{
  return new CGBAGLESContext;
}
