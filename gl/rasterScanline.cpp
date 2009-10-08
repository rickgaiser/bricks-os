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


#include "rasterScanline.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "mathlib.h"


#ifndef CONFIG_GL_TINY
  #define CONFIG_GL_ENABLE_ALPHA_TEST
  #define CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  //#define CONFIG_GL_SIMPLE_TEXTURES
#else
  //#define CONFIG_GL_ENABLE_ALPHA_TEST
  //#define CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  #define CONFIG_GL_SIMPLE_TEXTURES
#endif


#define SHIFT_COLOR_INTERP (30 - SHIFT_XY)
#define SHIFT_COLOR_CALC   14


// INTERFACE_TO_INTERP_COLOR
#if SHIFT_COLOR > SHIFT_COLOR_INTERP
  #define INTERFACE_TO_INTERP_COLOR(c) ((c) >> (SHIFT_COLOR - SHIFT_COLOR_INTERP))
#elif SHIFT_COLOR < SHIFT_COLOR_INTERP
  #define INTERFACE_TO_INTERP_COLOR(c) ((c) << (SHIFT_COLOR_INTERP - SHIFT_COLOR))
#else
  #define INTERFACE_TO_INTERP_COLOR(c) ((c))
#endif

// INTERFACE_TO_CALC_COLOR
#if SHIFT_COLOR > SHIFT_COLOR_CALC
  #define INTERFACE_TO_CALC_COLOR(c) ((c) >> (SHIFT_COLOR - SHIFT_COLOR_CALC))
#elif SHIFT_COLOR < SHIFT_COLOR_CALC
  #define INTERFACE_TO_CALC_COLOR(c) ((c) << (SHIFT_COLOR_CALC - SHIFT_COLOR))
#else
  #define INTERFACE_TO_CALC_COLOR(c) ((c))
#endif

// INTERP_TO_CALC_COLOR
#if SHIFT_COLOR_INTERP > SHIFT_COLOR_CALC
  #define INTERP_TO_CALC_COLOR(c) ((c) >> (SHIFT_COLOR_INTERP - SHIFT_COLOR_CALC))
#elif SHIFT_COLOR_INTERP < SHIFT_COLOR_CALC
  #define INTERP_TO_CALC_COLOR(c) ((c) << (SHIFT_COLOR_CALC - SHIFT_COLOR_INTERP))
#else
  #define INTERP_TO_CALC_COLOR(c) ((c))
#endif


namespace raster
{


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CRasterizerScanline::CRasterizerScanline()
 : CASoftRasterizer()
{
}

//-----------------------------------------------------------------------------
CRasterizerScanline::~CRasterizerScanline()
{
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterTriangle(const SVertex & v0, const SVertex & v1, const SVertex & v2)
{
#ifdef ENABLE_PROFILING
  prof_raster.start();
#endif

  _rasterTriangle(v0, v1, v2);

#ifdef ENABLE_PROFILING
  prof_raster.end();
#endif
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterTexture(SColor & out, const SColor & cfragment, const SColor & ctexture)
{
#ifdef CONFIG_GL_TINY
  if(texEnvMode_ == GL_MODULATE)
  {
    out.r = (cfragment.r * ctexture.r) >> SHIFT_COLOR_CALC;
    out.g = (cfragment.g * ctexture.g) >> SHIFT_COLOR_CALC;
    out.b = (cfragment.b * ctexture.b) >> SHIFT_COLOR_CALC;
  }
  else
    out = ctexture;
#else
#if 1
  out = ctexture;
#else
  bool alphaChannel = pCurrentTex_->bRGBA_;

  switch(texEnvMode_)
  {
    case GL_REPLACE:
      out.r = ctexture.r;
      out.g = ctexture.g;
      out.b = ctexture.b;
      out.a = alphaChannel ? ctexture.a : cfragment.a;
      break;
    case GL_MODULATE:
      // Texture * Fragment color
      out.r = (cfragment.r * ctexture.r) >> SHIFT_COLOR_CALC;
      out.g = (cfragment.g * ctexture.g) >> SHIFT_COLOR_CALC;
      out.b = (cfragment.b * ctexture.b) >> SHIFT_COLOR_CALC;
      out.a = alphaChannel ? ((cfragment.a * ctexture.a) >> SHIFT_COLOR_CALC) : cfragment.a;
      break;
    case GL_DECAL:
      out.r = alphaChannel ? (COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.r, ((1<<SHIFT_COLOR_CALC)-ctexture.a)) + COLOR_MUL_COMP(SHIFT_COLOR_CALC, ctexture.r, ctexture.a)) : ctexture.r;
      out.g = alphaChannel ? (COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.g, ((1<<SHIFT_COLOR_CALC)-ctexture.a)) + COLOR_MUL_COMP(SHIFT_COLOR_CALC, ctexture.g, ctexture.a)) : ctexture.g;
      out.b = alphaChannel ? (COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.b, ((1<<SHIFT_COLOR_CALC)-ctexture.a)) + COLOR_MUL_COMP(SHIFT_COLOR_CALC, ctexture.b, ctexture.a)) : ctexture.b;
      out.a = cfragment.a;
      break;
    case GL_BLEND:
      //out.r = COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.r, ((1<<SHIFT_COLOR_CALC)-ctexture.r)) + COLOR_MUL_COMP(SHIFT_COLOR_CALC, texEnvColorFX_.r, ctexture.r);
      //out.g = COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.g, ((1<<SHIFT_COLOR_CALC)-ctexture.g)) + COLOR_MUL_COMP(SHIFT_COLOR_CALC, texEnvColorFX_.g, ctexture.g);
      //out.b = COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.b, ((1<<SHIFT_COLOR_CALC)-ctexture.b)) + COLOR_MUL_COMP(SHIFT_COLOR_CALC, texEnvColorFX_.b, ctexture.b);
      //out.a = alphaChannel ? COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.a, ctexture.a) : cfragment.a;
      break;
    case GL_ADD:
      out.r = cfragment.r + ctexture.r;
      out.g = cfragment.g + ctexture.g;
      out.b = cfragment.b + ctexture.b;
      out.a = alphaChannel ? COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.a, ctexture.a) : cfragment.a;
      break;
  };
#endif
#endif
}

#ifndef CONFIG_GL_TINY
//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBlend(SColor & out, const SColor & source, const SColor & dest)
{
  #define BLEND_FACTOR(factor,c) \
  switch(factor) \
  { \
    case GL_ONE: \
      c.r = c.g = c.b = c.a = (1<<SHIFT_COLOR_CALC); \
      break; \
    case GL_SRC_COLOR: \
      c.r = source.r; \
      c.g = source.g; \
      c.b = source.b; \
      c.a = source.a; \
      break; \
    case GL_ONE_MINUS_SRC_COLOR: \
      c.r = (1<<SHIFT_COLOR_CALC) - source.r; \
      c.g = (1<<SHIFT_COLOR_CALC) - source.g; \
      c.b = (1<<SHIFT_COLOR_CALC) - source.b; \
      c.a = (1<<SHIFT_COLOR_CALC) - source.a; \
      break; \
    case GL_DST_COLOR: \
      c.r = dest.r; \
      c.g = dest.g; \
      c.b = dest.b; \
      c.a = dest.a; \
      break; \
    case GL_ONE_MINUS_DST_COLOR: \
      c.r = (1<<SHIFT_COLOR_CALC) - dest.r; \
      c.g = (1<<SHIFT_COLOR_CALC) - dest.g; \
      c.b = (1<<SHIFT_COLOR_CALC) - dest.b; \
      c.a = (1<<SHIFT_COLOR_CALC) - dest.a; \
      break; \
    case GL_SRC_ALPHA: \
      c.r = c.g = c.b = c.a = source.a; \
      break; \
    case GL_ONE_MINUS_SRC_ALPHA: \
      c.r = c.g = c.b = c.a = (1<<SHIFT_COLOR_CALC) - source.a; \
      break; \
    case GL_DST_ALPHA: \
      c.r = c.g = c.b = c.a = dest.a; \
      break; \
    case GL_ONE_MINUS_DST_ALPHA: \
      c.r = c.g = c.b = c.a = (1<<SHIFT_COLOR_CALC) - dest.a; \
      break; \
    case GL_ZERO: \
    default: \
      c.r = c.g = c.b = c.a = 0; \
  }

  switch(blendFast_)
  {
    case FB_SOURCE:
      out.r = source.r;
      out.g = source.g;
      out.b = source.b;
      out.a = source.a;
      break;
    case FB_DEST:
      out.r = dest.r;
      out.g = dest.g;
      out.b = dest.b;
      out.a = dest.a;
      break;
    case FB_BLEND:
    {
      COLOR_AVG(out, dest, source, source.a, SHIFT_COLOR_CALC);
      COLOR_CLAMP(out, out, SHIFT_COLOR_CALC);
      break;
    }
    default:
    {
      SColor cs, cd;
      BLEND_FACTOR(blendSFactor_, cs);
      BLEND_FACTOR(blendDFactor_, cd);

      cd.r = ((source.r * cs.r) + (dest.r * cd.r)) >> SHIFT_COLOR_CALC;
      cd.g = ((source.g * cs.g) + (dest.g * cd.g)) >> SHIFT_COLOR_CALC;
      cd.b = ((source.b * cs.b) + (dest.b * cd.b)) >> SHIFT_COLOR_CALC;
      cd.a = ((source.a * cs.a) + (dest.a * cd.a)) >> SHIFT_COLOR_CALC;

      COLOR_CLAMP_TOP(out, cd, SHIFT_COLOR_CALC);
    }
  }
}
#endif

//-----------------------------------------------------------------------------
void
CRasterizerScanline::_rasterTriangle(const SVertex & v0, const SVertex & v1, const SVertex & v2)
{
  // Bubble sort the 3 vertexes
  // NOTE: Top < Bottom
  const SVertex * vbottom(&v0);
  const SVertex * vmiddle(&v1);
  const SVertex * vtop   (&v2);
  {
    // Swap bottom with middle?
    if(vtop->y > vmiddle->y)
      mathlib::swap<const SVertex *>(vtop, vmiddle);

    // Swap middle with top?
    if(vmiddle->y > vbottom->y)
    {
      mathlib::swap<const SVertex *>(vmiddle, vbottom);

      // Swap bottom with middle again?
      if(vtop->y > vmiddle->y)
        mathlib::swap<const SVertex *>(vtop, vmiddle);
    }
  }

  // Calculate function code
  uint32_t function_code =
#ifndef CONFIG_GL_TINY
    (bBlendingEnabled_      << 3) |
    (bSmoothShadingEnabled_ << 2) |
#endif
    (bTexturesEnabled_      << 1) |
    (bDepthTestEnabled_         );
  // Execute selected function
  switch(function_code)
  {
    case 0x00: raster    (vtop, vmiddle, vbottom); break;
    case 0x01: rasterZ   (vtop, vmiddle, vbottom); break;
    case 0x02: rasterT   (vtop, vmiddle, vbottom); break;
    case 0x03: rasterTZ  (vtop, vmiddle, vbottom); break;
#ifndef CONFIG_GL_TINY
    case 0x04: rasterC   (vtop, vmiddle, vbottom); break;
    case 0x05: rasterCZ  (vtop, vmiddle, vbottom); break;
    case 0x06: rasterCT  (vtop, vmiddle, vbottom); break;
    case 0x07: rasterCTZ (vtop, vmiddle, vbottom); break;
    case 0x08: rasterB   (vtop, vmiddle, vbottom); break;
    case 0x09: rasterBZ  (vtop, vmiddle, vbottom); break;
    case 0x0a: rasterBT  (vtop, vmiddle, vbottom); break;
    case 0x0b: rasterBTZ (vtop, vmiddle, vbottom); break;
    case 0x0c: rasterBC  (vtop, vmiddle, vbottom); break;
    case 0x0d: rasterBCZ (vtop, vmiddle, vbottom); break;
    case 0x0e: rasterBCT (vtop, vmiddle, vbottom); break;
    case 0x0f: rasterBCTZ(vtop, vmiddle, vbottom); break;
#endif
  };
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::raster(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_DEPTH_TEST

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterT(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_TEXTURES

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterTZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_DEPTH_TEST
  #define RASTER_ENABLE_TEXTURES

  #include "rasterScanline.inl"
}

#ifndef CONFIG_GL_TINY
//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterC(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_SMOOTH_SHADING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterCZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_DEPTH_TEST
  #define RASTER_ENABLE_SMOOTH_SHADING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterCT(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_TEXTURES
  #define RASTER_ENABLE_SMOOTH_SHADING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterCTZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_DEPTH_TEST
  #define RASTER_ENABLE_TEXTURES
  #define RASTER_ENABLE_SMOOTH_SHADING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterB(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_BLENDING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_DEPTH_TEST
  #define RASTER_ENABLE_BLENDING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBT(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_TEXTURES
  #define RASTER_ENABLE_BLENDING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBTZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_DEPTH_TEST
  #define RASTER_ENABLE_TEXTURES
  #define RASTER_ENABLE_BLENDING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBC(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_SMOOTH_SHADING
  #define RASTER_ENABLE_BLENDING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBCZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_DEPTH_TEST
  #define RASTER_ENABLE_SMOOTH_SHADING
  #define RASTER_ENABLE_BLENDING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBCT(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_TEXTURES
  #define RASTER_ENABLE_SMOOTH_SHADING
  #define RASTER_ENABLE_BLENDING

  #include "rasterScanline.inl"
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBCTZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom)
{
  #define RASTER_ENABLE_DEPTH_TEST
  #define RASTER_ENABLE_TEXTURES
  #define RASTER_ENABLE_SMOOTH_SHADING
  #define RASTER_ENABLE_BLENDING

  #include "rasterScanline.inl"
}
#endif


};