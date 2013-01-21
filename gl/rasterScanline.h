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
#include "vhl/color.h"
#include "vhl/CInt32_4.h"
#include "vhl/TTexCoord.h"


#define CONFIG_GL_ENABLE_ALPHA_TEST
#define CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
//#define CONFIG_GL_SIMPLE_TEXTURES


namespace raster
{


//-----------------------------------------------------------------------------
struct SVertex
{
  int32_t            x;
  int32_t            y;
  GLfloat            z;
  GLfloat            w;

  TColor<GLfloat>    c;
  TTexCoord<GLfloat> t;
};

//-----------------------------------------------------------------------------
class CRasterizerScanline
 : public CASoftRasterizer
{
public:
  CRasterizerScanline();
  virtual ~CRasterizerScanline();

  void texEnvfv(GLenum target, GLenum pname, const GLfloat * params);
  void alphaFunc(GLenum func, GLclampf ref);

  void rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2);

private:
  void rasterTexture(CInt32_4 & out, const CInt32_4 & cfragment, const CInt32_4 & ctexture);
  void rasterBlend(CInt32_4 & out, const CInt32_4 & source, const CInt32_4 & dest);

  void _rasterTriangle(const SVertex & v0, const SVertex & v1, const SVertex & v2);

  void raster    (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterZ   (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterT   (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterTZ  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterC   (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterCZ  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterCT  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterCTZ (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterB   (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterBZ  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterBT  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterBTZ (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterBC  (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterBCZ (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterBCT (const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);
  void rasterBCTZ(const SVertex * vtop, const SVertex * vmiddle, const SVertex * vbottom);

private:
  // Color
  CInt32_4                grad_c_ddx;
  CInt32_4                grad_c_ddy;
  CInt32_4                edge_c_current;
  CInt32_4                edge_c_increment;
  CInt32_4                scan_c_current;
  CInt32_4                scan_c_ddx;
  // Depth (z=1/w)
  GLfloat                 grad_z_ddx;
  GLfloat                 grad_z_ddy;
  GLfloat                 edge_z_current;
  GLfloat                 edge_z_increment;
  GLfloat                 scan_z_current;
  GLfloat                 scan_z_ddx;
#ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  // Depth (w=1/z)
  GLfloat                 grad_w_ddx;
  GLfloat                 grad_w_ddy;
  GLfloat                 edge_w_current;
  GLfloat                 edge_w_increment;
  GLfloat                 scan_w_current;
  GLfloat                 scan_w_ddx;
  // Texture (t/z)
  TTexCoord<GLfloat>      grad_tz_ddx;
  TTexCoord<GLfloat>      grad_tz_ddy;
  TTexCoord<GLfloat>      edge_tz_current;
  TTexCoord<GLfloat>      edge_tz_increment;
  TTexCoord<GLfloat>      scan_tz_current;
  TTexCoord<GLfloat>      scan_tz_ddx;
#else
  // Texture
  TTexCoord<GLfloat>      grad_t_ddx;
  TTexCoord<GLfloat>      grad_t_ddy;
  TTexCoord<GLfloat>      edge_t_current;
  TTexCoord<GLfloat>      edge_t_increment;
  TTexCoord<GLfloat>      scan_t_current;
  TTexCoord<GLfloat>      scan_t_ddx;
#endif

  CInt32_4                texEnvColorFX_;
  int32_t                 alphaValueFX_;
};


};


#endif // GL_RASTERSCANLINE_H
