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

#include "rasterTileCommon.h"


namespace raster
{


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CTileSet::CTileSet()
 : tile(NULL)
{
}

//-----------------------------------------------------------------------------
CTileSet::~CTileSet()
{
  if(tile != NULL)
  {
    delete[] tile;
    tile = NULL;
  }
}

//-----------------------------------------------------------------------------
void
CTileSet::generateTileSet(int width, int height)
{
  if(tile != NULL)
  {
    if((frameWidth == width) && (frameHeight == height))
      return;

    delete[] tile;
    tile = NULL;
  }

  frameWidth          = width;
  frameHeight         = height;
  tileCountHorizontal = (width  + TILE_SIZE - 1) >> TILE_SHIFT;
  tileCountVertical   = (height + TILE_SIZE - 1) >> TILE_SHIFT;
  tileCount           = tileCountHorizontal * tileCountVertical;
  tile                = new STile[tileCount];

  STile * pTile = tile;
  for(int tile_y = 0; tile_y < tileCountVertical; tile_y++)
  {
    for(int tile_x = 0; tile_x < tileCountHorizontal; tile_x++)
    {
      pTile->xpos = tile_x << TILE_SHIFT;
      pTile->ypos = tile_y << TILE_SHIFT;
      pTile++;
    }
  }
}


};
