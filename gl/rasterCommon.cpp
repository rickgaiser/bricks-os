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


#include "rasterCommon.h"
#include "GL/gl.h"
#ifndef __BRICKS__
#include "GL/glext.h"
#endif
#include "textures.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "mathlib.h"


#ifdef ENABLE_PROFILING
CTimeProfiler prof_render("render");
CTimeProfiler prof_clear ("clear");
CTimeProfiler prof_init  ("init");
CTimeProfiler prof_raster("raster");
uint64_t fillRate;
#endif


namespace raster
{

//-----------------------------------------------------------------------------
CASoftRasterizer::CASoftRasterizer()
 : pZBuffer_(NULL)
 , bSmoothShadingEnabled_(false)
 , bDepthTestEnabled_(false)
 , depthFunction_(GL_LESS)
 , depthClear_(1.0f)
 , zClearValue_((1<<SHIFT_Z)-1)
 , zRangeNear_(0.0f)
 , zRangeFar_(1.0f)
 , bTexturesEnabled_(false)
 , texEnvMode_(GL_MODULATE)
 , pCurrentTex_(NULL)
 , bBlendingEnabled_(false)
 , blendSFactor_(GL_ONE)
 , blendDFactor_(GL_ZERO)
{
  texEnvColor_.r = 0;
  texEnvColor_.g = 0;
  texEnvColor_.b = 0;
  texEnvColor_.a = 0;

  for(int i(0); i < MAX_TEXTURE_COUNT; i++)
    textures_[i] = NULL;

  // Default pixel center OpenGL (center)
  this->setUsePixelCenter(true);
}

//-----------------------------------------------------------------------------
CASoftRasterizer::~CASoftRasterizer()
{
  if(pZBuffer_)
    delete pZBuffer_;

  for(int i(0); i < MAX_TEXTURE_COUNT; i++)
    if(textures_[i] == NULL)
      delete textures_[i];
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::enableDepthTest(bool enable)
{
  bDepthTestEnabled_ = enable;

  if(enable == true)
  {
    if(pZBuffer_)
      delete pZBuffer_;

    // FIXME: Should be viewportPixelCount
    pZBuffer_ = new int32_t[(viewportWidth + viewportXOffset) * (viewportHeight + viewportYOffset)];
    //if(pZBuffer_ == NULL)
    //  setError(GL_OUT_OF_MEMORY);
  }
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::enableSmoothShading(bool enable)
{
  bSmoothShadingEnabled_ = enable;
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::enableTextures(bool enable)
{
  bTexturesEnabled_ = enable;
}
//-----------------------------------------------------------------------------
void
CASoftRasterizer::enableBlending(bool enable)
{
  bBlendingEnabled_ = enable;
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::clearDepthf(GLclampf depth)
{
  depthClear_ = mathlib::clamp<GLclampf>(depth, 0.0f, 1.0f);

  zClearValue_ = (int32_t)(depthClear_ * ((1<<SHIFT_Z)-1));
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::depthRangef(GLclampf zNear, GLclampf zFar)
{
  zRangeNear_ = mathlib::clamp<GLclampf>(zNear, 0.0f, 1.0f);
  zRangeFar_  = mathlib::clamp<GLclampf>(zFar,  0.0f, 1.0f);
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::depthFunc(GLenum func)
{
  depthFunction_ = func;
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::bindTexture(GLenum target, GLuint texture)
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
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::deleteTextures(GLsizei n, const GLuint *textures)
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
CASoftRasterizer::genTextures(GLsizei n, GLuint *textures)
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
CASoftRasterizer::texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
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
//  if((GLint)format != internalformat)
//  {
//    setError(GL_INVALID_OPERATION);
//    return;
//  }
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

  if((pCurrentTex_ == 0) || (renderSurface == 0))
  {
//    setError(GL_INVALID_OPERATION);
    return;
  }

  if(level == 0)
  {
    // MipMap level == 0
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

    pCurrentTex_->iMaxLevel_   = 0;
    pCurrentTex_->iWidthMask_  = width  - 1;
    pCurrentTex_->iHeightMask_ = height - 1;
    pCurrentTex_->width        = width;
    pCurrentTex_->height       = height;
    pCurrentTex_->bRGBA_       = true; // FIXME

    // Delete old texture buffer if present
    if(pCurrentTex_->data_raw != NULL)
      delete ((uint8_t *)pCurrentTex_->data_raw);

    // Allocate texture data
    //  - Alligned to 256bit == 32byte
    //  - Allocate MipMap levels also (1/3 of original size)
    // FIXME: dataSize is too much!
    int bpp = 32; // FIXME
    int dataSize = width * height * (bpp / 8);
    dataSize = dataSize + (dataSize >> 1);
    pCurrentTex_->data_raw = new unsigned char[dataSize + 32 - 1];
    pCurrentTex_->data     = (unsigned char *)((((unsigned int)pCurrentTex_->data_raw) + 32 - 1) & ~(32 - 1));

    // Set MipMap pointers
    unsigned char * pData = (unsigned char *)pCurrentTex_->data;
    int level_width  = pCurrentTex_->width;
    int level_height = pCurrentTex_->height;
    for(int iLevel(0); (level_width > 1) && (level_height > 1) && (iLevel < TEXTURE_MIPMAP_LEVEL_COUNT); iLevel++)
    {
      pCurrentTex_->level[iLevel].data = pData;

      pData += level_width * level_height * (bpp / 8);
      level_width  >>= 1;
      level_height >>= 1;
    }
  }
  else
  {
    // MipMap level > 0
    if((level != (pCurrentTex_->iMaxLevel_ + 1)) || (level >= TEXTURE_MIPMAP_LEVEL_COUNT))
    {
      // One level at a time!
      //setError(GL_INVALID_OPERATION);
      return;
    }
    pCurrentTex_->iMaxLevel_++;
  }

  EColorFormat fmtTo   = cfA8R8G8B8;//renderSurface->mode.format;
  EColorFormat fmtFrom = convertGLToBxColorFormat(format, type);
  if(fmtFrom == cfUNKNOWN)
  {
//    setError(GL_INVALID_ENUM);
    return;
  }

  convertImageFormat(pCurrentTex_->level[level].data, fmtTo, pixels, fmtFrom, width, height);
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::texParameterf(GLenum target, GLenum pname, GLfloat param)
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
        pCurrentTex_->minFilter = (GLint)param;
        break;
      case GL_TEXTURE_MAG_FILTER:
        switch((GLint)param)
        {
          case GL_NEAREST: break;
          case GL_LINEAR:  break;
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
        pCurrentTex_->uWrapMode = (GLint)param;
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
        pCurrentTex_->vWrapMode = (GLint)param;
        break;
      default:
//        setError(GL_INVALID_ENUM);
        return;
    };
  }
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::texEnvf(GLenum target, GLenum pname, GLfloat param)
{
  if(target != GL_TEXTURE_ENV)
  {
    //setError(GL_INVALID_ENUM);
    return;
  }

  if(pname != GL_TEXTURE_ENV_MODE)
  {
    //setError(GL_INVALID_ENUM);
    return;
  }

  switch((GLenum)param)
  {
    case GL_MODULATE:
    case GL_DECAL:
    case GL_BLEND:
    case GL_REPLACE:
      texEnvMode_ = (GLenum)param;
      break;
    default:
      //setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::blendFunc(GLenum sfactor, GLenum dfactor)
{
  blendSFactor_ = sfactor;
  blendDFactor_ = dfactor;

  if((sfactor == GL_ONE) && (dfactor == GL_ZERO))
    blendFast_ = FB_SOURCE;
  else if((sfactor == GL_ZERO) && (dfactor == GL_ONE))
    blendFast_ = FB_DEST;
  else if((sfactor == GL_SRC_ALPHA) && (dfactor == GL_ONE_MINUS_SRC_ALPHA))
    blendFast_ = FB_BLEND;
  else
    blendFast_ = FB_OTHER;
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  clClear.r = mathlib::clamp<GLclampf>(red,   0.0f, 1.0f);
  clClear.g = mathlib::clamp<GLclampf>(green, 0.0f, 1.0f);
  clClear.b = mathlib::clamp<GLclampf>(blue,  0.0f, 1.0f);
  clClear.a = mathlib::clamp<GLclampf>(alpha, 0.0f, 1.0f);
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::clear(GLbitfield mask)
{
#ifdef ENABLE_PROFILING
  prof_clear.start();
#endif

  if(mask & GL_COLOR_BUFFER_BIT)
  {
    color_t color = BxColorFormat_FromRGBA(renderSurface->mode.format, (uint8_t)(clClear.r * 255), (uint8_t)(clClear.g * 255), (uint8_t)(clClear.b * 255), (uint8_t)(clClear.a * 255));
    int yoff = renderSurface->mode.height - (viewportHeight + viewportYOffset);
    int xoff = viewportXOffset;

    switch(renderSurface->mode.bpp)
    {
      case 8:
      {
        for(int32_t y(0); y < viewportHeight; y++)
          for(int32_t x(0); x < viewportWidth; x++)
            ((uint8_t  *)renderSurface->p)[(y + yoff) * renderSurface->mode.xpitch + (x + xoff)] = color;
        break;
      }
      case 16:
      {
        for(int32_t y(0); y < viewportHeight; y++)
          for(int32_t x(0); x < viewportWidth; x++)
            ((uint16_t *)renderSurface->p)[(y + yoff) * renderSurface->mode.xpitch + (x + xoff)] = color;
        break;
      }
      case 32:
      {
        uint32_t * pBuffer = (uint32_t *)renderSurface->p;
        for(int i(0); i < viewportPixelCount; i++)
          pBuffer[i] = color;
        break;
      }
    };
  }
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    if(pZBuffer_ != NULL)
    {
      for(int i(0); i < viewportPixelCount; i++)
        pZBuffer_[i] = zClearValue_;
    }
  }
#ifdef ENABLE_PROFILING
  prof_clear.end();
#endif
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  viewportXOffset    = x;
  viewportYOffset    = y;
  viewportWidth      = width;
  viewportHeight     = height;
  viewportPixelCount = width * height;

  // Update z-buffer
  if(bDepthTestEnabled_ == true)
    enableDepthTest(true);
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::setUsePixelCenter(bool bCenter)
{
  bUsePixelCenter_ = bCenter;

  if(bCenter == true)
  {
    pixelFloorOffset_  = (1 << (SHIFT_XY - 1)) - 1; // Half a pixel minus one
    pixelCenterOffset_ = (1 << (SHIFT_XY - 1));     // Half a pixel (the center)
  }
  else
  {
    pixelFloorOffset_  = (1 << (SHIFT_XY    )) - 1; // A whole pixel minus one
    pixelCenterOffset_ = 0;                         // Zero (Top-Left corner)
  }

  oneMinusPixelCenterOffset_ = (1 << SHIFT_XY) - pixelCenterOffset_;
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::flush()
{
}

//-----------------------------------------------------------------------------
bool
CASoftRasterizer::rasterDepth(uint32_t z_pix, uint32_t z_buf)
{
  switch(depthFunction_)
  {
    case GL_LESS:     return (z_pix <  z_buf); break;
    case GL_EQUAL:    return (z_pix == z_buf); break;
    case GL_LEQUAL:   return (z_pix <= z_buf); break;
    case GL_GREATER:  return (z_pix >  z_buf); break;
    case GL_NOTEQUAL: return (z_pix != z_buf); break;
    case GL_GEQUAL:   return (z_pix >= z_buf); break;
    case GL_ALWAYS:   return true;             break;
    case GL_NEVER:    return false;            break;
  };

  return false;
}


};
