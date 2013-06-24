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

#include "rasterTileFill.h"
#ifdef ENABLE_PROFILING
#include "glProfiling.h"
#endif
#include "mathlib.h"


namespace raster
{


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CRasterizerTileFill::CRasterizerTileFill()
 : CASoftRasterizer()
{
}

//-----------------------------------------------------------------------------
CRasterizerTileFill::~CRasterizerTileFill()
{
}

//-----------------------------------------------------------------------------
void
CRasterizerTileFill::rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2)
{
  SVertexTileFill vtx0, vtx1, vtx2;

#ifdef ENABLE_PROFILING
  prof_rasterTotal.start();
#endif

  vtx0.x   = fpfromf(SHIFT_XY, (fXA_ * v0.vd.x) + fXB_);
  vtx0.y   = fpfromf(SHIFT_XY, (fYA_ * v0.vd.y) + fYB_);
  vtx0.z   = (fZA_ * v0.vd.z) + fZB_;
  vtx0.w   = v0.vd.w;
  vtx0.c.r = v0.c.r;
  vtx0.c.g = v0.c.g;
  vtx0.c.b = v0.c.b;
  vtx0.c.a = v0.c.a;

  vtx1.x   = fpfromf(SHIFT_XY, (fXA_ * v1.vd.x) + fXB_);
  vtx1.y   = fpfromf(SHIFT_XY, (fYA_ * v1.vd.y) + fYB_);
  vtx1.z   = (fZA_ * v1.vd.z) + fZB_;
  vtx1.w   = v1.vd.w;
  vtx1.c.r = v1.c.r;
  vtx1.c.g = v1.c.g;
  vtx1.c.b = v1.c.b;
  vtx1.c.a = v1.c.a;

  vtx2.x   = fpfromf(SHIFT_XY, (fXA_ * v2.vd.x) + fXB_);
  vtx2.y   = fpfromf(SHIFT_XY, (fYA_ * v2.vd.y) + fYB_);
  vtx2.z   = (fZA_ * v2.vd.z) + fZB_;
  vtx2.w   = v2.vd.w;
  vtx2.c.r = v2.c.r;
  vtx2.c.g = v2.c.g;
  vtx2.c.b = v2.c.b;
  vtx2.c.a = v2.c.a;

  this->_rasterTriangle(vtx0, vtx1, vtx2);

#ifdef ENABLE_PROFILING
  prof_rasterTotal.end();
#endif
}

//-----------------------------------------------------------------------------
void
CRasterizerTileFill::_rasterTriangle(const SVertexTileFill & v0, const SVertexTileFill & v1, const SVertexTileFill & v2)
{
#ifdef ENABLE_PROFILING
  prof_rasterInit.start();
#endif

  STileFragment         frag;
  STriangleHalfspaces & tri = frag.tri;
  SBoundingRect         rect;

  // Flat color only
  frag.f_flat_c = v2.c;

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

  // Bounding rectangle
  rect.x      = fpfloor(SHIFT_XY, mathlib::min(tri.x1, tri.x2, tri.x3) + (1 << SHIFT_XY) - 1);
  rect.y      = fpfloor(SHIFT_XY, mathlib::min(tri.y1, tri.y2, tri.y3) + (1 << SHIFT_XY) - 1);
  rect.width  = fpfloor(SHIFT_XY, mathlib::max(tri.x1, tri.x2, tri.x3) + (1 << SHIFT_XY) - 1) - rect.x;
  rect.height = fpfloor(SHIFT_XY, mathlib::max(tri.y1, tri.y2, tri.y3) + (1 << SHIFT_XY) - 1) - rect.y;

#ifdef ENABLE_PROFILING
  prof_rasterInit.end();
  prof_rasterMain.start();
#endif
  _rasterTrianglePoint(frag, rect);
#ifdef ENABLE_PROFILING
  prof_rasterMain.end();
#endif
}

//-----------------------------------------------------------------------------
void
CRasterizerTileFill::_rasterTrianglePoint(const STileFragment & frag, const SBoundingRect & rect)
{
  const STriangleHalfspaces & tri = frag.tri;
  STriangleConstants corner_x, corner_y;

  unsigned int * colorBuffer = (unsigned int *)renderSurface->p;
  int stride = renderSurface->width();
  uint32_t color = CREATE_COLOR(frag.f_flat_c);

  colorBuffer += rect.y * stride;

  corner_y.c1 = tri.edge12.distance((rect.x << SHIFT_XY), (rect.y << SHIFT_XY));
  corner_y.c2 = tri.edge23.distance((rect.x << SHIFT_XY), (rect.y << SHIFT_XY));
  corner_y.c3 = tri.edge31.distance((rect.x << SHIFT_XY), (rect.y << SHIFT_XY));

  for(int y = rect.y; y < (rect.y + rect.height); y += 1)
  {
    corner_x = corner_y;

    for(int x = rect.x; x < (rect.x + rect.width); x += 1)
    {
      if((corner_x.c1 < 0) && (corner_x.c2 < 0) && (corner_x.c3 < 0))
      {
        // Accept point
        colorBuffer[x] = color;

#ifdef ENABLE_PROFILING_FILLRATE
        fillRate++;
#endif
      }
      corner_x.c1 += tri.fdy12;
      corner_x.c2 += tri.fdy23;
      corner_x.c3 += tri.fdy31;
    }
    corner_y.c1 -= tri.fdx12;
    corner_y.c2 -= tri.fdx23;
    corner_y.c3 -= tri.fdx31;

    colorBuffer += stride;
  }
}


};
