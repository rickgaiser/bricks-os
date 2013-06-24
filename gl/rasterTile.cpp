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

#include "rasterTile.h"
#ifdef ENABLE_PROFILING
#include "glProfiling.h"
#endif
#include "textures.h"
#include "mathlib.h"
#include "vhl/CInt32_4.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"


#define COLORED_BLOCK_TEST
//#define PLACE_IN_TILES
#define BLOCK_ITERATION
//#define LAST_BLOCK_4X4


const CInt32_4 i4Zero(0);
const CInt32_4 i4Bit0(0x00000001);
const CInt32_4 i4Bit1(0x00000002);
const CInt32_4 i4Bit2(0x00000004);
const CInt32_4 i4Full(0xffffffff);


namespace raster
{


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CRasterizerTile::CRasterizerTile()
 : CASoftRasterizer()
{
  tileFragments_ = new STileFragment[MAX_FRAGMENTS];
  fragmentIndex_ = 0;
}

//-----------------------------------------------------------------------------
CRasterizerTile::~CRasterizerTile()
{
  if(tileFragments_ != NULL)
  {
    delete[] tileFragments_;
    tileFragments_ = NULL;
  }
}

//-----------------------------------------------------------------------------
void
CRasterizerTile::setSurface(CSurface * surface)
{
  CASoftRasterizer::setSurface(surface);

  tileSet_.generateTileSet(surface->width(), surface->height());
}

//-----------------------------------------------------------------------------
void
CRasterizerTile::flush()
{
#ifdef ENABLE_PROFILING
  prof_rasterTotal.start();
  prof_rasterMain.start();
#endif

  for(int iTile = 0; iTile < tileSet_.tileCount; iTile++)
    doTile(tileSet_.tile[iTile], tileSet_.tile[iTile].xpos, tileSet_.tile[iTile].ypos);
  // Reset fragment index
  fragmentIndex_ = 0;

#ifdef ENABLE_PROFILING
  prof_rasterMain.end();
  prof_rasterTotal.end();
#endif
}

//-----------------------------------------------------------------------------
void
CRasterizerTile::rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2)
{
  SVertexTile vtx0, vtx1, vtx2;

#ifdef ENABLE_PROFILING
  prof_rasterTotal.start();
#endif

  vtx0.x = fpfromf(SHIFT_XY, (fXA_ * v0.vd.x) + fXB_);
  vtx0.y = fpfromf(SHIFT_XY, (fYA_ * v0.vd.y) + fYB_);
  vtx0.z = (fZA_ * v0.vd.z) + fZB_;
  vtx0.w = v0.vd.w;
  vtx0.c = v0.c;
  vtx0.t = v0.t;

  vtx1.x = fpfromf(SHIFT_XY, (fXA_ * v1.vd.x) + fXB_);
  vtx1.y = fpfromf(SHIFT_XY, (fYA_ * v1.vd.y) + fYB_);
  vtx1.z = (fZA_ * v1.vd.z) + fZB_;
  vtx1.w = v1.vd.w;
  vtx1.c = v1.c;
  vtx1.t = v1.t;

  vtx2.x = fpfromf(SHIFT_XY, (fXA_ * v2.vd.x) + fXB_);
  vtx2.y = fpfromf(SHIFT_XY, (fYA_ * v2.vd.y) + fYB_);
  vtx2.z = (fZA_ * v2.vd.z) + fZB_;
  vtx2.w = v2.vd.w;
  vtx2.c = v2.c;
  vtx2.t = v2.t;

  this->_rasterTriangle(vtx0, vtx1, vtx2);

#ifdef ENABLE_PROFILING
  prof_rasterTotal.end();
#endif
}

//-----------------------------------------------------------------------------
inline void
CRasterizerTile::_rasterTriangle(const SVertexTile & v0, const SVertexTile & v1, const SVertexTile & v2)
{
  if(fragmentIndex_ >= MAX_FRAGMENTS)
  {
    printf("CRasterizerTile::rasterTriangle: Fragment buffer overflow!\n");
    return;
  }

#ifdef ENABLE_PROFILING
  prof_rasterInit.start();
#endif

  STileFragment       & frag = tileFragments_[fragmentIndex_]; fragmentIndex_++;
  STriangleHalfspaces & tri  = frag.tri;
  SBoundingRect         rect;

  // Flat color only
#ifdef COLORED_BLOCK_TEST
  frag.f_flat_c = v2.c;
  //frag.f_flat_c = TColor<GLfloat>(1.0, 1.0, 1.0, 1.0);
#else
  frag.f_flat_c = v2.c;
#endif // COLORED_BLOCK_TEST

  // Fixed point (1.27.4) Vertex coordinates
  // Move (SUB-)pixel center to the top-left corner
  tri.x1 = v0.x - fxPixelCenterOffset_;
  tri.y1 = v0.y - fxPixelCenterOffset_;
  tri.x2 = v1.x - fxPixelCenterOffset_;
  tri.y2 = v1.y - fxPixelCenterOffset_;
  tri.x3 = v2.x - fxPixelCenterOffset_;
  tri.y3 = v2.y - fxPixelCenterOffset_;

  // Fixed point (1.27.4 and 1.23.8) edges
  tri.edge12.create(tri.x1, tri.y1, tri.x2, tri.y2);
  tri.edge23.create(tri.x2, tri.y2, tri.x3, tri.y3);
  tri.edge31.create(tri.x3, tri.y3, tri.x1, tri.y1);

  // Fixed point (1.23.8) edge increments per pixel
  tri.fdx12 = tri.edge12.dx << SHIFT_XY;
  tri.fdx23 = tri.edge23.dx << SHIFT_XY;
  tri.fdx31 = tri.edge31.dx << SHIFT_XY;
  tri.fdy12 = tri.edge12.dy << SHIFT_XY;
  tri.fdy23 = tri.edge23.dy << SHIFT_XY;
  tri.fdy31 = tri.edge31.dy << SHIFT_XY;

  int cx[2], cy[2];
  cx[0] = 0;
  cy[0] = 0;
  cx[1] = tri.fdy12;
  cy[1] = tri.fdx12;
  int32_t acorner_c1 = cx[tri.edge12.dy > 0] + cy[tri.edge12.dx > 0]; // Accept corner
  int32_t rcorner_c1 = tri.fdy12 + tri.fdx12 - acorner_c1;            // Reject corner is pposite to accept corner
  cx[1] = tri.fdy23;
  cy[1] = tri.fdx23;
  int32_t acorner_c2 = cx[tri.edge23.dy > 0] + cy[tri.edge23.dx > 0]; // Accept corner
  int32_t rcorner_c2 = tri.fdy23 + tri.fdx23 - acorner_c2;            // Reject corner is pposite to accept corner
  cx[1] = tri.fdy31;
  cy[1] = tri.fdx31;
  int32_t acorner_c3 = cx[tri.edge31.dy > 0] + cy[tri.edge31.dx > 0]; // Accept corner
  int32_t rcorner_c3 = tri.fdy31 + tri.fdx31 - acorner_c3;            // Reject corner is pposite to accept corner

  CInt32_4 i4ACorner1_offset;
  CInt32_4 i4ACorner2_offset;
  CInt32_4 i4ACorner3_offset;
  CInt32_4 i4RCorner1_offset;
  CInt32_4 i4RCorner2_offset;
  CInt32_4 i4RCorner3_offset;
  // Offset from accept corner to accept corners
  for(int y = 0; y < 2; y++)
  {
    for(int x = 0; x < 2; x++)
    {
      i4ACorner1_offset.e[y*2+x] = (tri.fdy12 * x) + (tri.fdx12 * y) - acorner_c1;
      i4ACorner2_offset.e[y*2+x] = (tri.fdy23 * x) + (tri.fdx23 * y) - acorner_c2;
      i4ACorner3_offset.e[y*2+x] = (tri.fdy31 * x) + (tri.fdx31 * y) - acorner_c3;
    }
  }
  // Offset from accept corner to reject corners
  i4RCorner1_offset = i4ACorner1_offset + (rcorner_c1 - acorner_c1);
  i4RCorner2_offset = i4ACorner2_offset + (rcorner_c2 - acorner_c2);
  i4RCorner3_offset = i4ACorner3_offset + (rcorner_c3 - acorner_c3);

#ifdef LAST_BLOCK_4X4
  // Fixed point (1.23.8) offsets for 4 pixel
  for(int x = 0; x < 4; x++)
  {
    frag.edge12.i4ACorner_offsets[0].e[x] = ((tri.edge12.dy << SHIFT_XY) * x) - (acorner_c1 * 4);
    frag.edge23.i4ACorner_offsets[0].e[x] = ((tri.edge23.dy << SHIFT_XY) * x) - (acorner_c2 * 4);
    frag.edge31.i4ACorner_offsets[0].e[x] = ((tri.edge31.dy << SHIFT_XY) * x) - (acorner_c3 * 4);
  }
  // Fixed point (1.23.8) edge increments per 4x1 pixel
  frag.edge12.i4RCorner_offsets[0] = tri.fdx12;
  frag.edge23.i4RCorner_offsets[0] = tri.fdx23;
  frag.edge31.i4RCorner_offsets[0] = tri.fdx31;

  // Offset[2..x]: 8x8 and bigger blocks use accept/reject corners
  for(int i = 2; i < TILE_SHIFT; i++)
  {
    frag.edge12.i4ACorner_offsets[i] = i4ACorner1_offset << i;
    frag.edge23.i4ACorner_offsets[i] = i4ACorner2_offset << i;
    frag.edge31.i4ACorner_offsets[i] = i4ACorner3_offset << i;
    frag.edge12.i4RCorner_offsets[i] = i4RCorner1_offset << i;
    frag.edge23.i4RCorner_offsets[i] = i4RCorner2_offset << i;
    frag.edge31.i4RCorner_offsets[i] = i4RCorner3_offset << i;
  }
#else
  // Offset[0]: 2x2 blocks use pixel centers
  frag.edge12.i4ACorner_offsets[0] = i4ACorner1_offset - acorner_c1;
  frag.edge23.i4ACorner_offsets[0] = i4ACorner2_offset - acorner_c2;
  frag.edge31.i4ACorner_offsets[0] = i4ACorner3_offset - acorner_c3;
  // Offset[1..x]: 4x4 and bigger blocks use accept/reject corners
  for(int i = 1; i < TILE_SHIFT; i++)
  {
    frag.edge12.i4ACorner_offsets[i] = i4ACorner1_offset << i;
    frag.edge23.i4ACorner_offsets[i] = i4ACorner2_offset << i;
    frag.edge31.i4ACorner_offsets[i] = i4ACorner3_offset << i;
    frag.edge12.i4RCorner_offsets[i] = i4RCorner1_offset << i;
    frag.edge23.i4RCorner_offsets[i] = i4RCorner2_offset << i;
    frag.edge31.i4RCorner_offsets[i] = i4RCorner3_offset << i;
  }
#endif // LAST_BLOCK_4X4

  frag.edge12.pEdge = &frag.tri.edge12;
  frag.edge23.pEdge = &frag.tri.edge23;
  frag.edge31.pEdge = &frag.tri.edge31;

  // Bounding rectangle
  rect.x      = fpfloor(SHIFT_XY, mathlib::min(tri.x1, tri.x2, tri.x3) + (1 << SHIFT_XY) - 1);
  rect.y      = fpfloor(SHIFT_XY, mathlib::min(tri.y1, tri.y2, tri.y3) + (1 << SHIFT_XY) - 1);
  rect.width  = fpfloor(SHIFT_XY, mathlib::max(tri.x1, tri.x2, tri.x3) + (1 << SHIFT_XY) - 1) - rect.x;
  rect.height = fpfloor(SHIFT_XY, mathlib::max(tri.y1, tri.y2, tri.y3) + (1 << SHIFT_XY) - 1) - rect.y;

#ifdef PLACE_IN_TILES

  // Align to tile size
  rect.width  += rect.x &  (TILE_SIZE - 1);
  rect.height += rect.y &  (TILE_SIZE - 1);
  rect.x       = rect.x & ~(TILE_SIZE - 1);
  rect.y       = rect.y & ~(TILE_SIZE - 1);
  rect.width   = (rect.width  + TILE_SIZE - 1) & ~(TILE_SIZE - 1);
  rect.height  = (rect.height + TILE_SIZE - 1) & ~(TILE_SIZE - 1);

  placeInTiles(frag, rect);

  #ifdef ENABLE_PROFILING
  prof_rasterInit.end();
  #endif

#else

  #ifdef ENABLE_PROFILING
  prof_rasterInit.end();
  prof_rasterMain.start();
  #endif

  #ifdef BLOCK_ITERATION
  // Align to tile size
  rect.width  += rect.x &  (TILE_SIZE - 1);
  rect.height += rect.y &  (TILE_SIZE - 1);
  rect.x       = rect.x & ~(TILE_SIZE - 1);
  rect.y       = rect.y & ~(TILE_SIZE - 1);
  rect.width   = (rect.width  + TILE_SIZE - 1) & ~(TILE_SIZE - 1);
  rect.height  = (rect.height + TILE_SIZE - 1) & ~(TILE_SIZE - 1);
  if((rect.width == TILE_SIZE) && (rect.height == TILE_SIZE))
    rasterTriangleDyn<TILE_SHIFT-1>(frag, rect.x, rect.y);
  else
    rasterTriangleBlock(frag, rect);
  #else
  _rasterTrianglePoint(frag, rect);
  #endif

  #ifdef ENABLE_PROFILING
  prof_rasterMain.end();
  #endif

#endif
}

//-----------------------------------------------------------------------------
void
CRasterizerTile::doTile(STile & tile, int x, int y)
{
  if(tile.bin.count() > 0)
  {
    const STileFragment * frag = tile.bin.getList();

    for(int i = 0; i < tile.bin.count(); i++)
    {
      rasterTriangleDyn<TILE_SHIFT-1>(*frag++, x, y);
    }

    tile.bin.reset();
  }
}

//-----------------------------------------------------------------------------
void
CRasterizerTile::placeInTiles(const STileFragment & frag, const SBoundingRect & rect)
{
  const STriangleHalfspaces & tri = frag.tri;
//  STriangleConstants acorner_x, acorner_y;
  STriangleConstants rcorner_x, rcorner_y;
  STriangleConstants corner_x_inc, corner_y_inc;
  int x_end = rect.x + rect.width;
  int y_end = rect.y + rect.height;

  // Precalculated steps
  corner_x_inc.c1 = tri.fdy12 << TILE_SHIFT;
  corner_x_inc.c2 = tri.fdy23 << TILE_SHIFT;
  corner_x_inc.c3 = tri.fdy31 << TILE_SHIFT;
  corner_y_inc.c1 = tri.fdx12 << TILE_SHIFT;
  corner_y_inc.c2 = tri.fdx23 << TILE_SHIFT;
  corner_y_inc.c3 = tri.fdx31 << TILE_SHIFT;

  // Corners of block
  int cx[2], cy[2];
  cx[0] = (rect.x                ) << SHIFT_XY;
  cx[1] = (rect.x + TILE_SIZE - 1) << SHIFT_XY;
  cy[0] = (rect.y                ) << SHIFT_XY;
  cy[1] = (rect.y + TILE_SIZE - 1) << SHIFT_XY;

  // Check two corners depending on the slope of the edge
/*
  // Accept corner
  acorner_y.c1 = tri.edge12.distance(cx[tri.edge12.dy > 0], cy[tri.edge12.dx > 0]);
  acorner_y.c2 = tri.edge23.distance(cx[tri.edge23.dy > 0], cy[tri.edge23.dx > 0]);
  acorner_y.c3 = tri.edge31.distance(cx[tri.edge31.dy > 0], cy[tri.edge31.dx > 0]);
*/
  // Reject corner
  rcorner_y.c1 = tri.edge12.distance(cx[tri.edge12.dy < 0], cy[tri.edge12.dx < 0]);
  rcorner_y.c2 = tri.edge23.distance(cx[tri.edge23.dy < 0], cy[tri.edge23.dx < 0]);
  rcorner_y.c3 = tri.edge31.distance(cx[tri.edge31.dy < 0], cy[tri.edge31.dx < 0]);

  for(int y = rect.y; y < y_end; y += TILE_SIZE)
  {
//    acorner_x = acorner_y;
    rcorner_x = rcorner_y;

    for(int x = rect.x; x < x_end; x += TILE_SIZE)
    {
      // Test reject corner
      if((rcorner_x.c1 < 0) && (rcorner_x.c2 < 0) && (rcorner_x.c3 < 0))
      {
        // Place in tile-bin when totally or partially covered
        int iTileIndex = ((y >> TILE_SHIFT) * tileSet_.tileCountHorizontal) + (x >> TILE_SHIFT);
        tileSet_.tile[iTileIndex].bin.add(&frag);
        //rasterTriangleDyn<TILE_SHIFT-1>(frag, x, y);
/*
        // Test accept corner
        if((acorner_x.c1 < 0) && (acorner_x.c2 < 0) && (acorner_x.c3 < 0))
        {
          // Totally covered -> draw
          int iTileIndex = (y >> TILE_SHIFT) * tileSet_.tileCountHorizontal + (x >> TILE_SHIFT);
          tileSet_.tile[iTileIndex].bin.add(&frag);
        }
        else
        {
          // Recurse to smaller block size
          int iTileIndex = (y >> TILE_SHIFT) * tileSet_.tileCountHorizontal + (x >> TILE_SHIFT);
          tileSet_.tile[iTileIndex].bin.add(&frag);
        }
*/
      }
/*
      acorner_x.c1 += corner_x_inc.c1;
      acorner_x.c2 += corner_x_inc.c2;
      acorner_x.c3 += corner_x_inc.c3;
*/
      rcorner_x.c1 += corner_x_inc.c1;
      rcorner_x.c2 += corner_x_inc.c2;
      rcorner_x.c3 += corner_x_inc.c3;
    }
/*
    acorner_y.c1 += corner_y_inc.c1;
    acorner_y.c2 += corner_y_inc.c2;
    acorner_y.c3 += corner_y_inc.c3;
*/
    rcorner_y.c1 += corner_y_inc.c1;
    rcorner_y.c2 += corner_y_inc.c2;
    rcorner_y.c3 += corner_y_inc.c3;
  }
}

//-----------------------------------------------------------------------------
void
CRasterizerTile::rasterTriangleBlock(const STileFragment & frag, const SBoundingRect & rect)
{
  const STriangleHalfspaces & tri = frag.tri;
  STriangleConstants acorner_x, acorner_y;
  STriangleConstants rcorner_x, rcorner_y;
  STriangleConstants corner_x_inc, corner_y_inc;
  int x_end = rect.x + rect.width;
  int y_end = rect.y + rect.height;

  // Precalculated steps
  corner_x_inc.c1 = tri.fdy12 << TILE_SHIFT;
  corner_x_inc.c2 = tri.fdy23 << TILE_SHIFT;
  corner_x_inc.c3 = tri.fdy31 << TILE_SHIFT;
  corner_y_inc.c1 = tri.fdx12 << TILE_SHIFT;
  corner_y_inc.c2 = tri.fdx23 << TILE_SHIFT;
  corner_y_inc.c3 = tri.fdx31 << TILE_SHIFT;

  // Corners of block
  int cx[2], cy[2];
  cx[0] = (rect.x            ) << SHIFT_XY;
  cx[1] = (rect.x + TILE_SIZE) << SHIFT_XY;
  cy[0] = (rect.y            ) << SHIFT_XY;
  cy[1] = (rect.y + TILE_SIZE) << SHIFT_XY;

  // Check two corners depending on the slope of the edge
  // Accept corner
  acorner_y.c1 = tri.edge12.distance(cx[tri.edge12.dy > 0], cy[tri.edge12.dx > 0]);
  acorner_y.c2 = tri.edge23.distance(cx[tri.edge23.dy > 0], cy[tri.edge23.dx > 0]);
  acorner_y.c3 = tri.edge31.distance(cx[tri.edge31.dy > 0], cy[tri.edge31.dx > 0]);
  // Reject corner
  rcorner_y.c1 = tri.edge12.distance(cx[tri.edge12.dy < 0], cy[tri.edge12.dx < 0]);
  rcorner_y.c2 = tri.edge23.distance(cx[tri.edge23.dy < 0], cy[tri.edge23.dx < 0]);
  rcorner_y.c3 = tri.edge31.distance(cx[tri.edge31.dy < 0], cy[tri.edge31.dx < 0]);

  for(int y = rect.y; y < y_end; y += TILE_SIZE)
  {
    acorner_x = acorner_y;
    rcorner_x = rcorner_y;

    for(int x = rect.x; x < x_end; x += TILE_SIZE)
    {
      // Test reject corner
      if((rcorner_x.c1 < 0) && (rcorner_x.c2 < 0) && (rcorner_x.c3 < 0))
      {
        // Test accept corner
        if((acorner_x.c1 < 0) && (acorner_x.c2 < 0) && (acorner_x.c3 < 0))
        {
          // Totally covered -> draw
          drawBlock(frag, x, y, TILE_SIZE);
        }
        else
        {
          // Recurse to smaller block size
          rasterTriangleDyn3<TILE_SHIFT-1>(frag, x, y, frag.edge12, frag.edge23, frag.edge31, acorner_x.c1, acorner_x.c2, acorner_x.c3);
        }
      }
      acorner_x.c1 += corner_x_inc.c1;
      acorner_x.c2 += corner_x_inc.c2;
      acorner_x.c3 += corner_x_inc.c3;

      rcorner_x.c1 += corner_x_inc.c1;
      rcorner_x.c2 += corner_x_inc.c2;
      rcorner_x.c3 += corner_x_inc.c3;
    }
    acorner_y.c1 += corner_y_inc.c1;
    acorner_y.c2 += corner_y_inc.c2;
    acorner_y.c3 += corner_y_inc.c3;

    rcorner_y.c1 += corner_y_inc.c1;
    rcorner_y.c2 += corner_y_inc.c2;
    rcorner_y.c3 += corner_y_inc.c3;
  }
}

//-----------------------------------------------------------------------------
void
CRasterizerTile::rasterTrianglePoint(const STileFragment & frag, const SBoundingRect & rect)
{
  const STriangleHalfspaces & tri = frag.tri;
  STriangleConstants corner_x, corner_y;
  int x_end = rect.x + rect.width;
  int y_end = rect.y + rect.height;

  corner_y.c1 = tri.edge12.distance((rect.x << SHIFT_XY), (rect.y << SHIFT_XY));
  corner_y.c2 = tri.edge23.distance((rect.x << SHIFT_XY), (rect.y << SHIFT_XY));
  corner_y.c3 = tri.edge31.distance((rect.x << SHIFT_XY), (rect.y << SHIFT_XY));

  for(int y = rect.y; y < y_end; y += 1)
  {
    corner_x = corner_y;

    for(int x = rect.x; x < x_end; x += 1)
    {
      if((corner_x.c1 < 0) && (corner_x.c2 < 0) && (corner_x.c3 < 0))
      {
        // Accept point
        drawPoint(frag, x, y);
      }
      corner_x.c1 += tri.fdy12;
      corner_x.c2 += tri.fdy23;
      corner_x.c3 += tri.fdy31;
    }
    corner_y.c1 += tri.fdx12;
    corner_y.c2 += tri.fdx23;
    corner_y.c3 += tri.fdx31;
  }
}

//-----------------------------------------------------------------------------
template<const int blockShift>
INLINE_FUNC void
CRasterizerTile::rasterTriangleDyn(const STileFragment & frag, const int & x, const int & y)
{
  const STriangleHalfspaces & tri = frag.tri;

  // Corners of block
  int cx[2], cy[2];
  cx[0] = (x                          ) << SHIFT_XY;
  cx[1] = (x + (1 << (blockShift + 1))) << SHIFT_XY;
  cy[0] = (y                          ) << SHIFT_XY;
  cy[1] = (y + (1 << (blockShift + 1))) << SHIFT_XY;

  // Accept corner
  int ic1 = tri.edge12.distance(cx[tri.edge12.dy > 0], cy[tri.edge12.dx > 0]);
  int ic2 = tri.edge23.distance(cx[tri.edge23.dy > 0], cy[tri.edge23.dx > 0]);
  int ic3 = tri.edge31.distance(cx[tri.edge31.dy > 0], cy[tri.edge31.dx > 0]);

  rasterTriangleDyn3<blockShift>(frag, x, y, frag.edge12, frag.edge23, frag.edge31, ic1, ic2, ic3);
}

//-----------------------------------------------------------------------------
template<const int blockShift>
INLINE_FUNC void
CRasterizerTile::rasterTriangleDyn3(const STileFragment & frag, const int & x, const int & y, const SEdgeWorkSet & c1, const SEdgeWorkSet & c2, const SEdgeWorkSet & c3, const int32_t & ic1, const int32_t &  ic2, const int32_t &  ic3)
{
  CInt32_4 i4Accept, i4Reject;

  // Get distance at accept corner
  CInt32_4 i4ACorner1 = ic1;
  CInt32_4 i4ACorner2 = ic2;
  CInt32_4 i4ACorner3 = ic3;
  CInt32_4 i4RCorner1 = i4ACorner1;
  CInt32_4 i4RCorner2 = i4ACorner2;
  CInt32_4 i4RCorner3 = i4ACorner3;
  // Move to accept corners
  i4ACorner1 += c1.i4ACorner_offsets[blockShift];
  i4ACorner2 += c2.i4ACorner_offsets[blockShift];
  i4ACorner3 += c3.i4ACorner_offsets[blockShift];
  // Move to reject corners
  i4RCorner1 += c1.i4RCorner_offsets[blockShift];
  i4RCorner2 += c2.i4RCorner_offsets[blockShift];
  i4RCorner3 += c3.i4RCorner_offsets[blockShift];

  // Create accept flags
  i4Accept  = (i4ACorner1 < i4Zero) & i4Bit0;
  i4Accept += (i4ACorner2 < i4Zero) & i4Bit1;
  i4Accept += (i4ACorner3 < i4Zero) & i4Bit2;
  // Create reject flags
  i4Reject  = (i4RCorner1 < i4Zero) & i4Bit0;
  i4Reject += (i4RCorner2 < i4Zero) & i4Bit1;
  i4Reject += (i4RCorner3 < i4Zero) & i4Bit2;

  int ie = 0;
  for(int iy = 0; iy < 2; iy++)
  {
    for(int ix = 0; ix < 2; ix++)
    {
      if(i4Reject.e[ie] == 0x00000007)
      {
        int x2 = x + (ix << blockShift);
        int y2 = y + (iy << blockShift);
        if(i4Accept.e[ie] == 0x00000007)
        {
          drawBlock(frag, x2, y2, (1 << blockShift));
        }
        else
        {
          rasterTriangleDyn3<blockShift-1>(frag, x2, y2, c1, c2, c3, i4ACorner1.e[ie], i4ACorner2.e[ie], i4ACorner3.e[ie]);
        }
      }
      ie++;
    }
  }
}

#ifdef LAST_BLOCK_4X4
//-----------------------------------------------------------------------------
// Specialized template to terminate the recursion
template<>
INLINE_FUNC void
CRasterizerTile::rasterTriangleDyn3<1>(const STileFragment & frag, const int & x, const int & y, const SEdgeWorkSet & c1, const SEdgeWorkSet & c2, const SEdgeWorkSet & c3, const int32_t &  ic1, const int32_t &  ic2, const int32_t &  ic3)
{
  char * colorBuffer = (char *)renderSurface->p;
  colorBuffer += (renderSurface->width() * y + x) * 4;
  int skip = (renderSurface->width()) * 4;

#ifdef ENABLE_PROFILING_FILLRATE
  fillRate += 4*4;
#endif

#ifdef COLORED_BLOCK_TEST
  CInt32_4 i4Color(CREATE_COLOR(frag.f_flat_c * CFloat_4(0.7, 0.7, 0.7, 1.0)));
#else
  CInt32_4 i4Color(CREATE_COLOR(frag.f_flat_c));
#endif // COLORED_BLOCK_TEST

  CInt32_4 i4Mask;

  // Get distance at accept corner
  CInt32_4 i4PCenter1_4x1 = ic1;
  CInt32_4 i4PCenter2_4x1 = ic2;
  CInt32_4 i4PCenter3_4x1 = ic3;
  // Move to pixel centers
  i4PCenter1_4x1 += c1.i4ACorner_offsets[0];
  i4PCenter2_4x1 += c2.i4ACorner_offsets[0];
  i4PCenter3_4x1 += c3.i4ACorner_offsets[0];

  for(int i = 0; i < 4; i++)
  {
    i4Mask = (i4PCenter1_4x1 < i4Zero) & (i4PCenter2_4x1 < i4Zero) & (i4PCenter3_4x1 < i4Zero);
    _mm_maskmoveu_si128(i4Color.xmm, i4Mask.xmm, colorBuffer);
    i4PCenter1_4x1 += c1.i4RCorner_offsets[0];
    i4PCenter2_4x1 += c2.i4RCorner_offsets[0];
    i4PCenter3_4x1 += c3.i4RCorner_offsets[0];
    colorBuffer += skip;
  }
}
#else
//-----------------------------------------------------------------------------
// Specialized template to terminate the recursion
template<>
INLINE_FUNC void
CRasterizerTile::rasterTriangleDyn3<0>(const STileFragment & frag, const int & x, const int & y, const SEdgeWorkSet & c1, const SEdgeWorkSet & c2, const SEdgeWorkSet & c3, const int32_t &  ic1, const int32_t &  ic2, const int32_t &  ic3)
{
  CInt32_4 i4Accept;

  // Get distance at accept corner
  CInt32_4 i4ACorner1 = ic1;
  CInt32_4 i4ACorner2 = ic2;
  CInt32_4 i4ACorner3 = ic3;
  // Move to accept corners
  i4ACorner1 += c1.i4ACorner_offsets[0];
  i4ACorner2 += c2.i4ACorner_offsets[0];
  i4ACorner3 += c3.i4ACorner_offsets[0];
  // Create accept flags
  i4Accept = (i4ACorner1 < i4Zero) & (i4ACorner2 < i4Zero) & (i4ACorner3 < i4Zero);

  // Draw
  draw2x2WithMask(frag, x, y, i4Accept);
}
#endif // LAST_BLOCK_4X4

//-----------------------------------------------------------------------------
INLINE_FUNC void
CRasterizerTile::draw2x2WithMask(const STileFragment & frag, const int & x, const int & y, const CInt32_4 & mask)
{
  uint32_t * colorBuffer = (uint32_t *)renderSurface->p;
  colorBuffer += (renderSurface->width() * y + x);

#ifdef ENABLE_PROFILING_FILLRATE
  fillRate += 4;
#endif

#ifdef COLORED_BLOCK_TEST
  uint32_t iColor(CREATE_COLOR(frag.f_flat_c * CFloat_4(0.7, 0.7, 0.7, 1.0)));
#else
  uint32_t iColor(CREATE_COLOR(frag.f_flat_c));
#endif // COLORED_BLOCK_TEST

  // Write
  if(mask.e[0]) colorBuffer[0] = iColor;
  if(mask.e[1]) colorBuffer[1] = iColor;
  colorBuffer += renderSurface->width();
  if(mask.e[2]) colorBuffer[0] = iColor;
  if(mask.e[3]) colorBuffer[1] = iColor;
}

//-----------------------------------------------------------------------------
void
CRasterizerTile::drawBlock(const STileFragment & frag, int x, int y, int blockSize)
{
  uint32_t * colorBuffer = (uint32_t *)renderSurface->p;
  colorBuffer += (renderSurface->width() * y + x);
  int skip = (renderSurface->width() - blockSize);
  uint32_t color;

#ifdef ENABLE_PROFILING_FILLRATE
  fillRate += blockSize * blockSize;
#endif

#ifdef COLORED_BLOCK_TEST
  CFloat_4 colorFilter;
  if     (blockSize >= 64) colorFilter = CFloat_4(1.0, 1.0, 1.0, 1.0);
  else if(blockSize == 32) colorFilter = CFloat_4(1.0, 1.0, 1.0, 1.0);
  else if(blockSize == 16) colorFilter = CFloat_4(1.0, 1.0, 1.0, 1.0);
  else if(blockSize ==  8) colorFilter = CFloat_4(0.9, 0.9, 0.9, 1.0);
  else if(blockSize ==  4) colorFilter = CFloat_4(0.8, 0.8, 0.8, 1.0);
  else if(blockSize ==  2) colorFilter = CFloat_4(0.7, 0.7, 0.7, 1.0);
  else                     colorFilter = CFloat_4(0.6, 0.6, 0.6, 1.0);
  color = CREATE_COLOR(frag.f_flat_c * colorFilter);
#else
  color = CREATE_COLOR(frag.f_flat_c);
#endif

  for(int iy = 0; iy < blockSize; iy++)
  {
    for(int ix = 0; ix < blockSize; ix++)
    {
      *colorBuffer++ = color;
    }
    colorBuffer += skip;
  }
}

//-----------------------------------------------------------------------------
void
CRasterizerTile::drawPoint(const STileFragment & frag, int x, int y)
{
  uint32_t * colorBuffer = (uint32_t *)renderSurface->p;
  colorBuffer += (renderSurface->width() * y + x);
  uint32_t color;

#ifdef ENABLE_PROFILING_FILLRATE
  fillRate++;
#endif

#ifdef COLORED_BLOCK_TEST
  // Red Bright
  color = CREATE_COLOR(frag.f_flat_c * CFloat_4(0.6, 0.6, 0.6, 1.0));
#else
  color = CREATE_COLOR(frag.f_flat_c);
#endif

  *colorBuffer = color;
}


};
