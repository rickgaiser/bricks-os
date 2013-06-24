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


#ifndef GL_RASTERTILECOMMON_H
#define GL_RASTERTILECOMMON_H


#include "raster.h"
#include "rasterCommon.h"
#include "vhl/vhl.h"

//#include "pthread.h"
#include "stdio.h"


#define TILE_SHIFT            6               // 1<<6 = 64
#define TILE_SIZE             (1<<TILE_SHIFT) // 64


namespace raster
{


//-----------------------------------------------------------------------------
// Edge Equation Bx + Cy:
//   B = DeltaY
//   C = -DeltaX
//   Bx + Cy == 0   --> exactly on the edge
//   Bx + Cy >  0   --> outside of triangle
//   Bx + Cy <  0   --> inside  of triangle
class CEdge
{
public:
  CEdge(){}

  inline void create(int x0, int y0, int x1, int y1)
  {
    dy = y1 - y0; // B
    dx = x0 - x1; // C
    // Use the first point as the base constant
    c  = 0;
    c  = distance(x0, y0);

    // Correct for fill convention:
    // We want the Top-Left edges to be drawn, using "distance < 0"
    //  - Top:  dy == 0 && dx > 0
    //  - Left: dy > 0
    if(dy > 0 || (dy == 0 && dx > 0))
      c++;
  }
  inline int distance(int x, int y) const
  {
    // NOTE_1: Distance is only valid if normalized.
    //         Since we only need the sign bit, we don't care.
    // NOTE_2: 2x Fixed Point format
    return (dy * x) + (dx * y) - c;
  }

//private:
public:
  int dy;
  int dx;
  int c;
};

//-----------------------------------------------------------------------------
struct STriangleConstants
{
/*
  inline void operator+=(const STriangleConstants & tc){
    c1 += tc.c1;
    c2 += tc.c2;
    c3 += tc.c3;
  }
  inline void operator-=(const STriangleConstants & tc){
    c1 -= tc.c1;
    c2 -= tc.c2;
    c3 -= tc.c3;
  }
*/
  int c1;
  int c2;
  int c3;
};

//-----------------------------------------------------------------------------
struct STriangleHalfspaces
{
  // Vertex coordinates (1.27.4)
  int x1, y1;
  int x2, y2;
  int x3, y3;

  // Edges
  CEdge edge12;
  CEdge edge23;
  CEdge edge31;

  // Fixed-point edges (1.23.8)
  int fdx12, fdy12;
  int fdx23, fdy23;
  int fdx31, fdy31;
};

//-----------------------------------------------------------------------------
struct SBoundingRect
{
  // Position on screen
  int x, y;
  // Size
  int width, height;
};

//-----------------------------------------------------------------------------
struct SEdgeWorkSet
{
  CInt32_4 i4ACorner_offsets[TILE_SHIFT];
  CInt32_4 i4RCorner_offsets[TILE_SHIFT];
  CEdge * pEdge;
};

//-----------------------------------------------------------------------------
#define MAX_FRAGMENTS (256 * 1024)
struct STileFragment
{
  STriangleHalfspaces tri;
  CFloat_4 f_flat_c;

  SEdgeWorkSet edge12;
  SEdgeWorkSet edge23;
  SEdgeWorkSet edge31;

  // 0:  2x2  blocks, offsets to 4  1x1  blocks
  // 1:  4x4  blocks, offsets to 4  2x2  blocks
  // 2:  8x8  blocks, offsets to 4  4x4  blocks
  // 3: 16x16 blocks, offsets to 4  8x8  blocks
  // 4: 32x32 blocks, offsets to 4 16x16 blocks
  // 5: 64x64 blocks, offsets to 4 32x32 blocks
  //CInt32_4 i4ACorner1_offsets[TILE_SHIFT];
  //CInt32_4 i4ACorner2_offsets[TILE_SHIFT];
  //CInt32_4 i4ACorner3_offsets[TILE_SHIFT];
  //CInt32_4 i4RCorner1_offsets[TILE_SHIFT];
  //CInt32_4 i4RCorner2_offsets[TILE_SHIFT];
  //CInt32_4 i4RCorner3_offsets[TILE_SHIFT];
};

//-----------------------------------------------------------------------------
#define MAX_FRAGMENTS_PER_TILE (MAX_FRAGMENTS)
class CTileBin
{
public:
  CTileBin()
   : index_(0)
  {
    frag_ = new const STileFragment*[MAX_FRAGMENTS_PER_TILE];
  }

  ~CTileBin()
  {
    delete[] frag_;
  }

  inline void add(const STileFragment * frag)
  {
    if(index_ < MAX_FRAGMENTS_PER_TILE)
    {
      frag_[index_] = frag;
      index_++;
    }
  }

  inline int count() const
  {
    return index_;
  }

  inline const STileFragment * getList() const
  {
    return *frag_;
  }

  inline void reset()
  {
    index_ = 0;
  }

private:
  const STileFragment ** frag_;
  int index_;
};

//-----------------------------------------------------------------------------
struct STile
{
  CTileBin bin;

  int xpos;
  int ypos;
};

//-----------------------------------------------------------------------------
class CTileSet
{
public:
  CTileSet();
  ~CTileSet();

  void generateTileSet(int width, int height);

public:
  STile * tile;

  int tileCountHorizontal;
  int tileCountVertical;
  int tileCount;

  int frameWidth;
  int frameHeight;
};


};


#endif // GL_RASTERTILECOMMON_H
