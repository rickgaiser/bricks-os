/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
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


#include "textures.h"
#include "bitResolution.h"
#include "asm/arch/config.h"


//-----------------------------------------------------------------------------
#define UV_TO_INDEX(U,V) ((((V) & iHeightMask_) * width) + ((U) & iWidthMask_))
#define GET_TEXEL(U,V)   ((TEXEL_FORMAT *)data)[UV_TO_INDEX((U), (V))];
//-----------------------------------------------------------------------------
void
CTexture::getTexel(SRasterColor & c, int32_t u, int32_t v, bool near)
{
  if((near == false) || (minFilter == GL_NEAREST))
  {
#if 1
    uint32_t texel = GET_TEXEL(u >> SHIFT_TEX, v >> SHIFT_TEX);
    TEXEL_LOAD(c, texel);
#else
    c.r = ((u)&0xfff) << 8;
    c.g = 0;
    c.b = 0;
    c.a = (1<<SHIFT_COLOR);
#endif
  }
  else
  {
    // Get the texel location
    int32_t upos = u >> SHIFT_TEX;
    int32_t vpos = v >> SHIFT_TEX;
    // Get the fractional parts (in fixed point color format)
    int32_t ufrac = (u & ((1<<SHIFT_TEX)-1)) >> (SHIFT_TEX - SHIFT_COLOR);
    int32_t vfrac = (v & ((1<<SHIFT_TEX)-1)) >> (SHIFT_TEX - SHIFT_COLOR);
    // Calculate the multiplications per texel
    int32_t cv00 = (((1 << SHIFT_COLOR) - ufrac) * ((1 << SHIFT_COLOR) - vfrac)) >> SHIFT_COLOR;
    int32_t cv01 = (((1 << SHIFT_COLOR) - ufrac) * (                     vfrac)) >> SHIFT_COLOR;
    int32_t cv10 = ((                     ufrac) * ((1 << SHIFT_COLOR) - vfrac)) >> SHIFT_COLOR;
    int32_t cv11 = ((                     ufrac) * (                     vfrac)) >> SHIFT_COLOR;

    uint32_t texel00;
    uint32_t texel01;
    uint32_t texel10;
    uint32_t texel11;

    // Only calculate the index near the left and bottom edge
    if((upos < (width-1)) && (vpos < (height-1)))
    {
      int index = UV_TO_INDEX(upos, vpos);
      texel00 = ((TEXEL_FORMAT *)data)[index];
      texel01 = ((TEXEL_FORMAT *)data)[index + width];
      texel10 = ((TEXEL_FORMAT *)data)[index + 1];
      texel11 = ((TEXEL_FORMAT *)data)[index + width + 1];
    }
    else
    {
      texel00 = GET_TEXEL(upos    , vpos    );
      texel01 = GET_TEXEL(upos    , vpos + 1);
      texel10 = GET_TEXEL(upos + 1, vpos    );
      texel11 = GET_TEXEL(upos + 1, vpos + 1);
    }

    SRasterColor c00;
    SRasterColor c01;
    SRasterColor c10;
    SRasterColor c11;

    TEXEL_LOAD(c00, texel00);
    TEXEL_LOAD(c01, texel01);
    TEXEL_LOAD(c10, texel10);
    TEXEL_LOAD(c11, texel11);

    c.r = (c00.r * cv00 + c01.r * cv01 + c10.r * cv10 + c11.r * cv11) >> SHIFT_COLOR;
    c.g = (c00.g * cv00 + c01.g * cv01 + c10.g * cv10 + c11.g * cv11) >> SHIFT_COLOR;
    c.b = (c00.b * cv00 + c01.b * cv01 + c10.b * cv10 + c11.b * cv11) >> SHIFT_COLOR;
    c.a = (c00.a * cv00 + c01.a * cv01 + c10.a * cv10 + c11.a * cv11) >> SHIFT_COLOR;
  }
}
