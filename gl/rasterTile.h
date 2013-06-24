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


#ifndef GL_RASTERTILE_H
#define GL_RASTERTILE_H


#include "GL/gl.h"
#include "raster.h"
#include "rasterCommon.h"
#include "rasterTileCommon.h"


namespace raster
{


//-----------------------------------------------------------------------------
struct SVertexTile
{
  int32_t x, y;
  float z, w;
  CFloat_4 c;
  CFloat_2 t;
};

//-----------------------------------------------------------------------------
//#define INLINE_FUNC inline
#define INLINE_FUNC
class CRasterizerTile
 : public CASoftRasterizer
{
public:
  CRasterizerTile();
  virtual ~CRasterizerTile();

  void setSurface(CSurface * surface);
  void flush();

  void rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2);

private:
  inline void _rasterTriangle(const SVertexTile & v0, const SVertexTile & v1, const SVertexTile & v2);
  void doTile(STile & tile, int x, int y);

  void placeInTiles       (const STileFragment & frag, const SBoundingRect & rect);
  void rasterTriangleBlock(const STileFragment & frag, const SBoundingRect & rect);
  void rasterTrianglePoint(const STileFragment & frag, const SBoundingRect & rect);

  template<const int blockShift> INLINE_FUNC void rasterTriangleDyn (const STileFragment & frag, const int & x, const int & y);
  template<const int blockShift> INLINE_FUNC void rasterTriangleDyn3(const STileFragment & frag, const int & x, const int & y, const SEdgeWorkSet & c1, const SEdgeWorkSet & c2, const SEdgeWorkSet & c3, const int32_t &  ic1, const int32_t &  ic2, const int32_t &  ic3);

  INLINE_FUNC void draw2x2WithMask(const STileFragment & frag, const int & x, const int & y, const CInt32_4 & mask);
  void drawBlock(const STileFragment & frag, int x, int y, int blockSize);
  void drawPoint(const STileFragment & frag, int x, int y);

private:
  CTileSet tileSet_;
  STileFragment * tileFragments_;
  int fragmentIndex_;
};


};


#endif // GL_RASTERTILE_H
