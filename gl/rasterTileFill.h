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


#ifndef GL_RASTERTILEFILL_H
#define GL_RASTERTILEFILL_H


#include "rasterTileCommon.h"


namespace raster
{


//-----------------------------------------------------------------------------
struct SVertexTileFill
{
  int32_t x, y;
  float z, w;
  CFloat_4 c;
};

//-----------------------------------------------------------------------------
class CRasterizerTileFill
 : public CASoftRasterizer
{
public:
  CRasterizerTileFill();
  virtual ~CRasterizerTileFill();

  void rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2);

private:
  void _rasterTriangle(const SVertexTileFill & v0, const SVertexTileFill & v1, const SVertexTileFill & v2);
  void _rasterTrianglePoint(const STileFragment & frag, const SBoundingRect & rect);
};


};


#endif // GL_RASTERTILEFILL_H
