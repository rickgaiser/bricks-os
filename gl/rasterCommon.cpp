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


// FIXME
#define setError(x)


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
 , zClearValue_((1<<DEPTH_Z)-1)
 , bDepthMask_(true)
 , bTexturesEnabled_(false)
 , texEnvMode_(GL_MODULATE)
 , pCurrentTex_(NULL)
 , bBlendingEnabled_(false)
 , blendSFactor_(GL_ONE)
 , blendDFactor_(GL_ZERO)
 , viewportPixelCount(0)
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
  if(pZBuffer_ != NULL)
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
CASoftRasterizer::enableAlphaTest(bool enable)
{
  bAlphaTestEnabled_ = enable;
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::clearDepthf(GLclampf depth)
{
  depthClear_ = depth;
  zClearValue_ = (int32_t)(depthClear_ * ((1<<DEPTH_Z)-1));
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::depthFunc(GLenum func)
{
  depthFunction_ = func;
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::depthMask(GLboolean flag)
{
  bDepthMask_ = flag;
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::bindTexture(GLenum target, GLuint texture)
{
  pCurrentTex_ = NULL;

  switch(target)
  {
    //case GL_TEXTURE_1D:
    case GL_TEXTURE_2D:
    //case GL_TEXTURE_3D:
    //case GL_TEXTURE_3D_EXT:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  if((texture >= MAX_TEXTURE_COUNT) || (textures_[texture] == NULL))
  {
    setError(GL_INVALID_VALUE);
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
    setError(GL_INVALID_VALUE);
    return;
  }

  for(GLsizei i(0); i < n; i++)
  {
    if(textures[i] < MAX_TEXTURE_COUNT)
    {
      if(textures_[textures[i]] != NULL)
      {
        delete textures_[textures[i]];

        if(pCurrentTex_ == textures_[textures[i]])
          pCurrentTex_ = NULL;
        textures_[textures[i]] = NULL;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::genTextures(GLsizei n, GLuint *textures)
{
  if(n < 0)
  {
    setError(GL_INVALID_VALUE);
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
  unsigned int iWidthLog2;
  unsigned int iHeightLog2;
  bool bAlpha;

  if(pCurrentTex_ == NULL)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(target)
  {
    case GL_TEXTURE_2D:
    //case GL_PROXY_TEXTURE_2D:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  if(level < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  switch(internalformat)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    //case GL_ABGR_EXT:
    //case GL_ALPHA:
    //case GL_ALPHA4:
    //case GL_ALPHA8:
    //case GL_ALPHA12:
    //case GL_ALPHA16:
    //case GL_LUMINANCE:
    //case GL_LUMINANCE4:
    //case GL_LUMINANCE8:
    //case GL_LUMINANCE12:
    //case GL_LUMINANCE16:
    //case GL_LUMINANCE_ALPHA:
    //case GL_LUMINANCE4_ALPHA4:
    //case GL_LUMINANCE6_ALPHA2:
    //case GL_LUMINANCE8_ALPHA8:
    //case GL_LUMINANCE12_ALPHA4:
    //case GL_LUMINANCE12_ALPHA12:
    //case GL_LUMINANCE16_ALPHA16:
    //case GL_INTENSITY:
    //case GL_INTENSITY4:
    //case GL_INTENSITY8:
    //case GL_INTENSITY12:
    //case GL_INTENSITY16:
    case GL_R3_G3_B2:
    case GL_RGB:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
    case GL_RGBA:
    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGB5_A1:
    case GL_RGBA8:
    case GL_RGB10_A2:
    case GL_RGBA12:
    case GL_RGBA16:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(width)
  {
    //case    1: iWidthLog2 =  0; break;
    //case    2: iWidthLog2 =  1; break;
    //case    4: iWidthLog2 =  2; break;
    case    8: iWidthLog2 =  3; break;
    case   16: iWidthLog2 =  4; break;
    case   32: iWidthLog2 =  5; break;
    case   64: iWidthLog2 =  6; break;
    case  128: iWidthLog2 =  7; break;
    case  256: iWidthLog2 =  8; break;
    case  512: iWidthLog2 =  9; break;
    case 1024: iWidthLog2 = 10; break;
    default:
      setError(GL_INVALID_VALUE);
      return;
  };

  switch(height)
  {
    //case    1: iHeightLog2 =  0; break;
    //case    2: iHeightLog2 =  1; break;
    //case    4: iHeightLog2 =  2; break;
    case    8: iHeightLog2 =  3; break;
    case   16: iHeightLog2 =  4; break;
    case   32: iHeightLog2 =  5; break;
    case   64: iHeightLog2 =  6; break;
    case  128: iHeightLog2 =  7; break;
    case  256: iHeightLog2 =  8; break;
    case  512: iHeightLog2 =  9; break;
    case 1024: iHeightLog2 = 10; break;
    default:
      setError(GL_INVALID_VALUE);
      return;
  };

  switch(border)
  {
    case 0:
    case 1:
      break;
    default:
      setError(GL_INVALID_VALUE);
      return;
  };

  switch(format)
  {
    //case GL_COLOR_INDEX:
    //case GL_RED:
    //case GL_GREEN:
    //case GL_BLUE:
    //case GL_ALPHA:
    case GL_RGB:      bAlpha = false; break;
    case GL_RGBA:     bAlpha = true;  break;
    case GL_BGR:      bAlpha = false; break;
    case GL_BGRA:     bAlpha = true;  break;
    //case GL_ABGR_EXT: bAlpha = true;  break;
    //case GL_LUMINANCE:
    //case GL_422_EXT:
    //case GL_422_REV_EXT:
    //case GL_422_AVERAGE_EXT:
    //case GL_422_REV_AVERAGE_EXT:
    //case GL_LUMINANCE_ALPHA:
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(type)
  {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:
    case GL_BITMAP:
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  if(level == 0)
  {
    // MipMap level == 0
    pCurrentTex_->bitWidth_    = iWidthLog2;
    pCurrentTex_->bitHeight_   = iHeightLog2;
    pCurrentTex_->iMaxLevel_   = 0;
    pCurrentTex_->iWidthMask_  = width  - 1;
    pCurrentTex_->iHeightMask_ = height - 1;
    pCurrentTex_->width        = width;
    pCurrentTex_->height       = height;
    pCurrentTex_->bRGBA_       = bAlpha;

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
      setError(GL_INVALID_OPERATION);
      return;
    }
    pCurrentTex_->iMaxLevel_++;
  }

  EColorFormat fmtTo   = cfA8R8G8B8;//renderSurface->mode.format;
  EColorFormat fmtFrom = convertGLToBxColorFormat(format, type);
  if(fmtFrom == cfUNKNOWN)
  {
    setError(GL_INVALID_ENUM);
    return;
  }

  convertImageFormat(pCurrentTex_->level[level].data, fmtTo, pixels, fmtFrom, width, height);
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::texParameterf(GLenum target, GLenum pname, GLfloat param)
{
  if(pCurrentTex_ == NULL)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(target)
  {
    //case GL_TEXTURE_1D:
    case GL_TEXTURE_2D:
    //case GL_TEXTURE_3D:
    //case GL_TEXTURE_CUBE_MAP:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

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
          setError(GL_INVALID_ENUM);
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
          setError(GL_INVALID_ENUM);
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
          setError(GL_INVALID_ENUM);
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
          setError(GL_INVALID_ENUM);
          return;
      };
      pCurrentTex_->vWrapMode = (GLint)param;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::texEnvf(GLenum target, GLenum pname, GLfloat param)
{
  switch(target)
  {
    case GL_TEXTURE_ENV:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(pname)
  {
    case GL_TEXTURE_ENV_MODE:
      switch((GLenum)param)
      {
        case GL_MODULATE:
        case GL_DECAL:
        case GL_BLEND:
        //case GL_COMBINE_EXT:
        case GL_ADD:
        case GL_REPLACE:
        //case GL_ADD_SIGNED_EXT:
        //case GL_INTERPOLATE_EXT:
          break;
        default:
          setError(GL_INVALID_ENUM);
          return;
      };
      texEnvMode_ = (GLenum)param;
      break;
    //case GL_COMBINE_RGB_EXT:
    //case GL_COMBINE_ALPHA_EXT:
    //case GL_SOURCE0_RGB_EXT:
    //case GL_SOURCE1_RGB_EXT:
    //case GL_SOURCE2_RGB_EXT:
    //case GL_SOURCE0_ALPHA_EXT:
    //case GL_SOURCE1_ALPHA_EXT:
    //case GL_SOURCE2_ALPHA_EXT:
    //case GL_OPERAND0_RGB_EXT:
    //case GL_OPERAND1_RGB_EXT:
    //case GL_OPERAND2_RGB_EXT:
    //case GL_OPERAND0_ALPHA_EXT:
    //case GL_OPERAND1_ALPHA_EXT:
    //case GL_OPERAND2_ALPHA_EXT:
    //case GL_RGB_SCALE_EXT:
    //case GL_ALPHA_SCALE:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::colorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
  switch(target)
  {
    case GL_COLOR_TABLE:
    //case GL_TEXTURE_COLOR_TABLE_EXT:
    //case GL_PROXY_COLOR_TABLE:
    //case GL_PROXY_TEXTURE_COLOR_TABLE_EXT:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(internalformat)
  {
    //case GL_ABGR_EXT:
    //case GL_ALPHA:
    //case GL_ALPHA4:
    //case GL_ALPHA8:
    //case GL_ALPHA12:
    //case GL_ALPHA16:
    //case GL_LUMINANCE:
    //case GL_LUMINANCE4:
    //case GL_LUMINANCE8:
    //case GL_LUMINANCE12:
    //case GL_LUMINANCE16:
    //case GL_LUMINANCE_ALPHA:
    //case GL_LUMINANCE4_ALPHA4:
    //case GL_LUMINANCE6_ALPHA2:
    //case GL_LUMINANCE8_ALPHA8:
    //case GL_LUMINANCE12_ALPHA4:
    //case GL_LUMINANCE12_ALPHA12:
    //case GL_LUMINANCE16_ALPHA16:
    //case GL_INTENSITY:
    //case GL_INTENSITY4:
    //case GL_INTENSITY8:
    //case GL_INTENSITY12:
    //case GL_INTENSITY16:
    case GL_R3_G3_B2:
    case GL_RGB:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
    case GL_RGBA:
    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGB5_A1:
    case GL_RGBA8:
    case GL_RGB10_A2:
    case GL_RGBA12:
    case GL_RGBA16:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(format)
  {
    //case GL_RED:
    //case GL_GREEN:
    //case GL_BLUE:
    //case GL_ALPHA:
    //case GL_LUMINANCE:
    //case GL_LUMINANCE_ALPHA:
    case GL_RGB:
    case GL_BGR:
    case GL_RGBA:
    case GL_BGRA:
    //case GL_422_EXT:
    //case GL_422_REV_EXT:
    //case GL_422_AVERAGE_EXT:
    //case GL_422_REV_AVERAGE_EXT:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(type)
  {
    case GL_UNSIGNED_BYTE:
    //case GL_BYTE:
    //case GL_UNSIGNED_SHORT:
    //case GL_SHORT:
    //case GL_UNSIGNED_INT:
    //case GL_INT:
    //case GL_FLOAT:
    //case GL_UNSIGNED_BYTE_3_3_2:
    //case GL_UNSIGNED_BYTE_2_3_3_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    //case GL_UNSIGNED_INT_10_10_10_2:
    //case GL_UNSIGNED_INT_2_10_10_10_REV:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::alphaFunc(GLenum func, GLclampf ref)
{
  alphaFunc_  = func;
  alphaValue_ = ref;
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
  clClear.r = red;
  clClear.g = green;
  clClear.b = blue;
  clClear.a = alpha;
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
CASoftRasterizer::begin(GLenum mode)
{
}

//-----------------------------------------------------------------------------
void
CASoftRasterizer::end()
{
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
