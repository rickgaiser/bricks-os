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


#include "2dRenderer.h"


//---------------------------------------------------------------------------
// Clip point to surface
#define CLIP_POINT_SURF(x,y) \
{ \
  if(x < 0) x = 0; \
  else if((unsigned int)x >= pSurface_->mode.width)  x = pSurface_->mode.width  - 1; \
  if(y < 0) y = 0; \
  else if((unsigned int)y >= pSurface_->mode.height) y = pSurface_->mode.height - 1; \
}


//---------------------------------------------------------------------------
CPS22DRenderer::CPS22DRenderer(CSurface * surface)
 : CAPS2Renderer(surface)
{
}

//---------------------------------------------------------------------------
CPS22DRenderer::~CPS22DRenderer()
{
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setSurface(CSurface * surface)
{
  CAPS2Renderer::setSurface(surface);

  if(pSurface_ != NULL)
  {
    // (Re)set clipper
    clipper_.xmin = 0;
    clipper_.ymin = 0;
    clipper_.xmax = pSurface_->mode.width  - 1;
    clipper_.ymax = pSurface_->mode.height - 1;
  }
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setColor(color_t rgb)
{
  colorR_ = BxColorFormat_GetR(cfA8R8G8B8, rgb);
  colorG_ = BxColorFormat_GetG(cfA8R8G8B8, rgb);
  colorB_ = BxColorFormat_GetB(cfA8R8G8B8, rgb);

  colorPS2_ = GIF::REG::RGBAQ(colorR_, colorG_, colorB_, 0x80, 0);
  packet_.gifAddPackedAD(GIF::REG::rgbaq, colorPS2_);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  colorR_ = r;
  colorG_ = g;
  colorB_ = b;

  colorPS2_ = GIF::REG::RGBAQ(colorR_, colorG_, colorB_, 0x80, 0);
  packet_.gifAddPackedAD(GIF::REG::rgbaq, colorPS2_);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setPixel(int x, int y)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(GS_PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x<<4, y<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::fill()
{
  fillRect(clipper_.xmin, clipper_.ymin, clipper_.xmax - clipper_.xmin + 1, clipper_.ymax - clipper_.ymin + 1);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::fillRect(int x, int y, unsigned int width, unsigned int height)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(GS_PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((x         << 4), (y          <<4), 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(((x+width) << 4), ((y+height) <<4), 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
// FIXME: The last pixel is never drawn, so we need to make the line 1px longer
void
CPS22DRenderer::drawLine(int x1, int y1, int x2, int y2)
{
  x1 += GS_X_BASE;
  y1 += GS_Y_BASE;
  x2 += GS_X_BASE;
  y2 += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(GS_PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x1<<4, y1<<4, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x2<<4, y2<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawHLine(int x, int y, unsigned int width)
{
  drawLine(x, y, x + width - 1, y);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawVLine(int x, int y, unsigned int height)
{
  drawLine(x, y, x, y + height - 1);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(GS_PRIM_LINE_STRIP, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((x           << 4), (y            << 4), 0)); // Top-Left
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(((x+width-1) << 4), (y            << 4), 0)); // Top-Right
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(((x+width-1) << 4), ((y+height-1) << 4), 0)); // Bottom-Right
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((x           << 4), ((y+height-1) << 4), 0)); // Bottom-Left
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((x           << 4), (y            << 4), 0)); // Top-Left
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setClipRect(int x, int y, unsigned int width, unsigned int height)
{
  int x2 = x + width  - 1;
  int y2 = y + height - 1;

  // Clip to surface
  CLIP_POINT_SURF(x,  y);
  CLIP_POINT_SURF(x2, y2);

  clipper_.xmin = x;
  clipper_.ymin = y;
  clipper_.xmax = x2;
  clipper_.ymax = y2;

  packet_.gifAddPackedAD(GIF::REG::scissor_1, GIF::REG::SCISSOR(clipper_.xmin, clipper_.xmax, clipper_.ymin, clipper_.ymax));
}
