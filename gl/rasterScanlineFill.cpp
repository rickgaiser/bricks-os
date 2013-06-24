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

#include "rasterScanlineFill.h"
#include "rasterCompare.h"
#include "CDDA.h"
#ifdef ENABLE_PROFILING
#include "glProfiling.h"
#endif
#include "mathlib.h"
#include "vhl/fixedPoint.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include <cstdlib>


//#define USE_RANDOM_COLOR


namespace raster
{


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CRasterizerScanlineFill::CRasterizerScanlineFill()
 : CASoftRasterizer()
{
}

//-----------------------------------------------------------------------------
CRasterizerScanlineFill::~CRasterizerScanlineFill()
{
}

//-----------------------------------------------------------------------------
void
CRasterizerScanlineFill::rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2)
{
  SVertexScanlineFill vtx0, vtx1, vtx2;

#ifdef ENABLE_PROFILING
  prof_rasterMain.start();
#endif

  vtx0.fx = (fXA_ * v0.vd.x) + fXB_;
  vtx0.x  = fpfromf(SHIFT_XY, vtx0.fx);
  vtx0.fy = (fYA_ * v0.vd.y) + fYB_;
  vtx0.y  = fpfromf(SHIFT_XY, vtx0.fy);
  vtx1.fx = (fXA_ * v1.vd.x) + fXB_;
  vtx1.x  = fpfromf(SHIFT_XY, vtx1.fx);
  vtx1.fy = (fYA_ * v1.vd.y) + fYB_;
  vtx1.y  = fpfromf(SHIFT_XY, vtx1.fy);
  vtx2.fx = (fXA_ * v2.vd.x) + fXB_;
  vtx2.x  = fpfromf(SHIFT_XY, vtx2.fx);
  vtx2.fy = (fYA_ * v2.vd.y) + fYB_;
  vtx2.y  = fpfromf(SHIFT_XY, vtx2.fy);

  // Bubble sort the 3 vertexes
  // NOTE: Top < Bottom
  const SVertexScanlineFill * vbottom(&vtx0);
  const SVertexScanlineFill * vmiddle(&vtx1);
  const SVertexScanlineFill * vtop   (&vtx2);
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

  // Create ARGB color
#ifdef USE_RANDOM_COLOR
  #define fCREATE_COLOR(c) (((uint32_t)(rand() & 0xff) << 16) | ((uint32_t)(rand() & 0xff) << 8) | (uint32_t)(rand() & 0xff))
#else
  #define fCREATE_COLOR(c) (((uint32_t)(c.r * 255) << 16) | ((uint32_t)(c.g * 255) << 8) | (uint32_t)(c.b * 255))
#endif
  uint32_t flat_color = fCREATE_COLOR(v2.c);

  _rasterTriangle(vtop, vmiddle, vbottom, flat_color);

#ifdef ENABLE_PROFILING
  prof_rasterMain.end();
#endif
}

//-----------------------------------------------------------------------------
void
CRasterizerScanlineFill::_rasterTriangle(const SVertexScanlineFill * vtop, const SVertexScanlineFill * vmiddle, const SVertexScanlineFill * vbottom, uint32_t flat_color)
{

  float fdx1  = vmiddle->fx - vtop->fx;
  float fdy1  = vmiddle->fy - vtop->fy;
  float fdx2  = vbottom->fx - vtop->fx;
  float fdy2  = vbottom->fy - vtop->fy;
  float farea = fdx1 * fdy2 - fdx2 * fdy1;
  if(farea == 0.0f)
    return;

  CDDA left_edge;
  CDDA right_edge;

  int32_t iYTop, iYBottom;
  bool bUpdateLeft  = true;
  bool bUpdateRight = true;
  const SVertexScanlineFill *vtop_l = NULL, *vbottom_l = NULL, *vtop_r = NULL, *vbottom_r = NULL;
  for(int i(0); i < 2; i++)
  {
    if(i == 0)
    {
      if(farea > 0.0f)
      {
        vtop_l    = vtop;
        vbottom_l = vbottom;
        vtop_r    = vtop;
        vbottom_r = vmiddle;
      }
      else
      {
        vtop_l    = vtop;
        vbottom_l = vmiddle;
        vtop_r    = vtop;
        vbottom_r = vbottom;
      }
      // height = bottom - top;
      iYTop    = fpfloor(SHIFT_XY, vtop->y    + fxPixelFloorOffset_);
      iYBottom = fpfloor(SHIFT_XY, vmiddle->y + fxPixelFloorOffset_);
    }
    else
    {
      if(farea > 0.0f)
      {
        bUpdateRight = true;
        vtop_r    = vmiddle;
        vbottom_r = vbottom;
      }
      else
      {
        bUpdateLeft  = true;
        vtop_l    = vmiddle;
        vbottom_l = vbottom;
      }
      // height = bottom - top;
      iYTop    = iYBottom;
      iYBottom = fpfloor(SHIFT_XY, vbottom->y + fxPixelFloorOffset_);
    }

    // Need to draw at least one line
    if((iYBottom - iYTop) <= 0)
      continue;

    if(bUpdateLeft == true){
      bUpdateLeft = false;
      left_edge.init(vtop_l->x, vtop_l->y, vbottom_l->x, vbottom_l->y);
    }

    if(bUpdateRight == true){
      bUpdateRight = false;
      right_edge.init(vtop_r->x, vtop_r->y, vbottom_r->x, vbottom_r->y);
    }

    for(int32_t iY = iYTop; iY < iYBottom; iY++)
    {
      int32_t iLineWidth = right_edge.x() - left_edge.x();

      if(iLineWidth > 0)
      {
#ifdef ENABLE_PROFILING_FILLRATE
        fillRate += iLineWidth;
#endif
        uint32_t pixelIndex = iY * renderSurface->width() + left_edge.x();
        uint32_t *pDestPixel = ((uint32_t *)renderSurface->p) + pixelIndex;

        while(iLineWidth--)
        {
          //testAndSetPixel(*pDestPixel++);
          *pDestPixel++ = flat_color;
        }
      }

      left_edge.advance();
      right_edge.advance();
    }
  }
}


};
