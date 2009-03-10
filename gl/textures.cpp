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
#include "bitResolution.h"

#include "unistd.h"
#include "string.h"


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
        case GL_RGB:  fmt = cfR8G8B8;   break;
        case GL_BGR:  fmt = cfB8G8R8;   break;
        case GL_RGBA: fmt = cfR8G8B8A8; break;
        case GL_BGRA: fmt = cfB8G8R8A8; break;
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
 : minFilter(GL_NEAREST_MIPMAP_LINEAR)
 , magFilter(GL_LINEAR)
 , wrapS(GL_REPEAT)
 , wrapT(GL_REPEAT)
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
//-----------------------------------------------------------------------------
CAGLESTextures::CAGLESTextures()
 : pCurrentTex_(NULL)
{
  for(int i(0); i < MAX_TEXTURE_COUNT; i++)
    textures_[i] = NULL;
}

//-----------------------------------------------------------------------------
CAGLESTextures::~CAGLESTextures()
{
  for(int i(0); i < MAX_TEXTURE_COUNT; i++)
    if(textures_[i] == NULL)
      delete textures_[i];
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glBindTexture(GLenum target, GLuint texture)
{
  if(target != GL_TEXTURE_2D)
  {
//    setError(GL_INVALID_ENUM);
    return;
  }
  if((texture >= MAX_TEXTURE_COUNT) || (textures_[texture] == NULL))
  {
//    setError(GL_INVALID_VALUE);
    return;
  }

  pCurrentTex_ = textures_[texture];

  pCurrentTex_->bind();
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  if(n < 0)
  {
//    setError(GL_INVALID_VALUE);
    return;
  }

  for(GLsizei i(0); i < n; i++)
    if(textures[i] < MAX_TEXTURE_COUNT)
      if(textures_[textures[i]] != NULL)
        delete textures_[textures[i]];
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glGenTextures(GLsizei n, GLuint *textures)
{
  if(n < 0)
  {
//    setError(GL_INVALID_VALUE);
    return;
  }

  for(GLsizei idxNr(0); idxNr < n; idxNr++)
  {
    bool bFound(false);

    for(GLuint idxTex(0); idxTex < MAX_TEXTURE_COUNT; idxTex++)
    {
      if(textures_[idxTex] == NULL)
      {
        textures_[idxTex] = new CTexture;

        textures[idxNr] = idxTex;
        bFound = true;
        break;
      }
    }

    if(bFound = false)
      break;
  }
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
  if(target != GL_TEXTURE_2D)
  {
//    setError(GL_INVALID_ENUM);
    return;
  }
  if(level < 0)
  {
//    setError(GL_INVALID_VALUE);
    return;
  }
  if(((format != GL_RGB) && (format != GL_BGR)) &&
     ((type == GL_UNSIGNED_SHORT_5_6_5) ||
      (type == GL_UNSIGNED_SHORT_5_6_5_REV)))
  {
//    setError(GL_INVALID_OPERATION);
    return;
  }
  if(((format != GL_RGBA) && (format != GL_BGRA)) &&
     ((type == GL_UNSIGNED_SHORT_4_4_4_4) ||
      (type == GL_UNSIGNED_SHORT_4_4_4_4_REV) ||
      (type == GL_UNSIGNED_SHORT_5_5_5_1) ||
      (type == GL_UNSIGNED_SHORT_1_5_5_5_REV)))
  {
//    setError(GL_INVALID_OPERATION);
    return;
  }

  // FIXME: MipMaps not supported
  if(level > 0)
  {
//    setError(GL_INVALID_OPERATION);
    return;
  }

  if((pCurrentTex_ == 0) || (renderSurface == 0))
  {
//    setError(GL_INVALID_OPERATION);
    return;
  }

  if(level == 0)
  {
    switch(width)
    {
      case    8: pCurrentTex_->bitWidth_ =  3; break;
      case   16: pCurrentTex_->bitWidth_ =  4; break;
      case   32: pCurrentTex_->bitWidth_ =  5; break;
      case   64: pCurrentTex_->bitWidth_ =  6; break;
      case  128: pCurrentTex_->bitWidth_ =  7; break;
      case  256: pCurrentTex_->bitWidth_ =  8; break;
      case  512: pCurrentTex_->bitWidth_ =  9; break;
      case 1024: pCurrentTex_->bitWidth_ = 10; break;
      default:
  //      setError(GL_INVALID_VALUE);
        return;
    };
    switch(height)
    {
      case    8: pCurrentTex_->bitHeight_ =  3; break;
      case   16: pCurrentTex_->bitHeight_ =  4; break;
      case   32: pCurrentTex_->bitHeight_ =  5; break;
      case   64: pCurrentTex_->bitHeight_ =  6; break;
      case  128: pCurrentTex_->bitHeight_ =  7; break;
      case  256: pCurrentTex_->bitHeight_ =  8; break;
      case  512: pCurrentTex_->bitHeight_ =  9; break;
      case 1024: pCurrentTex_->bitHeight_ = 10; break;
      default:
  //      setError(GL_INVALID_VALUE);
        return;
    };

    pCurrentTex_->iWidthMask_  = width  - 1;
    pCurrentTex_->iHeightMask_ = height - 1;
    pCurrentTex_->width        = width;
    pCurrentTex_->height       = height;
    pCurrentTex_->bRGBA_       = internalformat == 4;
  }
  else
  {
    // MipMap level > 0
  }

#ifdef CONFIG_GL_SIMPLE_TEXTURES
  EColorFormat fmtTo = renderSurface->mode.format;
#else
  #ifdef CONFIG_GL_TEXTURES_16BIT
  EColorFormat fmtTo = cfR5G6B5;
  #else
  EColorFormat fmtTo = cfA8R8G8B8;
  #endif
#endif

  EColorFormat fmtFrom = convertGLToBxColorFormat(format, type);
  if(fmtFrom == cfUNKNOWN)
  {
//    setError(GL_INVALID_ENUM);
    return;
  }

  // Delete old texture buffer if present
  if(pCurrentTex_->data_raw != NULL)
    delete ((uint8_t *)pCurrentTex_->data_raw);
  // Allocate texture buffer
  switch(renderSurface->mode.bpp)
  {
    case 8:  pCurrentTex_->data_raw = new uint8_t [width*height+3]; break;
    case 16: pCurrentTex_->data_raw = new uint16_t[width*height+3]; break;
    case 32: pCurrentTex_->data_raw = new uint32_t[width*height+3]; break;
    default:
      return; // ERROR, invalid render surface
  };
  // Align to 32bit
  pCurrentTex_->data = (void *)(((uint32_t)pCurrentTex_->data_raw + 3) & (~3));

  convertImageFormat(pCurrentTex_->data, fmtTo, pixels, fmtFrom, width, height);
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glTexSubImage2D(
  GLenum target,
  GLint level,
  GLint xoffset, GLint yoffset,
  GLsizei width, GLsizei height,
  GLenum format,
  GLenum type,
  const GLvoid * pixels)
{
  if(target != GL_TEXTURE_2D)
  {
//    setError(GL_INVALID_ENUM);
    return;
  }
  if(level < 0)
  {
//    setError(GL_INVALID_VALUE);
    return;
  }

  // FIXME: MipMaps not supported
  if(level > 0)
  {
//    setError(GL_INVALID_OPERATION);
    return;
  }

  if((pCurrentTex_ == 0) || (renderSurface == 0))
  {
//    setError(GL_INVALID_OPERATION);
    return;
  }

/*
  for(GLsizei y(0); y < height, y++)
  {
    for(GLsizei x(0); x < width, x++)
    {
      pCurrentTex_->data[...] = pixels[...];
    }
  }
*/
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  if(target != GL_TEXTURE_2D)
  {
//    setError(GL_INVALID_ENUM);
    return;
  }

  if(pCurrentTex_ != 0)
  {
    switch(pname)
    {
      case GL_TEXTURE_MIN_FILTER:
        switch((GLint)param)
        {
          case GL_NEAREST: break;
          case GL_LINEAR:  break;
          default:
//            setError(GL_INVALID_ENUM);
            return;
        };
        pCurrentTex_->minFilter = (GLint)param;
        break;
      case GL_TEXTURE_MAG_FILTER:
        switch((GLint)param)
        {
          case GL_NEAREST:                break;
          case GL_LINEAR:                 break;
          case GL_NEAREST_MIPMAP_NEAREST: break;
          case GL_LINEAR_MIPMAP_NEAREST:  break;
          case GL_NEAREST_MIPMAP_LINEAR:  break;
          case GL_LINEAR_MIPMAP_LINEAR:   break;
          default:
//            setError(GL_INVALID_ENUM);
            return;
        };
        pCurrentTex_->magFilter = (GLint)param;
        break;
      case GL_TEXTURE_WRAP_S:
        switch((GLint)param)
        {
          case GL_CLAMP:  break;
          case GL_REPEAT: break;
          default:
//            setError(GL_INVALID_ENUM);
            return;
        };
        pCurrentTex_->wrapS = (GLint)param;
        break;
      case GL_TEXTURE_WRAP_T:
        switch((GLint)param)
        {
          case GL_CLAMP:  break;
          case GL_REPEAT: break;
          default:
//            setError(GL_INVALID_ENUM);
            return;
        };
        pCurrentTex_->wrapT = (GLint)param;
        break;
      default:
//        setError(GL_INVALID_ENUM);
        return;
    };
  }
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
  glTexParameterf(target, pname, gl_fptof(param));
}
