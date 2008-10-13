#include "GL/glu.h"
#include "GLES/gl.h"
#include "math.h"
#include "textures.h"
#include "kernel/videoManager.h"


#define GLU_INVALID_VALUE -1
#define GLU_INVALID_ENUM  -1


//-----------------------------------------------------------------------------
int
gluBuild2DMipmaps(GLenum target, GLint components, GLint width, GLint height, GLenum format, GLenum type, const void * data)
{
  uint16_t * fromTexture;
  uint16_t * toTexture;
  unsigned int iLevel(0);

  if(target != GL_TEXTURE_2D)
    return GLU_INVALID_ENUM;

  if((components < 1) || (components > 4))
    return GLU_INVALID_VALUE;

  if((width < 0) || (height < 0))
    return GLU_INVALID_VALUE;

  switch(format)
  {
    case GL_RGB:  break;
    case GL_BGR:  break;
    case GL_RGBA: break;
    case GL_BGRA: break;
    default:
      return GLU_INVALID_ENUM;
  };

  switch(type)
  {
    case GL_UNSIGNED_SHORT_5_6_5:       break;
    case GL_UNSIGNED_SHORT_5_6_5_REV:   break;
    case GL_UNSIGNED_SHORT_4_4_4_4:     break;
    case GL_UNSIGNED_SHORT_4_4_4_4_REV: break;
    case GL_UNSIGNED_SHORT_5_5_5_1:     break;
    case GL_UNSIGNED_SHORT_1_5_5_5_REV: break;
//    case GL_UNSIGNED_INT_8_8_8_8:       break;
//    case GL_UNSIGNED_INT_8_8_8_8_REV:   break;
    default:
      return GLU_INVALID_ENUM;
  };

  // Load base texture
  glTexImage2D(target, iLevel, format, width, height, 0, format, type, data);
  iLevel++;

  // Create MipMaps
  fromTexture = (uint16_t *)data;
  while((width > 1) || (height > 1))
  {
    GLint newWidth  = (width  > 1) ? (width  >> 1) : width;
    GLint newHeight = (height > 1) ? (height >> 1) : height;

    // Allocate data for new texture
    toTexture = new uint16_t[newWidth * newHeight];

    // Scale texture
    gluScaleImage(format, width, height, type, fromTexture, newWidth, newHeight, type, toTexture);
    // Upload new MipMap level to GL
    glTexImage2D(target, iLevel, format, newWidth, newHeight, 0, format, type, toTexture);

    // Advance to next MipMap level
    width  = newWidth;
    height = newHeight;
    iLevel++;
    if(fromTexture != data)
      delete fromTexture;
    fromTexture = toTexture;
  }

  return 0;
}

//-----------------------------------------------------------------------------
void
gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
  GLdouble xmin, xmax, ymin, ymax;

  ymax = zNear * tan(fovy * M_PI / 360.0f);
  ymin = -ymax;
  xmin = ymin * aspect;
  xmax = ymax * aspect;

  glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}

//-----------------------------------------------------------------------------
int
gluScaleImage(GLenum format, GLint widthin, GLint heightin, GLenum typein, const void * datain, GLint widthout, GLint heightout, GLenum typeout, void * dataout)
{
  EColorFormat fmtTo;
  EColorFormat fmtFrom;
  unsigned int iWidthScale;
  unsigned int iHeightScale;
  unsigned int iTotalScale;
  unsigned int iTotalShift;
  unsigned int r, g, b, a;

  if((widthin < 0) || (heightin < 0) || (widthout < 0) || (heightout < 0))
    return GLU_INVALID_VALUE;

  fmtTo   = convertGLToBxColorFormat(format, typeout);
  fmtFrom = convertGLToBxColorFormat(format, typein);
  if((fmtTo == cfUNKNOWN) || (fmtFrom == cfUNKNOWN))
    return GLU_INVALID_ENUM;

  // We only scale down
  if((widthout > widthin) || (heightout > heightin))
    return GLU_INVALID_VALUE;

  iWidthScale  = widthin  / widthout;
  iHeightScale = heightin / heightout;
  iTotalScale  = iWidthScale * iHeightScale;
  switch(iTotalScale)
  {
    case  2: iTotalShift = 1; break;
    case  4: iTotalShift = 2; break;
    case  8: iTotalShift = 3; break;
    case 16: iTotalShift = 4; break;
    default: iTotalShift = 0;
  };

  for(unsigned int iY(0); iY < (unsigned int)heightout; iY++)
  {
    for(unsigned int iX(0); iX < (unsigned int)widthout; iX++)
    {
      unsigned int pixel;
      r=0;
      g=0;
      b=0;
      a=0;

      for(unsigned int iY2(0); iY2 < iHeightScale; iY2++)
      {
        for(unsigned int iX2(0); iX2 < iWidthScale; iX2++)
        {
          // Get source pixel
          pixel = ((uint16_t *)datain)[(iY*iHeightScale+iY2)*widthin+(iX*iWidthScale+iX2)];
          // Extract RGBA components
          r += BxColorFormat_GetR(fmtFrom, pixel);
          g += BxColorFormat_GetG(fmtFrom, pixel);
          b += BxColorFormat_GetB(fmtFrom, pixel);
          a += BxColorFormat_GetA(fmtFrom, pixel);
        }
      }

      if(iTotalShift != 0)
      {
        r >>= iTotalShift;
        g >>= iTotalShift;
        b >>= iTotalShift;
        a >>= iTotalShift;
      }
      else
      {
        r /= iTotalScale;
        g /= iTotalScale;
        b /= iTotalScale;
        a /= iTotalScale;
      }
      pixel = BxColorFormat_FromRGBA(fmtTo, r, g, b, a);

      ((uint16_t *)dataout)[iY * widthout + iX] = pixel;
    }
  }

  return 0;
}
