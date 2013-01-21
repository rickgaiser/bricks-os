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
 : pData_(NULL)
 , minFilter(GL_NEAREST_MIPMAP_LINEAR)
 , magFilter(GL_LINEAR)
 , uWrapMode(GL_REPEAT)
 , vWrapMode(GL_REPEAT)
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
  if(pData_ != NULL)
  {
    delete (uint8_t *)pData_;
    pData_ = NULL;
  }
}

//-----------------------------------------------------------------------------
void
CTexture::getTexel(CInt32_4 & c, float u, float v)
{
  getTexel(c, (int)(u * width), (int)(v * height));
}

//-----------------------------------------------------------------------------
void
CTexture::getTexel(CInt32_4 & c, int u, int v)
{
  uint32_t texel;

  getTexel(texel, u, v);

  // Convert to fixed point
  c.a = ((texel >> 24) & 0xff) * ((1<<14) / 255);
  c.r = ((texel >> 16) & 0xff) * ((1<<14) / 255);
  c.g = ((texel >>  8) & 0xff) * ((1<<14) / 255);
  c.b = ((texel      ) & 0xff) * ((1<<14) / 255);
}

//-----------------------------------------------------------------------------
void
CTexture::getTexel(uint32_t & c, int u, int v)
{
  // Wrap
  u &= iWidthMask_;
  v &= iHeightMask_;

  c = ((uint32_t *)pData_)[v * width + u];
}
