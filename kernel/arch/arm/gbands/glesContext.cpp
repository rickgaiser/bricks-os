/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


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
