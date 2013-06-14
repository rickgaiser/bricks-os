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


#include "glProfiling.h"


#ifdef ENABLE_PROFILING


CTimeProfiler prof_renderTotal;     // Renderer
CTimeProfiler prof_renderTransform; //  |- Vertex transformation
CTimeProfiler prof_renderLight;     //  |- Lighting
//CTimeProfiler prof_renderClip;      //  |- Clipping

CTimeProfiler prof_rasterTotal;     // Rasterizer
CTimeProfiler prof_rasterClear;     //  |- Clearing the buffers (color/depth)
CTimeProfiler prof_rasterInit;      //  |- Initialization of raster buffers (tile-based only?)
CTimeProfiler prof_rasterMain;      //  |- Main rasterization process
CTimeProfiler prof_rasterCopy;      //  |- Copying of render buffer to back buffer (tile-based only?)

#ifdef ENABLE_PROFILING_FILLRATE
uint64_t fillRate;                  // Number of pixels rastered per frame
#endif // ENABLE_PROFILING_FILLRATE


#endif // ENABLE_PROFILING
