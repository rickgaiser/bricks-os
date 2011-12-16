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
#ifdef ENABLE_PROFILING
#include "glProfiling.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "mathlib.h"


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
  texEnvColorFX_.r = fpfromf(SHIFT_COLOR_CALC, texEnvColor_.r);
  texEnvColorFX_.g = fpfromf(SHIFT_COLOR_CALC, texEnvColor_.r);
  texEnvColorFX_.b = fpfromf(SHIFT_COLOR_CALC, texEnvColor_.r);
  texEnvColorFX_.a = fpfromf(SHIFT_COLOR_CALC, texEnvColor_.r);

  alphaValueFX_ = fpfromf(SHIFT_COLOR_CALC, alphaValue_);
}

//-----------------------------------------------------------------------------
CRasterizerScanline::~CRasterizerScanline()
{
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::texEnvfv(GLenum target, GLenum pname, const GLfloat * params)
{
  CASoftRasterizer::texEnvfv(target, pname, params);

  texEnvColorFX_.r = fpfromf(SHIFT_COLOR_CALC, texEnvColor_.r);
  texEnvColorFX_.g = fpfromf(SHIFT_COLOR_CALC, texEnvColor_.r);
  texEnvColorFX_.b = fpfromf(SHIFT_COLOR_CALC, texEnvColor_.r);
  texEnvColorFX_.a = fpfromf(SHIFT_COLOR_CALC, texEnvColor_.r);
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::alphaFunc(GLenum func, GLclampf ref)
{
  CASoftRasterizer::alphaFunc(func, ref);

  alphaValueFX_ = fpfromf(SHIFT_COLOR_CALC, alphaValue_);
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2)
{
  SVertex vtx0, vtx1, vtx2;

#ifdef ENABLE_PROFILING
  prof_rasterMain.start();
#endif

  vtx0.x   = fpfromf(SHIFT_XY, (fXA_ * v0.vd.x) + fXB_);
  vtx0.y   = fpfromf(SHIFT_XY, (fYA_ * v0.vd.y) + fYB_);
  vtx0.z   = (fZA_ * v0.vd.z) + fZB_;
  vtx0.w   = v0.vd.w;
  vtx0.c.r = (int32_t)(v0.c.r * (1<<SHIFT_COLOR));
  vtx0.c.g = (int32_t)(v0.c.g * (1<<SHIFT_COLOR));
  vtx0.c.b = (int32_t)(v0.c.b * (1<<SHIFT_COLOR));
  vtx0.c.a = (int32_t)(v0.c.a * (1<<SHIFT_COLOR));
  vtx0.t.u = v0.t[0];
  vtx0.t.v = v0.t[1];

  vtx1.x   = fpfromf(SHIFT_XY, (fXA_ * v1.vd.x) + fXB_);
  vtx1.y   = fpfromf(SHIFT_XY, (fYA_ * v1.vd.y) + fYB_);
  vtx1.z   = (fZA_ * v1.vd.z) + fZB_;
  vtx1.w   = v1.vd.w;
  vtx1.c.r = (int32_t)(v1.c.r * (1<<SHIFT_COLOR));
  vtx1.c.g = (int32_t)(v1.c.g * (1<<SHIFT_COLOR));
  vtx1.c.b = (int32_t)(v1.c.b * (1<<SHIFT_COLOR));
  vtx1.c.a = (int32_t)(v1.c.a * (1<<SHIFT_COLOR));
  vtx1.t.u = v1.t[0];
  vtx1.t.v = v1.t[1];

  vtx2.x   = fpfromf(SHIFT_XY, (fXA_ * v2.vd.x) + fXB_);
  vtx2.y   = fpfromf(SHIFT_XY, (fYA_ * v2.vd.y) + fYB_);
  vtx2.z   = (fZA_ * v2.vd.z) + fZB_;
  vtx2.w   = v2.vd.w;
  vtx2.c.r = (int32_t)(v2.c.r * (1<<SHIFT_COLOR));
  vtx2.c.g = (int32_t)(v2.c.g * (1<<SHIFT_COLOR));
  vtx2.c.b = (int32_t)(v2.c.b * (1<<SHIFT_COLOR));
  vtx2.c.a = (int32_t)(v2.c.a * (1<<SHIFT_COLOR));
  vtx2.t.u = v2.t[0];
  vtx2.t.v = v2.t[1];

  _rasterTriangle(vtx0, vtx1, vtx2);

#ifdef ENABLE_PROFILING
  prof_rasterMain.end();
#endif
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterTexture(TColor<int32_t> & out, const TColor<int32_t> & cfragment, const TColor<int32_t> & ctexture)
{
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
      out.r = COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.r, ((1<<SHIFT_COLOR_CALC)-ctexture.r)) + COLOR_MUL_COMP(SHIFT_COLOR_CALC, texEnvColorFX_.r, ctexture.r);
      out.g = COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.g, ((1<<SHIFT_COLOR_CALC)-ctexture.g)) + COLOR_MUL_COMP(SHIFT_COLOR_CALC, texEnvColorFX_.g, ctexture.g);
      out.b = COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.b, ((1<<SHIFT_COLOR_CALC)-ctexture.b)) + COLOR_MUL_COMP(SHIFT_COLOR_CALC, texEnvColorFX_.b, ctexture.b);
      out.a = alphaChannel ? COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.a, ctexture.a) : cfragment.a;
      break;
    case GL_ADD:
      out.r = cfragment.r + ctexture.r;
      out.g = cfragment.g + ctexture.g;
      out.b = cfragment.b + ctexture.b;
      out.a = alphaChannel ? COLOR_MUL_COMP(SHIFT_COLOR_CALC, cfragment.a, ctexture.a) : cfragment.a;
      break;
  };
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBlend(TColor<int32_t> & out, const TColor<int32_t> & source, const TColor<int32_t> & dest)
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
    case FB_ZERO:
      out.r = 0;
      out.g = 0;
      out.b = 0;
      out.a = 0;
      break;
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
    case FB_ADD:
      out.r = COLOR_CLAMP_TOP_COMP(source.r + dest.r, SHIFT_COLOR_CALC);
      out.g = COLOR_CLAMP_TOP_COMP(source.g + dest.g, SHIFT_COLOR_CALC);
      out.b = COLOR_CLAMP_TOP_COMP(source.b + dest.b, SHIFT_COLOR_CALC);
      out.a = COLOR_CLAMP_TOP_COMP(source.a + dest.a, SHIFT_COLOR_CALC);
      break;
    case FB_BLEND:
    {
      COLOR_AVG(out, dest, source, source.a, SHIFT_COLOR_CALC);
      COLOR_CLAMP(out, out, SHIFT_COLOR_CALC);
      break;
    }
    default:
    {
      TColor<int32_t> cs, cd;
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
      mathlib::swap(vtop, vmiddle);

    // Swap middle with top?
    if(vmiddle->y > vbottom->y)
    {
      mathlib::swap(vmiddle, vbottom);

      // Swap bottom with middle again?
      if(vtop->y > vmiddle->y)
        mathlib::swap(vtop, vmiddle);
    }
  }

  // Calculate function code
  uint32_t function_code =
    (bBlendingEnabled_      << 3) |
    (bSmoothShadingEnabled_ << 2) |
    (bTexturesEnabled_      << 1) |
    (bDepthTestEnabled_         );
  // Execute selected function
  switch(function_code)
  {
    case 0x00: raster    (vtop, vmiddle, vbottom); break;
    case 0x01: rasterZ   (vtop, vmiddle, vbottom); break;
    case 0x02: rasterT   (vtop, vmiddle, vbottom); break;
    case 0x03: rasterTZ  (vtop, vmiddle, vbottom); break;
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


};
