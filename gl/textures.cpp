#include "textures.h"
#include "unistd.h"
#include "string.h"


//-----------------------------------------------------------------------------
#define IMG_CONV_32_TO_32 0x44
#define IMG_CONV_32_TO_16 0x42
#define IMG_CONV_32_TO_08 0x41
#define IMG_CONV_16_TO_32 0x24
#define IMG_CONV_16_TO_16 0x22
#define IMG_CONV_16_TO_08 0x21
#define IMG_CONV_08_TO_32 0x14
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
    uint32_t i = 0;
    uint8_t iConv =
      ((colorFormatOps[srcFmt].bitsPerPixel >> 3) << 4) |
      ((colorFormatOps[dstFmt].bitsPerPixel >> 3)     );

    switch(iConv)
    {
      case IMG_CONV_32_TO_32:
        for(; i < iPixelCount; i++)
          ((uint32_t *)dst)[i] = BxColorFormat_Convert(srcFmt, dstFmt, ((uint32_t *)src)[i]);
        break;
      case IMG_CONV_32_TO_16:
        for(; i < iPixelCount; i++)
          ((uint16_t *)dst)[i] = BxColorFormat_Convert(srcFmt, dstFmt, ((uint32_t *)src)[i]);
        break;
      case IMG_CONV_32_TO_08:
        for(; i < iPixelCount; i++)
          ((uint8_t  *)dst)[i] = BxColorFormat_Convert(srcFmt, dstFmt, ((uint32_t *)src)[i]);
        break;
      case IMG_CONV_16_TO_32:
        for(; i < iPixelCount; i++)
          ((uint32_t *)dst)[i] = BxColorFormat_Convert(srcFmt, dstFmt, ((uint16_t *)src)[i]);
        break;
      case IMG_CONV_16_TO_16:
        for(; i < iPixelCount; i++)
          ((uint16_t *)dst)[i] = BxColorFormat_Convert(srcFmt, dstFmt, ((uint16_t *)src)[i]);
        break;
      case IMG_CONV_16_TO_08:
        for(; i < iPixelCount; i++)
          ((uint8_t  *)dst)[i] = BxColorFormat_Convert(srcFmt, dstFmt, ((uint16_t *)src)[i]);
        break;
      case IMG_CONV_08_TO_32:
        for(; i < iPixelCount; i++)
          ((uint32_t *)dst)[i] = BxColorFormat_Convert(srcFmt, dstFmt, ((uint8_t  *)src)[i]);
        break;
      case IMG_CONV_08_TO_16:
        for(; i < iPixelCount; i++)
          ((uint16_t *)dst)[i] = BxColorFormat_Convert(srcFmt, dstFmt, ((uint8_t  *)src)[i]);
        break;
      case IMG_CONV_08_TO_08:
        for(; i < iPixelCount; i++)
          ((uint8_t  *)dst)[i] = BxColorFormat_Convert(srcFmt, dstFmt, ((uint8_t  *)src)[i]);
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
CAGLESTextures::CAGLESTextures()
 : pCurrentTex_(NULL)
{
  // Initialize all texture structs
  for(GLuint idxTex(0); idxTex < MAX_TEXTURE_COUNT; idxTex++)
  {
    textures_[idxTex].used = false;
    textures_[idxTex].data = NULL;
  }
}

//-----------------------------------------------------------------------------
CAGLESTextures::~CAGLESTextures()
{
  // Clear all texture structs
  for(GLsizei idxTex(0); idxTex < MAX_TEXTURE_COUNT; idxTex++)
  {
    textures_[idxTex].used = false;
    if(textures_[idxTex].data != NULL)
      delete ((uint8_t *)textures_[idxTex].data);
  }
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
  if(texture >= MAX_TEXTURE_COUNT)
  {
//    setError(GL_INVALID_VALUE);
    return;
  }

  pCurrentTex_ = &textures_[texture];
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

  for(GLsizei idxNr(0); idxNr < n; idxNr++)
  {
    if(textures[idxNr] < MAX_TEXTURE_COUNT)
    {
      textures_[textures[idxNr]].used = false;
      if(textures_[textures[idxNr]].data != NULL)
        delete ((uint8_t *)textures_[textures[idxNr]].data);
    }
  }
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
      if(textures_[idxTex].used == false)
      {
        textures_[idxTex].used = true;

        // Initialize some default values
        textures_[idxTex].texMinFilter = GL_LINEAR;
        textures_[idxTex].texMagFilter = GL_LINEAR;
        textures_[idxTex].texWrapS     = GL_REPEAT;
        textures_[idxTex].texWrapT     = GL_REPEAT;

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

  // FIXME: MipMaps not supported
  if(level > 0)
    return;

  if((renderSurface != 0) && (pCurrentTex_ != 0) && (pCurrentTex_->used == true))
  {
    switch(width)
    {
      case    8:
      case   16:
      case   32:
      case   64:
      case  128:
      case  256:
      case  512:
      case 1024:
        pCurrentTex_->maskWidth = width - 1;
        break;
      default:
//        setError(GL_INVALID_VALUE);
        return;
    };
    switch(height)
    {
      case    8:
      case   16:
      case   32:
      case   64:
      case  128:
      case  256:
      case  512:
      case 1024:
        pCurrentTex_->maskHeight = width - 1;
        break;
      default:
//        setError(GL_INVALID_VALUE);
        return;
    };

    pCurrentTex_->width  = width;
    pCurrentTex_->height = height;

    EColorFormat fmtTo = renderSurface->mode.format;
    EColorFormat fmtFrom;
    switch(type)
    {
      case GL_UNSIGNED_BYTE:          fmtFrom = cfA8B8G8R8; break;
      case GL_UNSIGNED_SHORT_5_6_5:   fmtFrom = cfR5G6B5;   break;
      case GL_UNSIGNED_SHORT_4_4_4_4: fmtFrom = cfA4B4G4R4; break;
      case GL_UNSIGNED_SHORT_5_5_5_1: fmtFrom = cfA1B5G5R5; break;
      default:
//        setError(GL_INVALID_ENUM);
        return;
    };

    // Delete old texture buffer if present
    if(pCurrentTex_->data != NULL)
      delete ((uint8_t *)pCurrentTex_->data);
    // Allocate texture buffer
    switch(renderSurface->mode.bpp)
    {
      case 8:  pCurrentTex_->data = new uint8_t [width*height]; break;
      case 16: pCurrentTex_->data = new uint16_t[width*height]; break;
      case 32: pCurrentTex_->data = new uint32_t[width*height]; break;
      default:
        return; // ERROR, invalid render surface
    };

    convertImageFormat(pCurrentTex_->data, fmtTo, pixels, fmtFrom, width, height);
  }
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
      case GL_TEXTURE_MIN_FILTER: pCurrentTex_->texMinFilter = (GLint)param; break;
      case GL_TEXTURE_MAG_FILTER: pCurrentTex_->texMagFilter = (GLint)param; break;
      case GL_TEXTURE_WRAP_S:     pCurrentTex_->texWrapS     = (GLint)param; break;
      case GL_TEXTURE_WRAP_T:     pCurrentTex_->texWrapT     = (GLint)param; break;
    };
  }
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
  glTexParameterf(target, pname, gl_fptof(param));
}
