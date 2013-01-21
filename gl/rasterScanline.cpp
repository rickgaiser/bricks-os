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
#include "CDDA.h"
#ifdef ENABLE_PROFILING
#include "glProfiling.h"
#endif
#include "mathlib.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"


//#define SHIFT_XY            4
#define DEPTH_Z            14 // Resolution of depth buffer
#define SHIFT_Z            15 // Added resolution for interpolation
#define SHIFT_COLOR        12

#define SHIFT_COLOR_INTERP (30 - SHIFT_XY)
#define SHIFT_COLOR_CALC   14


#define INTERFACE_TO_INTERP_COLOR(c) ((c) << (SHIFT_COLOR_INTERP - SHIFT_COLOR))
#define INTERFACE_TO_CALC_COLOR(c)   ((c) << (SHIFT_COLOR_CALC   - SHIFT_COLOR))
#define INTERP_TO_CALC_COLOR(c)      ((c) >> (SHIFT_COLOR_INTERP - SHIFT_COLOR_CALC))


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
  prof_rasterTotal.start();
  prof_rasterMain.start();
#endif

  vtx0.x   = fpfromf(SHIFT_XY, (fXA_ * v0.vd.x) + fXB_);
  vtx0.y   = fpfromf(SHIFT_XY, (fYA_ * v0.vd.y) + fYB_);
  vtx0.z   = (fZA_ * v0.vd.z) + fZB_;
  vtx0.w   = v0.vd.w;
  vtx0.c   = v0.c;
  vtx0.t.u = v0.t[0];
  vtx0.t.v = v0.t[1];

  vtx1.x   = fpfromf(SHIFT_XY, (fXA_ * v1.vd.x) + fXB_);
  vtx1.y   = fpfromf(SHIFT_XY, (fYA_ * v1.vd.y) + fYB_);
  vtx1.z   = (fZA_ * v1.vd.z) + fZB_;
  vtx1.w   = v1.vd.w;
  vtx1.c   = v1.c;
  vtx1.t.u = v1.t[0];
  vtx1.t.v = v1.t[1];

  vtx2.x   = fpfromf(SHIFT_XY, (fXA_ * v2.vd.x) + fXB_);
  vtx2.y   = fpfromf(SHIFT_XY, (fYA_ * v2.vd.y) + fYB_);
  vtx2.z   = (fZA_ * v2.vd.z) + fZB_;
  vtx2.w   = v2.vd.w;
  vtx2.c   = v2.c;
  vtx2.t.u = v2.t[0];
  vtx2.t.v = v2.t[1];

  _rasterTriangle(vtx0, vtx1, vtx2);

#ifdef ENABLE_PROFILING
  prof_rasterMain.end();
  prof_rasterTotal.end();
#endif
}

//-----------------------------------------------------------------------------
const CInt32_4 constColorOne (1<<SHIFT_COLOR_CALC, 1<<SHIFT_COLOR_CALC, 1<<SHIFT_COLOR_CALC, 1<<SHIFT_COLOR_CALC);
const CInt32_4 constColorZero(0<<SHIFT_COLOR_CALC, 0<<SHIFT_COLOR_CALC, 0<<SHIFT_COLOR_CALC, 0<<SHIFT_COLOR_CALC);
//-----------------------------------------------------------------------------
const int32_t  constColorCompOne (1<<SHIFT_COLOR_CALC);
const int32_t  constColorCompZero(0<<SHIFT_COLOR_CALC);
//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterTexture(CInt32_4 & out, const CInt32_4 & cfragment, const CInt32_4 & ctexture)
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
      out.r = alphaChannel ? ((cfragment.r * (constColorCompOne - ctexture.a)) >> SHIFT_COLOR_CALC) + ((ctexture.r * ctexture.a) >> SHIFT_COLOR_CALC) : ctexture.r;
      out.g = alphaChannel ? ((cfragment.g * (constColorCompOne - ctexture.a)) >> SHIFT_COLOR_CALC) + ((ctexture.g * ctexture.a) >> SHIFT_COLOR_CALC) : ctexture.g;
      out.b = alphaChannel ? ((cfragment.b * (constColorCompOne - ctexture.a)) >> SHIFT_COLOR_CALC) + ((ctexture.b * ctexture.a) >> SHIFT_COLOR_CALC) : ctexture.b;
      out.a = cfragment.a;
      break;
    case GL_BLEND:
      out.r = ((cfragment.r * (constColorCompOne - ctexture.r)) >> SHIFT_COLOR_CALC) + ((texEnvColorFX_.r * ctexture.r) >> SHIFT_COLOR_CALC);
      out.g = ((cfragment.g * (constColorCompOne - ctexture.g)) >> SHIFT_COLOR_CALC) + ((texEnvColorFX_.g * ctexture.g) >> SHIFT_COLOR_CALC);
      out.b = ((cfragment.b * (constColorCompOne - ctexture.b)) >> SHIFT_COLOR_CALC) + ((texEnvColorFX_.b * ctexture.b) >> SHIFT_COLOR_CALC);
      out.a = alphaChannel ? (cfragment.a * ctexture.a) >> SHIFT_COLOR_CALC : cfragment.a;
      break;
    case GL_ADD:
      out.r = cfragment.r + ctexture.r;
      out.g = cfragment.g + ctexture.g;
      out.b = cfragment.b + ctexture.b;
      out.a = alphaChannel ? (cfragment.a * ctexture.a) >> SHIFT_COLOR_CALC : cfragment.a;
      break;
  };
}

//-----------------------------------------------------------------------------
inline CInt32_4
colorBlendFactor(const GLenum & factor, const CInt32_4 & source, const CInt32_4 & dest)
{
  switch(factor)
  {
    case GL_ONE:                 return constColorOne;
    case GL_SRC_COLOR:           return source;
    case GL_ONE_MINUS_SRC_COLOR: return constColorOne - source;
    case GL_DST_COLOR:           return dest;
    case GL_ONE_MINUS_DST_COLOR: return constColorOne - dest;
    case GL_SRC_ALPHA:           { CInt32_4 c; c.r = c.g = c.b = c.a = source.a;                     return c; }
    case GL_ONE_MINUS_SRC_ALPHA: { CInt32_4 c; c.r = c.g = c.b = c.a = constColorCompOne - source.a; return c; }
    case GL_DST_ALPHA:           { CInt32_4 c; c.r = c.g = c.b = c.a = dest.a;                       return c; }
    case GL_ONE_MINUS_DST_ALPHA: { CInt32_4 c; c.r = c.g = c.b = c.a = constColorCompOne - dest.a;   return c; }
    case GL_ZERO:
    default:
      return constColorZero;
  }
}

//-----------------------------------------------------------------------------
inline CInt32_4
color_clamp_max(const CInt32_4 & c)
{
  CInt32_4 ret;
  ret.r = mathlib::clamp_max(c.r, constColorCompOne);
  ret.g = mathlib::clamp_max(c.g, constColorCompOne);
  ret.b = mathlib::clamp_max(c.b, constColorCompOne);
  ret.a = mathlib::clamp_max(c.a, constColorCompOne);
  return ret;
}

//-----------------------------------------------------------------------------
inline CInt32_4
color_clamp(const CInt32_4 & c)
{
  CInt32_4 ret;
  ret.r = mathlib::clamp(c.r, constColorCompZero, constColorCompOne);
  ret.g = mathlib::clamp(c.g, constColorCompZero, constColorCompOne);
  ret.b = mathlib::clamp(c.b, constColorCompZero, constColorCompOne);
  ret.a = mathlib::clamp(c.a, constColorCompZero, constColorCompOne);
  return ret;
}

//-----------------------------------------------------------------------------
void
CRasterizerScanline::rasterBlend(CInt32_4 & out, const CInt32_4 & source, const CInt32_4 & dest)
{
  switch(blendFast_)
  {
    case FB_ZERO:   out = constColorZero; break;
    case FB_SOURCE: out = source; break;
    case FB_DEST:   out = dest; break;
    case FB_ADD:    out = color_clamp_max(source + dest); break;
    case FB_BLEND:  out = mathlib::lerp_fx(source.a, dest, source, SHIFT_COLOR_CALC); break;

    default:
    {
      CInt32_4 cs = colorBlendFactor(blendSFactor_, source, dest);
      CInt32_4 cd = colorBlendFactor(blendDFactor_, source, dest);
      out = color_clamp_max(((source * cs) + (dest * cd)) >> SHIFT_COLOR_CALC);
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
