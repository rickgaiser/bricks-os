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


#ifndef GL_RASTERSCANLINE_H
#define GL_RASTERSCANLINE_H


#include "GL/gl.h"
#include "raster.h"
#include "rasterCommon.h"
#include "textures.h"

#ifdef __BRICKS__
#include "asm/arch/config.h"
#include "asm/arch/memory.h"
#else
//#define CONFIG_GL_TINY
#define FAST_CODE
#endif


namespace raster
{


//-----------------------------------------------------------------------------
class CRasterizerScanline
 : public CASoftRasterizer
{
public:
  CRasterizerScanline();
  virtual ~CRasterizerScanline();

  void rasterTriangle(const SVertex & v0, const SVertex & v1, const SVertex & v2);

private:
  void rasterTexture(raster::SColor & out, const raster::SColor & cfragment, const raster::SColor & ctexture) FAST_CODE;
#ifndef CONFIG_GL_TINY
  void rasterBlend(raster::SColor & out, const raster::SColor & source, const raster::SColor & dest) FAST_CODE;
#endif

  void _rasterTriangle(const SVertex & v0, const SVertex & v1, const SVertex & v2) FAST_CODE;

  void raster    (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterZ   (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterT   (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterTZ  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
#ifndef CONFIG_GL_TINY
  void rasterC   (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterCZ  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterCT  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterCTZ (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterB   (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterBZ  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterBT  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterBTZ (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterBC  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterBCZ (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterBCT (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
  void rasterBCTZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom) FAST_CODE;
#endif

private:
  // Triangle gradients
  TGradient<SColor>    grad_c; // Color
  TGradient<int32_t>   grad_z; // Depth
#ifndef CONFIG_GL_TINY
  TGradient<float>     grad_w; // Depth
#endif
  TGradient<TTexCoord<float> >   grad_t; // Textures
};


};


#endif // GL_RASTERSCANLINE_H
