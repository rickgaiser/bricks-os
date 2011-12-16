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


#include "textures.h"
#include "GL/gl.h"
#ifndef __BRICKS__
#include "GL/glext.h"
#endif
#include "mathlib.h"
#include "unistd.h"
#include "string.h"


//-----------------------------------------------------------------------------
#define UV_TO_INDEX_WRAP(U,V) ((((V) & iHeightMask_) * width) + ((U) & iWidthMask_))

#define TEX_FRACTIONAL_MASK ((1<<SHIFT_TEX)-1)

// Convert tex_coord format to color format
#if SHIFT_TEX > SHIFT_COLOR
  #define TEXCOORD_TO_COLOR(c) ((c) >> (SHIFT_TEX - SHIFT_COLOR))
#elif SHIFT_TEX < SHIFT_COLOR
  #define TEXCOORD_TO_COLOR(c) ((c) << (SHIFT_COLOR - SHIFT_TEX))
#else
  #define TEXCOORD_TO_COLOR(c) ((c))
#endif


//-----------------------------------------------------------------------------
#define IMG_CONV_32_TO_32 0x44
#define IMG_CONV_32_TO_24 0x43
#define IMG_CONV_32_TO_16 0x42
#define IMG_CONV_32_TO_08 0x41
#define IMG_CONV_24_TO_32 0x34
#define IMG_CONV_24_TO_24 0x33
#define IMG_CONV_24_TO_16 0x32
#define IMG_CONV_24_TO_08 0x31
#define IMG_CONV_16_TO_32 0x24
#define IMG_CONV_16_TO_24 0x23
#define IMG_CONV_16_TO_16 0x22
#define IMG_CONV_16_TO_08 0x21
#define IMG_CONV_08_TO_32 0x14
#define IMG_CONV_08_TO_24 0x13
#define IMG_CONV_08_TO_16 0x12
#define IMG_CONV_08_TO_08 0x11
//-----------------------------------------------------------------------------
int
convertImageFormat(void * dst, EColorFormat dstFmt, const void * src, EColorFormat srcFmt, int width, int height)
{
  uint32_t iPixelCount = width * height;

  // Copy/Convert to texture buffer
  if(dstFmt != srcFmt)
  {
    SColorFormatConverter sConverter;
    uint32_t i = 0;
    uint8_t iConv =
      ((colorFormatOps[srcFmt].bitsPerPixel >> 3) << 4) |
      ((colorFormatOps[dstFmt].bitsPerPixel >> 3)     );

    BxColorFormat_CreateConverter(sConverter, srcFmt, dstFmt);

    switch(iConv)
    {
      case IMG_CONV_32_TO_32:
        for(; i < iPixelCount; i++)
          ((uint32_t *)dst)[i] = BxColorFormat_ConvertRGBA(sConverter, ((uint32_t *)src)[i]);
        break;
      case IMG_CONV_32_TO_16:
        for(; i < iPixelCount; i++)
          ((uint16_t *)dst)[i] = BxColorFormat_ConvertRGBA(sConverter, ((uint32_t *)src)[i]);
        break;
      case IMG_CONV_32_TO_08:
        for(; i < iPixelCount; i++)
          ((uint8_t  *)dst)[i] = BxColorFormat_ConvertRGBA(sConverter, ((uint32_t *)src)[i]);
        break;
      case IMG_CONV_24_TO_32:
        for(; i < iPixelCount; i++)
        {
          uint8_t r = ((uint8_t *)src)[i*3+0];
          uint8_t g = ((uint8_t *)src)[i*3+1];
          uint8_t b = ((uint8_t *)src)[i*3+2];
          ((uint32_t *)dst)[i] = BxColorFormat_FromRGBA(dstFmt, r, g, b, 255);
        }
        break;
	  case IMG_CONV_16_TO_32:
        for(; i < iPixelCount; i++)
          ((uint32_t *)dst)[i] = BxColorFormat_ConvertRGBA(sConverter, ((uint16_t *)src)[i]);
        break;
      case IMG_CONV_16_TO_16:
        for(; i < iPixelCount; i++)
          ((uint16_t *)dst)[i] = BxColorFormat_ConvertRGBA(sConverter, ((uint16_t *)src)[i]);
        break;
      case IMG_CONV_16_TO_08:
        for(; i < iPixelCount; i++)
          ((uint8_t  *)dst)[i] = BxColorFormat_ConvertRGBA(sConverter, ((uint16_t *)src)[i]);
        break;
      case IMG_CONV_08_TO_32:
        for(; i < iPixelCount; i++)
          ((uint32_t *)dst)[i] = BxColorFormat_ConvertRGBA(sConverter, ((uint8_t  *)src)[i]);
        break;
      case IMG_CONV_08_TO_16:
        for(; i < iPixelCount; i++)
          ((uint16_t *)dst)[i] = BxColorFormat_ConvertRGBA(sConverter, ((uint8_t  *)src)[i]);
        break;
      case IMG_CONV_08_TO_08:
        for(; i < iPixelCount; i++)
          ((uint8_t  *)dst)[i] = BxColorFormat_ConvertRGBA(sConverter, ((uint8_t  *)src)[i]);
        break;
    };
  }
  else
  {
    // Just copy
    memcpy(dst, src, iPixelCount * (colorFormatOps[dstFmt].bitsPerPixel >> 3));
  }

  return 0;
}

//-----------------------------------------------------------------------------
EColorFormat
convertGLToBxColorFormat(GLenum format, GLenum type)
{
  EColorFormat fmt;

  // Validate format
  if((format != GL_RGB) && (format != GL_BGR) && (format != GL_RGBA) && (format != GL_BGRA))
    return cfUNKNOWN;

  switch(type)
  {
    case GL_UNSIGNED_BYTE:
    {
      switch(format)
      {
#ifdef __BIG_ENDIAN__
        case GL_RGB:  fmt = cfR8G8B8;   break;
        case GL_BGR:  fmt = cfB8G8R8;   break;
        case GL_RGBA: fmt = cfR8G8B8A8; break;
        case GL_BGRA: fmt = cfB8G8R8A8; break;
#else
        case GL_RGB:  fmt = cfB8G8R8;   break;
        case GL_BGR:  fmt = cfR8G8B8;   break;
        case GL_RGBA: fmt = cfA8B8G8R8; break;
        case GL_BGRA: fmt = cfA8R8G8B8; break;
#endif
        default:      fmt = cfUNKNOWN;
      };
      break;
    }
    case GL_UNSIGNED_SHORT_5_6_5:       fmt = (format == GL_RGB ) ? cfR5G6B5   : cfB5G6R5;   break;
    case GL_UNSIGNED_SHORT_5_6_5_REV:   fmt = (format == GL_RGB ) ? cfB5G6R5   : cfR5G6B5;   break;
    case GL_UNSIGNED_SHORT_4_4_4_4:     fmt = (format == GL_RGBA) ? cfR4G4B4A4 : cfB4G4R4A4; break;
    case GL_UNSIGNED_SHORT_4_4_4_4_REV: fmt = (format == GL_RGBA) ? cfA4B4G4R4 : cfA4R4G4B4; break;
    case GL_UNSIGNED_SHORT_5_5_5_1:     fmt = (format == GL_RGBA) ? cfR5G5B5A1 : cfB5G5R5A1; break;
    case GL_UNSIGNED_SHORT_1_5_5_5_REV: fmt = (format == GL_RGBA) ? cfA1B5G5R5 : cfA1R5G5B5; break;
    default:                            fmt = cfUNKNOWN;
  };

  return fmt;
}

//---------------------------------------------------------------------------
// 0x0001 ->  0
// 0x0002 ->  1
// 0x0004 ->  2
// 0x0008 ->  3
// ...... -> ..
// 0x8000 -> 15
uint8_t
getBitNr(uint32_t value)
{
  uint8_t iBitNr;

  value--;
  for(iBitNr = 0; value > 0; iBitNr++, value >>= 1)
    ;

  return iBitNr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CTexture::CTexture()
 : iMaxLevel_(-1)
 , minFilter(GL_NEAREST_MIPMAP_LINEAR)
 , magFilter(GL_LINEAR)
 , uWrapMode(GL_REPEAT)
 , vWrapMode(GL_REPEAT)
 , data_raw(NULL)
{
}

//-----------------------------------------------------------------------------
CTexture::~CTexture()
{
  this->free();
}

//-----------------------------------------------------------------------------
void
CTexture::free()
{
  if(data_raw != NULL)
  {
    delete (uint8_t *)data_raw;
    data_raw = NULL;
  }
}

//-----------------------------------------------------------------------------
void
CTexture::bind()
{
}

//-----------------------------------------------------------------------------
float
CTexture::lambda(float dudx, float dudy, float dvdx, float dvdy)
{
  float max, rho, lambda;

  dudx = mathlib::abs(dudx);
  dudy = mathlib::abs(dudy);
  max = mathlib::max(dudx, dudy) * width;
  rho = max;

  dvdx = mathlib::abs(dvdx);
  dvdy = mathlib::abs(dvdy);
  max = mathlib::max(dvdx, dvdy) * height;
  rho = mathlib::max(rho, max);

  lambda = mathlib::fast_log2(rho);

  return lambda;
}

//-----------------------------------------------------------------------------
void
CTexture::getTexel(TColor<GLfloat> & c, float u, float v, float lod)
{
  int upos, vpos;
  float colors[4];

  if(iMaxLevel_ < 0)
    return;

  u *= width;
  v *= height;

  upos = (int)u;
  vpos = (int)v;

#if 1
  int iFilter;
  int iMipMapLevel0;
  int iMipMapLevel1;
  float fMipMapBlend = 0.0f; // LERP between level0 and level1
  if(lod < 0.0f)
  {
    // Magnify
    iFilter = magFilter;

    // No MipMapping
    iMipMapLevel0 =
    iMipMapLevel1 = 0;
  }
  else
  {
    // Minify
    iFilter = minFilter;

    if((iFilter == GL_LINEAR_MIPMAP_NEAREST) || (iFilter == GL_LINEAR_MIPMAP_LINEAR))
    {
      // Linear MipMap filtering
      int level = (int)lod;
      fMipMapBlend = lod - level;
      iMipMapLevel0 = mathlib::clamp(level  , 0, iMaxLevel_);
      iMipMapLevel1 = mathlib::clamp(level+1, 0, iMaxLevel_);
    }
    else if((iFilter == GL_NEAREST_MIPMAP_NEAREST) || (iFilter == GL_NEAREST_MIPMAP_LINEAR))
    {
      // Nearest MipMap filtering
      int level = (int)(lod + 0.5f);
      iMipMapLevel0 =
      iMipMapLevel1 = mathlib::clamp(level  , 0, iMaxLevel_);
    }
    else
    {
      // No MipMapping
      iMipMapLevel0 =
      iMipMapLevel1 = 0;
    }
  }

  if(iMipMapLevel0 == iMipMapLevel1)
  {
    // No MipMapping
    if((iFilter == GL_NEAREST) ||
       (iFilter == GL_LINEAR_MIPMAP_NEAREST) ||
       (iFilter == GL_NEAREST_MIPMAP_NEAREST))
    {
      // point sampling (GL_NEAREST)
      getTexel(iMipMapLevel0, upos, vpos, colors);
    }
    else
    {
      // bilinear filtering (GL_LINEAR)
      float texels[4][4];
      getTexel(iMipMapLevel0, upos    , vpos    , texels[0]);
      getTexel(iMipMapLevel0, upos + 1, vpos    , texels[1]);
      getTexel(iMipMapLevel0, upos    , vpos + 1, texels[2]);
      getTexel(iMipMapLevel0, upos + 1, vpos + 1, texels[3]);

      float ufrac = u - upos;
      float vfrac = v - vpos;
      for(int channel(0); channel < 4; channel++)
        colors[channel] = mathlib::lerp_2d(ufrac, vfrac, texels[0][channel], texels[1][channel], texels[2][channel], texels[3][channel]);
    }
  }
  else
  {
    // MipMapping
    if((iFilter == GL_NEAREST) ||
       (iFilter == GL_LINEAR_MIPMAP_NEAREST) ||
       (iFilter == GL_NEAREST_MIPMAP_NEAREST))
    {
      // GL_LINEAR_MIPMAP_NEAREST
      float texels0[4];
      float texels1[4];
      getTexel(iMipMapLevel0, upos, vpos, texels0);
      getTexel(iMipMapLevel1, upos, vpos, texels1);

      for(int channel(0); channel < 4; channel++)
        colors[channel] = mathlib::lerp(fMipMapBlend, texels0[channel], texels1[channel]);
    }
    else
    {
      // Trilinear filtering (GL_NEAREST_MIPMAP_NEAREST)
      float texels0[4][4];
      getTexel(iMipMapLevel0, upos    , vpos    , texels0[0]);
      getTexel(iMipMapLevel0, upos + 1, vpos    , texels0[1]);
      getTexel(iMipMapLevel0, upos    , vpos + 1, texels0[2]);
      getTexel(iMipMapLevel0, upos + 1, vpos + 1, texels0[3]);

      float texels1[4][4];
      getTexel(iMipMapLevel1, upos    , vpos    , texels1[0]);
      getTexel(iMipMapLevel1, upos + 1, vpos    , texels1[1]);
      getTexel(iMipMapLevel1, upos    , vpos + 1, texels1[2]);
      getTexel(iMipMapLevel1, upos + 1, vpos + 1, texels1[3]);

      float ufrac = u - upos;
      float vfrac = v - vpos;
      for(int channel(0); channel < 4; channel++)
      {
        colors[channel] = mathlib::lerp_3d(
          ufrac,
          vfrac,
          fMipMapBlend,
          texels0[0][channel], texels0[1][channel], texels0[2][channel], texels0[3][channel],
          texels1[0][channel], texels1[1][channel], texels1[2][channel], texels1[3][channel]);
      }
    }
  }

#else
  // point sampling (GL_NEAREST)
  getTexel(0, upos, vpos, colors);
#endif

  c.a = colors[0];
  c.r = colors[1];
  c.g = colors[2];
  c.b = colors[3];
}

//-----------------------------------------------------------------------------
void
CTexture::getTexel(TColor<int32_t> & c, float u, float v, float lod)
{
#if 0
  TColor<GLfloat> temp = {0};

  getTexel(temp, u, v, lod);

  // Convert to fixed point
  c.a = fpfromf(14, temp.a);
  c.r = fpfromf(14, temp.r);
  c.g = fpfromf(14, temp.g);
  c.b = fpfromf(14, temp.b);
#else
  int iU, iV;
  uint32_t texel;

  iU = (int)(u * width);
  iV = (int)(v * height);
  // Wrap
  iU &= iWidthMask_;
  iV &= iHeightMask_;

  texel = ((uint32_t *)level[0].data)[(iV * width) + iU];

  // Convert to fixed point
  c.a = ((texel >> 24) & 0xff) * ((1<<14) / 255);
  c.r = ((texel >> 16) & 0xff) * ((1<<14) / 255);
  c.g = ((texel >>  8) & 0xff) * ((1<<14) / 255);
  c.b = ((texel      ) & 0xff) * ((1<<14) / 255);
#endif
}

//-----------------------------------------------------------------------------
void
CTexture::getTexel(int _level, int u, int v, float * channels)
{
  // Wrap
  u &= iWidthMask_;
  v &= iHeightMask_;

  uint32_t texel = ((uint32_t *)level[_level].data)[(v >> _level) * (width >> _level) + (u >> _level)];

  channels[0] = (texel >> 24) & 0xff;
  channels[1] = (texel >> 16) & 0xff;
  channels[2] = (texel >>  8) & 0xff;
  channels[3] = (texel      ) & 0xff;

  channels[0] *= (1.0f / 255.0f);
  channels[1] *= (1.0f / 255.0f);
  channels[2] *= (1.0f / 255.0f);
  channels[3] *= (1.0f / 255.0f);
}
