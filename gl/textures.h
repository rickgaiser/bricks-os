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


#ifndef TEXTURES_H
#define TEXTURES_H


#include "context.h"
#include "raster.h"
#include "color.h"


#define MAX_TEXTURE_COUNT 1024


//-----------------------------------------------------------------------------
#define TEXTURE_MIPMAP_LEVEL_COUNT 11
// One texture (MipMap) level
struct STextureLevel
{
  void * data;
};

//-----------------------------------------------------------------------------
class CTexture
{
public:
  CTexture();
  virtual ~CTexture();

  void free();
  void bind();

  float lambda(float dudx, float dudy, float dvdx, float dvdy);
  void getTexel(TColor<GLfloat> & c, float u, float v, float lod);
  void getTexel(TColor<int32_t> & c, float u, float v, float lod);

public:
  inline void getTexel(int level, int u, int v, float * channels);

  // Pointers to MipMap levels:
  // Nr Width Tiles
  //  0 1024     32
  //  1  512     16
  //  2  256      8
  //  3  128      4
  //  4   64      2
  //  5   32      1
  //  6   16
  //  7    8
  //  8    4
  //  9    2
  // 10    1
  STextureLevel level[TEXTURE_MIPMAP_LEVEL_COUNT];
  int iMaxLevel_;

  int32_t width;
  int32_t height;
  int32_t bitWidth_;
  int32_t bitHeight_;
  bool    bRGBA_;

  GLint minFilter;
  GLint magFilter;
  GLint uWrapMode;
  GLint vWrapMode;

  uint32_t iWidthMask_;
  uint32_t iHeightMask_;

  void * data;
  void * data_raw;
};

//-----------------------------------------------------------------------------
int convertImageFormat(void * dst, EColorFormat dstFmt, const void * src, EColorFormat srcFmt, int width, int height);
EColorFormat convertGLToBxColorFormat(GLenum format, GLenum type);
uint8_t getBitNr(uint32_t value);


#endif
