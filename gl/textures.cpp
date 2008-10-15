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
{
}

//-----------------------------------------------------------------------------
CTexture::~CTexture()
{
  this->free();
}

//-----------------------------------------------------------------------------
void
CTexture::init()
{
  minFilter   = GL_NEAREST_MIPMAP_LINEAR;
  magFilter   = GL_LINEAR;
  wrapS       = GL_REPEAT;
  wrapT       = GL_REPEAT;
}

//-----------------------------------------------------------------------------
void
CTexture::free()
{
}

//-----------------------------------------------------------------------------
void
CTexture::bind()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoftTexture::CSoftTexture()
 : CTexture()
 , data(NULL)
{
}

//-----------------------------------------------------------------------------
CSoftTexture::~CSoftTexture()
{
  this->free();
}

//-----------------------------------------------------------------------------
void
CSoftTexture::init()
{
}

//-----------------------------------------------------------------------------
void
CSoftTexture::free()
{
  if(data != NULL)
  {
    delete (uint8_t *)data;
    data = NULL;
  }

  CTexture::free();
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
        textures_[idxTex] = this->getTexture();
        textures_[idxTex]->init();

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
  if((GLint)format != internalformat)
  {
//    setError(GL_INVALID_OPERATION);
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
      case    8:
      case   16:
      case   32:
      case   64:
      case  128:
      case  256:
      case  512:
      case 1024:
        break;
      default:
  //      setError(GL_INVALID_VALUE);
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
        break;
      default:
  //      setError(GL_INVALID_VALUE);
        return;
    };

    ((CSoftTexture *)pCurrentTex_)->maskWidth  = width - 1;
    ((CSoftTexture *)pCurrentTex_)->maskHeight = width - 1;
    pCurrentTex_->width  = width;
    pCurrentTex_->height = height;
  }
  else
  {
    // MipMap level > 0
  }

  EColorFormat fmtTo   = renderSurface->mode.format;
  EColorFormat fmtFrom = convertGLToBxColorFormat(format, type);
  if(fmtFrom == cfUNKNOWN)
  {
//    setError(GL_INVALID_ENUM);
    return;
  }

  // Delete old texture buffer if present
  if(((CSoftTexture *)pCurrentTex_)->data != NULL)
    delete ((uint8_t *)((CSoftTexture *)pCurrentTex_)->data);
  // Allocate texture buffer
  switch(renderSurface->mode.bpp)
  {
    case 8:  ((CSoftTexture *)pCurrentTex_)->data = new uint8_t [width*height]; break;
    case 16: ((CSoftTexture *)pCurrentTex_)->data = new uint16_t[width*height]; break;
    case 32: ((CSoftTexture *)pCurrentTex_)->data = new uint32_t[width*height]; break;
    default:
      return; // ERROR, invalid render surface
  };

  convertImageFormat(((CSoftTexture *)pCurrentTex_)->data, fmtTo, pixels, fmtFrom, width, height);
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
          //case GL_CLAMP:         break;
          case GL_CLAMP_TO_EDGE: break;
          case GL_REPEAT:        break;
          default:
//            setError(GL_INVALID_ENUM);
            return;
        };
        pCurrentTex_->wrapS = (GLint)param;
        break;
      case GL_TEXTURE_WRAP_T:
        switch((GLint)param)
        {
          //case GL_CLAMP:         break;
          case GL_CLAMP_TO_EDGE: break;
          case GL_REPEAT:        break;
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

//-----------------------------------------------------------------------------
CTexture *
CAGLESTextures::getTexture()
{
  return new CSoftTexture;
}
