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


#include "3dRenderer.h"
#include "bios.h"
#include "dma.h"
#include "gif.h"
#include "stdlib.h"
#include "../../../../gl/textures.h"
#include "../../../../gl/mathlib.h"


#define setError(x)


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPS2TextureLevel::CPS2TextureLevel()
 : data(NULL)
 , gsMemLocation_(0)
 , iWidth_(0)
 , iHeight_(0)
 , iPsm_(GS_PSM_32)
 , used_(false)
{
}

//-----------------------------------------------------------------------------
CPS2TextureLevel::~CPS2TextureLevel()
{
  free();
}

//-----------------------------------------------------------------------------
void
CPS2TextureLevel::bind()
{
  // Load to fixed location
  gsMemLocation_ = 3*1024*1024;
  ee_to_gsBitBlt(gsMemLocation_, iPsm_, 0, 0, iWidth_, iHeight_, (unsigned int)data);
}

//-----------------------------------------------------------------------------
void
CPS2TextureLevel::use()
{
  used_ = true;
}

//-----------------------------------------------------------------------------
void
CPS2TextureLevel::free()
{
  used_ = false;

  if(data != NULL)
  {
    delete (uint8_t *)data;
    data = NULL;
  }
}

//-----------------------------------------------------------------------------
bool
CPS2TextureLevel::used()
{
  return used_;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPS2Texture::CPS2Texture(CGIFPacket & packet)
 : maxLevel_(0)
 , packet_(packet)
{
  ps2MinFilter = GS_TEX1_NEAREST_MIPMAP_LINEAR;
  ps2MagFilter = GS_TEX1_LINEAR;
}

//-----------------------------------------------------------------------------
CPS2Texture::~CPS2Texture()
{
  this->free();
}

//-----------------------------------------------------------------------------
void
CPS2Texture::free()
{
  for(int iLevel(0); iLevel < 7; iLevel++)
    level_[iLevel].free();
}

//-----------------------------------------------------------------------------
void
CPS2Texture::bind(uint16_t envMode)
{
  if(level_[0].used() == true)
  {
    // Transfer all texture levels to video memory
    for(int iLevel(0); iLevel < 7; iLevel++)
    {
      if(level_[iLevel].used() == false)
        break;

      level_[iLevel].bind();
    }

    packet_.gifAddPackedAD(GIF::REG::tex0_1,
      GIF::REG::TEX0(
        level_[0].gsMemLocation_ >> 8,   // base pointer
        level_[0].ps2Width_,             // width
        psm_,                            // pixel store mode
        iWidthLog2_,                     // width
        iHeightLog2_,                    // height
        rgba_,                           // 0=RGB, 1=RGBA
        envMode,                         // GL_TEXTURE_ENV_MODE
        0, 0, 0, 0, 0));                 // CLUT (currently not used)

    packet_.gifAddPackedAD(GIF::REG::tex1_1,
      GIF::REG::TEX1(
        0,                               // LOD Calculation: 0=formula, 1=fixed
        maxLevel_,                       // Max MipMap level
        ps2MagFilter,                    // GL_TEXTURE_MAG_FILTER
        ps2MinFilter,                    // GL_TEXTURE_MIN_FILTER
        0,                               // 0=We specify MipMap addr. 1=auto generated (with limitations)
        0,                               // LOD: L parameter (Shift LOD left 0..3)
        -20));                           // LOD: K parameter (Add to LOD -128..127)

    if(maxLevel_ > 0)
    {
      packet_.gifAddPackedAD(GIF::REG::miptbp1_1,
        GIF::REG::MIPTBP(
          level_[1].gsMemLocation_ >> 8, level_[1].ps2Width_,
          level_[2].gsMemLocation_ >> 8, level_[2].ps2Width_,
          level_[3].gsMemLocation_ >> 8, level_[3].ps2Width_));
    }

    if(maxLevel_ > 3)
    {
      packet_.gifAddPackedAD(GIF::REG::miptbp2_1,
        GIF::REG::MIPTBP(
          level_[4].gsMemLocation_ >> 8, level_[4].ps2Width_,
          level_[5].gsMemLocation_ >> 8, level_[5].ps2Width_,
          level_[6].gsMemLocation_ >> 8, level_[6].ps2Width_));
    }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPS23DRenderer::CPS23DRenderer(CPS2VideoDevice & device)
 : CAPS2Renderer()
 , device_(device)
 , zNear_(0.0f)
 , zFar_(1.0f)
 , fZA_(0.5f)
 , fZB_(0.5f)
{
  clClear = CFloat_4(0, 0, 0, 0);

  bDepthTestEnabled_ = false;
  ps2ZPSM_ = GS_PSM_16S;
  switch(ps2ZPSM_)
  {
    case GS_PSM_16:
    case GS_PSM_16S: zMax_ = 0x0000ffff; break;
    case GS_PSM_24:  zMax_ = 0x00ffffff; break;
    case GS_PSM_32:  zMax_ = 0xffffffff; break;
  };
  depthClear_ = 1.0f;
  zClearValue_ = (uint32_t)(depthClear_ * (GLfloat)zMax_);
  ps2ZBufferAddr_ = 0;
  ps2DepthFunction_ = GS_ZTST_GREATER;
  ps2DepthInvert_ = true;
  bDepthMask_ = true;

  bSmoothShadingEnabled_ = false;

  bTexturesEnabled_ = false;
  texEnvMode_ = GL_MODULATE;
  pCurrentTex_ = NULL;
  for(int i = 0; i < PS2_MAX_TEXTURE_COUNT; i++)
    textures_[i] = NULL;

  bAlphaTestEnabled_ = false;
  alphaFunc_ = GL_ALWAYS;
  ps2AlphaFunc_ = GS_ATST_ALWAYS;
  alphaValue_ = 0.0f;
  ps2AlphaValue_ = 0;

  bBlendingEnabled_ = false;
  blendSFactor_ = GL_ONE;
  blendDFactor_ = GL_ZERO;

  viewportXOffset = 0;
  viewportYOffset = 0;
  viewportWidth = 0;
  viewportHeight = 0;

  ps2Fog_ = GS_PRIM_FOG_OFF;
  ps2Aliasing_ = GS_PRIM_ALIASING_OFF;

  initializeGS();
}

//-----------------------------------------------------------------------------
CPS23DRenderer::~CPS23DRenderer()
{
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::initializeGS()
{
  // ZBuffer
  packet_.gifAddPackedAD(GIF::REG::zbuf_1, GIF::REG::ZBUF(ps2ZBufferAddr_ >> 13, ps2ZPSM_, !bDepthMask_));

  // Alpha blending
  packet_.gifAddPackedAD(GIF::REG::alpha_1, GIF::REG::ALPHA(0, 1, 0, 1, 0x00));
  packet_.gifAddPackedAD(GIF::REG::pabe,    0); // PABE: MSB of EACH pixels alpha value enables/disables alpha blending
  packet_.gifAddPackedAD(GIF::REG::fba_1,   0); // Alpha correction value (for correcting the alpha values written to the FB)

  // Test register:
  //  - Source alpha testing
  //  - Destination alpha testing (not used)
  //  - Depth testing
  packet_.gifAddPackedAD(GIF::REG::test_1, GIF::REG::TEST(bAlphaTestEnabled_, ps2AlphaFunc_, ps2AlphaValue_, 0, 0, 0, bDepthTestEnabled_, ps2DepthFunction_));

  // Primitive mode control: Primitive attributes are in
  //  - 0: PRMODE register OR
  //  - 1: PRIM register
  packet_.gifAddPackedAD(GIF::REG::prmodecont, 1);
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::setSurface(CSurface * surface)
{
  CAPS2Renderer::setSurface(surface);

  // First time initialization
  if(viewportXOffset == viewportYOffset == viewportWidth == viewportHeight == 0)
  {
    viewportWidth  = surface->width();
    viewportHeight = surface->height();
  }
}

//-----------------------------------------------------------------------------
CSurface *
CPS23DRenderer::getSurface()
{
  return CAPS2Renderer::getSurface();
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::enableDepthTest(bool enable)
{
  bDepthTestEnabled_ = enable;

  if((enable == true) && (ps2ZBufferAddr_ == 0) && (pSurface_ != NULL))
  {
    // Allocate z-buffer
    device_.allocFramebuffer(ps2ZBufferAddr_, pSurface_->mode.xpitch, pSurface_->mode.height, ps2ZPSM_);

    packet_.gifAddPackedAD(GIF::REG::zbuf_1, GIF::REG::ZBUF(ps2ZBufferAddr_ >> 13, ps2ZPSM_, !bDepthMask_));
  }

  // Update test register
  packet_.gifAddPackedAD(GIF::REG::test_1, GIF::REG::TEST(bAlphaTestEnabled_, ps2AlphaFunc_, ps2AlphaValue_, 0, 0, 0, bDepthTestEnabled_, ps2DepthFunction_));
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::enableSmoothShading(bool enable)
{
  bSmoothShadingEnabled_ = enable;
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::enableTextures(bool enable)
{
  bTexturesEnabled_ = enable;
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::enableBlending(bool enable)
{
  bBlendingEnabled_ = enable;
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::enableAlphaTest(bool enable)
{
  bAlphaTestEnabled_ = enable;

  // Update test register
  packet_.gifAddPackedAD(GIF::REG::test_1, GIF::REG::TEST(bAlphaTestEnabled_, ps2AlphaFunc_, ps2AlphaValue_, 0, 0, 0, bDepthTestEnabled_, ps2DepthFunction_));
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::clearDepthf(GLclampf depth)
{
  depthClear_ = depth;
  zClearValue_ = (uint32_t)(depthClear_ * (GLfloat)zMax_);
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::depthRange(GLclampf zNear, GLclampf zFar)
{
  zNear_ = zNear;
  zFar_  = zFar;

  fZA_   = (zFar - zNear) / 2;
  fZB_   = (mathlib::abs(zFar - zNear) / 2) + mathlib::min(zNear, zFar);
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::depthFunc(GLenum func)
{
  switch(func)
  {
    case GL_LESS:     ps2DepthFunction_ = GS_ZTST_GREATER; ps2DepthInvert_ = true;  break;
//    case GL_EQUAL:    ps2DepthFunction_ = ??; break;
    case GL_LEQUAL:   ps2DepthFunction_ = GS_ZTST_GEQUAL;  ps2DepthInvert_ = true;  break;
    case GL_GREATER:  ps2DepthFunction_ = GS_ZTST_GREATER; ps2DepthInvert_ = false; break;
//    case GL_NOTEQUAL: ps2DepthFunction_ = ??; break;
    case GL_GEQUAL:   ps2DepthFunction_ = GS_ZTST_GEQUAL;  ps2DepthInvert_ = false; break;
    case GL_NEVER:    ps2DepthFunction_ = GS_ZTST_NEVER;   ps2DepthInvert_ = false; break;
    case GL_ALWAYS:
    default:          ps2DepthFunction_ = GS_ZTST_ALWAYS;  ps2DepthInvert_ = false; break;
  }

  if(bDepthTestEnabled_ == true)
  {
    // Update test register
    packet_.gifAddPackedAD(GIF::REG::test_1, GIF::REG::TEST(bAlphaTestEnabled_, ps2AlphaFunc_, ps2AlphaValue_, 0, 0, 0, bDepthTestEnabled_, ps2DepthFunction_));
  }
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::depthMask(GLboolean flag)
{
  bDepthMask_ = flag;

  // Update the zbuf register
  packet_.gifAddPackedAD(GIF::REG::zbuf_1, GIF::REG::ZBUF(ps2ZBufferAddr_ >> 13, ps2ZPSM_, !bDepthMask_));
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::bindTexture(GLenum target, GLuint texture)
{
  //pCurrentTex_ = NULL;

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

  if((texture >= PS2_MAX_TEXTURE_COUNT) || (textures_[texture] == NULL))
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  bindTexture(textures_[texture]);
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::bindTexture(CPS2Texture * texture)
{
  pCurrentTex_ = texture;

  // Has the texture been loaded?
  if(pCurrentTex_->level_[0].used() == true)
  {
    uint16_t ps2TexEnvMode_;
    bTextureColor_ = false;
    bForceWhiteColor_ = false;

    // OpenGL Texture environment is difficult to translate to the ps2 texture environment
    if(pCurrentTex_->rgba_ == true)
    {
      switch(texEnvMode_)
      {
        // GL_MODULATE is exactly the same as GS_TEX0_MODULATE
        case GL_MODULATE:
          ps2TexEnvMode_ = GS_TEX0_MODULATE;
          bTextureColor_ = true;
          break;
        // GL_REPLACE is exactly the same as GS_TEX0_DECAL for RGBA textures
        case GL_REPLACE:
          ps2TexEnvMode_ = GS_TEX0_DECAL;
          break;
        // Not supported!!!, just show the texture
        case GL_DECAL:
        // Not supported!!!, just show the texture
        case GL_BLEND:
        // Not supported!!!, just show the texture
        case GL_ADD:
        default:
          ps2TexEnvMode_ = GS_TEX0_DECAL;
      };
    }
    else
    {
      switch(texEnvMode_)
      {
        // GL_MODULATE is exactly the same as GS_TEX0_MODULATE
        case GL_MODULATE:
          ps2TexEnvMode_ = GS_TEX0_MODULATE;
          bTextureColor_ = true;
          break;
        // GL_REPLACE is the same as GL_DECAL for RGB textures
        case GL_REPLACE:
        // ps2 decal mode uses texture alpha! So use modulate and force a white color
        case GL_DECAL:
          ps2TexEnvMode_ = GS_TEX0_MODULATE;
          bTextureColor_ = true;
          bForceWhiteColor_ = true;
          break;
        // Not supported!!!, just show the texture
        case GL_BLEND:
        // Not supported!!!, just show the texture
        case GL_ADD:
        default:
          ps2TexEnvMode_ = GS_TEX0_DECAL;
      };
    }

    pCurrentTex_->bind(ps2TexEnvMode_);
  }
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::deleteTextures(GLsizei n, const GLuint *textures)
{
  if(n < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  for(GLsizei i(0); i < n; i++)
  {
    if(textures[i] < PS2_MAX_TEXTURE_COUNT)
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
CPS23DRenderer::genTextures(GLsizei n, GLuint *textures)
{
  if(n < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  for(GLsizei idxNr(0); idxNr < n; idxNr++)
  {
    bool bFound(false);

    for(GLuint idxTex(0); idxTex < PS2_MAX_TEXTURE_COUNT; idxTex++)
    {
      if(textures_[idxTex] == NULL)
      {
        textures_[idxTex] = new CPS2Texture(packet_);

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
CPS23DRenderer::texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
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
    //case 1:
    //case 2:
    case 3: bAlpha = false; break;
    case 4: bAlpha = true;  break;
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
    case GL_R3_G3_B2: bAlpha = false; break;
    case GL_RGB:      bAlpha = false; break;
    case GL_RGB4:     bAlpha = false; break;
    case GL_RGB5:     bAlpha = false; break;
    case GL_RGB8:     bAlpha = false; break;
    case GL_RGB10:    bAlpha = false; break;
    case GL_RGB12:    bAlpha = false; break;
    case GL_RGB16:    bAlpha = false; break;
    case GL_RGBA:     bAlpha = true;  break;
    case GL_RGBA2:    bAlpha = true;  break;
    case GL_RGBA4:    bAlpha = true;  break;
    case GL_RGB5_A1:  bAlpha = true;  break;
    case GL_RGBA8:    bAlpha = true;  break;
    case GL_RGB10_A2: bAlpha = true;  break;
    case GL_RGBA12:   bAlpha = true;  break;
    case GL_RGBA16:   bAlpha = true;  break;
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
    case GL_RGB:
    case GL_RGBA:
    case GL_BGR:
    case GL_BGRA:
    //case GL_ABGR_EXT: bAlpha = true;  break;
    //case GL_LUMINANCE:
    //case GL_422_EXT:
    //case GL_422_REV_EXT:
    //case GL_422_AVERAGE_EXT:
    //case GL_422_REV_AVERAGE_EXT:
    //case GL_LUMINANCE_ALPHA:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  // Limit the number of mipmap levels
  if(level > 0)//6)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  if(level == 0)
  {
    // Clear entire texture
    pCurrentTex_->free();

    pCurrentTex_->iWidthLog2_  = iWidthLog2;
    pCurrentTex_->iHeightLog2_ = iHeightLog2;
    pCurrentTex_->width        = width;
    pCurrentTex_->height       = height;
    pCurrentTex_->rgba_        = bAlpha;
    pCurrentTex_->psm_         = GS_PSM_32;

    for(int iLevel(0); iLevel < 7; iLevel++)
    {
      // Texture Buffer Width, in units of 64 texels (>= 1)
      uint16_t ps2Width = width >> (iLevel+6);
      pCurrentTex_->level_[iLevel].ps2Width_ = (ps2Width >= 1) ? ps2Width : 1;
      pCurrentTex_->level_[iLevel].iWidth_   = width  >> level;
      pCurrentTex_->level_[iLevel].iHeight_  = height >> level;
      pCurrentTex_->level_[iLevel].iPsm_     = pCurrentTex_->psm_;
    }
  }
  else
  {
    // MipMap level > 0

    // Previous level must be used
    if(pCurrentTex_->level_[level-1].used() == false)
    {
      setError(GL_INVALID_OPERATION);
      return;
    }
    // Selected level must be free
    if(pCurrentTex_->level_[level-1].used() == true)
    {
      setError(GL_INVALID_OPERATION);
      return;
    }
    // Width and height must match the selected level
    if(((pCurrentTex_->width >> level) != width) || ((pCurrentTex_->height >> level) != height))
    {
      setError(GL_INVALID_OPERATION);
      return;
    }
    // RGB/RGBA must match
    if(pCurrentTex_->rgba_ != bAlpha)
    {
      setError(GL_INVALID_OPERATION);
      return;
    }
  }

  EColorFormat fmtTo;
  EColorFormat fmtFrom;

  // Get input format
  fmtFrom = convertGLToBxColorFormat(format, type);
  if(fmtFrom == cfUNKNOWN)
  {
    setError(GL_INVALID_ENUM);
    return;
  }
  // Get output format
  if(pCurrentTex_->psm_ == GS_PSM_16)
    fmtTo = cfA1B5G5R5;
  else
    fmtTo = cfA8B8G8R8;

  // Convert texture to GS texture format
  void * pData;
  if(colorFormatOps[fmtTo].bitsPerPixel <= 16)
    pData = new uint16_t[width*height];
  else
    pData = new uint32_t[width*height];
  if(pData == NULL)
  {
    setError(GL_OUT_OF_MEMORY);
    return;
  }
  convertImageFormat(pData, fmtTo, pixels, fmtFrom, width, height);

  pCurrentTex_->level_[level].data = pData;
  pCurrentTex_->maxLevel_ = level;
  pCurrentTex_->level_[level].use();

  // Transfer to video memory
  bindTexture(pCurrentTex_);
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::texParameterf(GLenum target, GLenum pname, GLfloat param)
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
    //case GL_TEXTURE_3D_EXT:
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
        case GL_NEAREST:                pCurrentTex_->ps2MinFilter = GS_TEX1_NEAREST;                break;
        case GL_LINEAR:                 pCurrentTex_->ps2MinFilter = GS_TEX1_LINEAR;                 break;
        case GL_NEAREST_MIPMAP_NEAREST: pCurrentTex_->ps2MinFilter = GS_TEX1_NEAREST_MIPMAP_NEAREST; break;
        case GL_LINEAR_MIPMAP_NEAREST:  pCurrentTex_->ps2MinFilter = GS_TEX1_LINEAR_MIPMAP_NEAREST;  break;
        case GL_NEAREST_MIPMAP_LINEAR:  pCurrentTex_->ps2MinFilter = GS_TEX1_NEAREST_MIPMAP_LINEAR;  break;
        case GL_LINEAR_MIPMAP_LINEAR:   pCurrentTex_->ps2MinFilter = GS_TEX1_LINEAR_MIPMAP_LINEAR;   break;
        default:
          setError(GL_INVALID_ENUM);
          return;
      };
      pCurrentTex_->minFilter = (GLint)param;
      break;
    case GL_TEXTURE_MAG_FILTER:
      switch((GLint)param)
      {
        case GL_NEAREST: pCurrentTex_->ps2MagFilter = GS_TEX1_NEAREST; break;
        case GL_LINEAR:  pCurrentTex_->ps2MagFilter = GS_TEX1_LINEAR; break;
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
CPS23DRenderer::texEnvf(GLenum target, GLenum pname, GLfloat param)
{
  if(target != GL_TEXTURE_ENV)
  {
    setError(GL_INVALID_ENUM);
    return;
  }

  if(pname != GL_TEXTURE_ENV_MODE)
  {
    setError(GL_INVALID_ENUM);
    return;
  }

  switch((GLenum)param)
  {
    case GL_MODULATE:
    case GL_DECAL:
    case GL_BLEND:
    case GL_REPLACE:
    case GL_ADD:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  texEnvMode_ = (GLenum)param;
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::texEnvfv(GLenum target, GLenum pname, const GLfloat * params)
{
/*
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
      switch((GLenum)params[0])
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
      texEnvMode_ = (GLenum)params[0];
      break;
    case GL_TEXTURE_ENV_COLOR:
      texEnvColor_ = CFloat_4(params);
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
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
*/
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::colorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
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
CPS23DRenderer::alphaFunc(GLenum func, GLclampf ref)
{
  switch(func)
  {
    case GL_NEVER:    ps2AlphaFunc_ = GS_ATST_NEVER;    break;
    case GL_LESS:     ps2AlphaFunc_ = GS_ATST_LESS;     break;
    case GL_EQUAL:    ps2AlphaFunc_ = GS_ATST_EQUAL;    break;
    case GL_LEQUAL:   ps2AlphaFunc_ = GS_ATST_LEQUAL;   break;
    case GL_GREATER:  ps2AlphaFunc_ = GS_ATST_GREATER;  break;
    case GL_NOTEQUAL: ps2AlphaFunc_ = GS_ATST_NOTEQUAL; break;
    case GL_GEQUAL:   ps2AlphaFunc_ = GS_ATST_GEQUAL;   break;
    case GL_ALWAYS:   ps2AlphaFunc_ = GS_ATST_ALWAYS;   break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  alphaFunc_  = func;
  alphaValue_ = ref;

  ps2AlphaValue_ = (uint8_t)(alphaValue_ * 0x80);

  if(bAlphaTestEnabled_ == true)
  {
    // Update test register
    packet_.gifAddPackedAD(GIF::REG::test_1, GIF::REG::TEST(bAlphaTestEnabled_, ps2AlphaFunc_, ps2AlphaValue_, 0, 0, 0, bDepthTestEnabled_, ps2DepthFunction_));
  }
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::blendFunc(GLenum sfactor, GLenum dfactor)
{
  blendSFactor_ = sfactor;
  blendDFactor_ = dfactor;

  if((sfactor == GL_SRC_ALPHA) && (dfactor == GL_ONE_MINUS_SRC_ALPHA))
  {
    // Normal alpha blending
    // (Cs - Cd) * As + Cd
    packet_.gifAddPackedAD(GIF::REG::alpha_1, GIF::REG::ALPHA(ALPHA_COLOR_SRC, ALPHA_COLOR_DST, ALPHA_ALPHA_SRC, ALPHA_COLOR_DST, 0x00));
  }
  else if((sfactor == GL_ZERO) && (dfactor == GL_ONE_MINUS_SRC_ALPHA))
  {
    // (0 - Cd) * As + Cd
    packet_.gifAddPackedAD(GIF::REG::alpha_1, GIF::REG::ALPHA(ALPHA_COLOR_ZERO, ALPHA_COLOR_DST, ALPHA_ALPHA_SRC, ALPHA_COLOR_DST, 0x00));
  }
  else if((sfactor == GL_ZERO) && (dfactor == GL_ONE_MINUS_SRC_COLOR))
  {
    // (0 - Cs) * 0x80 + Cs
    packet_.gifAddPackedAD(GIF::REG::alpha_1, GIF::REG::ALPHA(ALPHA_COLOR_ZERO, ALPHA_COLOR_SRC, ALPHA_ALPHA_FIX, ALPHA_COLOR_SRC, 0x80));
  }
  else if((sfactor == GL_ONE) && (dfactor == GL_ONE))
  {
    // (Cs - 0) * 0x80 + Cd
    packet_.gifAddPackedAD(GIF::REG::alpha_1, GIF::REG::ALPHA(ALPHA_COLOR_SRC, ALPHA_COLOR_ZERO, ALPHA_ALPHA_FIX, ALPHA_COLOR_DST, 0x80));
  }
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  clClear.r = mathlib::clamp<GLclampf>(red,   0.0f, 1.0f);
  clClear.g = mathlib::clamp<GLclampf>(green, 0.0f, 1.0f);
  clClear.b = mathlib::clamp<GLclampf>(blue,  0.0f, 1.0f);
  clClear.a = mathlib::clamp<GLclampf>(alpha, 0.0f, 1.0f);
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::clear(GLbitfield mask)
{
  if(pSurface_ == NULL)
    return;

  bool bClearZ = (mask & GL_DEPTH_BUFFER_BIT) && (ps2ZBufferAddr_ != 0);
  bool bClearC = (mask & GL_COLOR_BUFFER_BIT);// && (pSurface_ != NULL);

  if((bClearZ == false) && (bClearC == false))
    return;

  if(bClearC == false)
  {
    // Only update zbuffer
    packet_.gifAddPackedAD(GIF::REG::test_1, GIF::REG::TEST(bAlphaTestEnabled_, GS_ATST_NEVER, 0x80, GS_AFAIL_ZBONLY, 0, 0, 1, GS_ZTST_ALWAYS));
    packet_.gifAddPackedAD(GIF::REG::zbuf_1, GIF::REG::ZBUF(ps2ZBufferAddr_ >> 13, ps2ZPSM_, 0));
  }
  else if(bClearZ == false)
  {
    // Only update cbuffer
    packet_.gifAddPackedAD(GIF::REG::test_1, GIF::REG::TEST(0, 0, 0, 0, 0, 0, 0, 0));
    packet_.gifAddPackedAD(GIF::REG::zbuf_1, GIF::REG::ZBUF(ps2ZBufferAddr_ >> 13, ps2ZPSM_, 1));
  }
  else
  {
    // Update zbuffer and cbuffer
    packet_.gifAddPackedAD(GIF::REG::test_1, GIF::REG::TEST(0, 0, 0, 0, 0, 0, 1, GS_ZTST_ALWAYS));
    packet_.gifAddPackedAD(GIF::REG::zbuf_1, GIF::REG::ZBUF(ps2ZBufferAddr_ >> 13, ps2ZPSM_, 0));
  }

  uint8_t r = (uint8_t)(clClear.r * 255);
  uint8_t g = (uint8_t)(clClear.g * 255);
  uint8_t b = (uint8_t)(clClear.b * 255);
  //uint8_t a = (uint8_t)(clClear.a * 255);

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(GS_PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ(r, g, b, 0x80, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((0                  + GS_X_BASE) << 4, (0                   + GS_Y_BASE) << 4, zMax_ - zClearValue_));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((pSurface_->width() + GS_X_BASE) << 4, (pSurface_->height() + GS_Y_BASE) << 4, zMax_ - zClearValue_));

  // Restore test register
  packet_.gifAddPackedAD(GIF::REG::test_1, GIF::REG::TEST(bAlphaTestEnabled_, ps2AlphaFunc_, ps2AlphaValue_, 0, 0, 0, bDepthTestEnabled_, ps2DepthFunction_));
  // Restore z-buffer
  packet_.gifAddPackedAD(GIF::REG::zbuf_1, GIF::REG::ZBUF(ps2ZBufferAddr_ >> 13, ps2ZPSM_, !bDepthMask_));

  bDataWaiting_ = true;
  flush();
}

//-----------------------------------------------------------------------------
// Convert from normalized device coordinates. Example (640x480)
// x: in = -1..1  out = 0..640
// y: in = -1..1  out = 480..0 (NOTE: y is inverted)
void
CPS23DRenderer::viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(pSurface_ == NULL)
    return;

  viewportXOffset = x;
  viewportYOffset = (pSurface_->height() - height) - y;
  viewportWidth   = width;
  viewportHeight  = height;

  fXA_ = (GLfloat)(  (viewportWidth  >> 1));
  fXB_ = (GLfloat)(  (viewportWidth  >> 1) + viewportXOffset);
  fYA_ = (GLfloat)(- (viewportHeight >> 1));
  fYB_ = (GLfloat)(  (viewportHeight >> 1) + viewportYOffset);
}

//-----------------------------------------------------------------------------
// Not supported on PS2
void
CPS23DRenderer::setUsePixelCenter(bool bCenter)
{
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::flush()
{
  CAPS2Renderer::flush();
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::begin(GLenum mode)
{
//  switch(mode)
//  {
//    case GL_TRIANGLES:
      packet_.gifAddPackedAD(GIF::REG::prim, GIF::REG::PRIM(GS_PRIM_TRI,       bSmoothShadingEnabled_, bTexturesEnabled_, ps2Fog_, bBlendingEnabled_, ps2Aliasing_, GS_PRIM_TEXTURES_ST, 0, 0));
//      break;
//    case GL_TRIANGLE_STRIP:
//      packet_.gifAddPackedAD(GIF::REG::prim, GIF::REG::PRIM(GS_PRIM_TRI_STRIP, bSmoothShadingEnabled_, bTexturesEnabled_, ps2Fog_, bBlendingEnabled_, ps2Aliasing_, GS_PRIM_TEXTURES_ST, 0, 0));
//      break;
//    case GL_TRIANGLE_FAN:
//      packet_.gifAddPackedAD(GIF::REG::prim, GIF::REG::PRIM(GS_PRIM_TRI_FAN,   bSmoothShadingEnabled_, bTexturesEnabled_, ps2Fog_, bBlendingEnabled_, ps2Aliasing_, GS_PRIM_TEXTURES_ST, 0, 0));
//      break;
//  };
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::end()
{
  bDataWaiting_ = true;
  flush();
}

//-----------------------------------------------------------------------------
void
CPS23DRenderer::rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2)
{
  const SVertexF * va[3] = {&v0, &v1, &v2};

  for(int iVertex(0); iVertex < 3; iVertex++)
  {
    const SVertexF & v = *va[iVertex];

    // Add to message
    if(bTexturesEnabled_ == true)
    {
      // Use S/T/Q for perspective correct texture mapping
      GLfloat tq = v.vc.w;
      GLfloat ts = v.t.u * tq;
      GLfloat tt = v.t.v * tq;

      packet_.gifAddPackedAD(GIF::REG::st, GIF::REG::ST(ts, tt));
      if(bTextureColor_ == true)
      {
        // color range: 0x00 - 0x80 !!!! greater values will make the texture brighter
        // alpha range: 0x00 - 0x80
        if(bForceWhiteColor_ == true)
          packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ(0x80, 0x80, 0x80, (uint8_t)(v.c.a*0x80), tq));
        else
          packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ((uint8_t)(v.c.r*0x80), (uint8_t)(v.c.g*0x80), (uint8_t)(v.c.b*0x80), (uint8_t)(v.c.a*0x80), tq));
      }
      else
      {
        // We still need Q
        packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ(0x80, 0x80, 0x80, 0x80, tq));
      }
    }
    else
    {
      // color range: 0x00 - 0xff
      // alpha range: 0x00 - 0x80
      packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ((uint8_t)(v.c.r*255), (uint8_t)(v.c.g*255), (uint8_t)(v.c.b*255), (uint8_t)(v.c.a*0x80), 0));
    }

    int32_t x = fpfromf(4, (fXA_ * v.vd.x) + fXB_);
    int32_t y = fpfromf(4, (fYA_ * v.vd.y) + fYB_);
    int32_t z = 0; //(ps2DepthInvert_ == true) ? (zMax_ - v.z) : v.z;

    packet_.gifAddPackedAD(GIF::REG::xyz2, GIF::REG::XYZ2((GS_X_BASE<<4) + x, (GS_Y_BASE<<4) + y, z));
  }
}
