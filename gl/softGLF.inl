#include "softGLF.h"
#include "matrix.h"
#include "fixedPoint.h"


#define SL_LINE_WIDTH() \
  ((GLint)(to.x_[y] - from.x_[y]))
#define SL_PIXEL_INDEX() \
  ((GLuint)(((y + viewportYOffset) * renderSurface->mode.xpitch) + (from.x_[y] + viewportXOffset)))
#define SL_COLOR() \
  BxColorFormat_FromRGBA(renderSurface->mode.format, (uint8_t)(c.r * 255), (uint8_t)(c.g * 255), (uint8_t)(c.b * 255), (uint8_t)(c.a * 255))

#define SL_INTERPOLATE_COMMON() \
  GLfloat xwidth1 = (1.0f / (GLfloat)(to.x_[y] - from.x_[y]))
#define SL_INTERPOLATE_Z() \
  GLfloat z(from.z_[y]); \
  GLfloat mz((to.z_[y] - from.z_[y]) * xwidth1)
#define SL_INTERPOLATE_C() \
  GLfloat r(from.r_[y]); \
  GLfloat g(from.g_[y]); \
  GLfloat b(from.b_[y]); \
  GLfloat a(from.a_[y]); \
  GLfloat mr((to.r_[y] - from.r_[y]) * xwidth1); \
  GLfloat mg((to.g_[y] - from.g_[y]) * xwidth1); \
  GLfloat mb((to.b_[y] - from.b_[y]) * xwidth1); \
  GLfloat ma((to.a_[y] - from.a_[y]) * xwidth1)
#define SL_INTERPOLATE_TA() \
  GLfloat ts(from.ts_[y]); \
  GLfloat tt(from.tt_[y]); \
  GLfloat mts((to.ts_[y] - from.ts_[y]) * xwidth1); \
  GLfloat mtt((to.tt_[y] - from.tt_[y]) * xwidth1); \
  ts  *= pCurrentTex_->width; \
  tt  *= pCurrentTex_->height; \
  mts *= pCurrentTex_->width; \
  mtt *= pCurrentTex_->height
#define SL_INTERPOLATE_TP() \
  GLfloat tz(1.0f / from.z_[y]); \
  GLfloat ts(from.ts_[y] * pCurrentTex_->width  * tz); \
  GLfloat tt(from.tt_[y] * pCurrentTex_->height * tz); \
  GLfloat mtz(((1.0f / to.z_[y]) - tz) * xwidth1); \
  GLfloat mts(((to.ts_[y] - from.ts_[y]) * pCurrentTex_->width  * tz) * xwidth1); \
  GLfloat mtt(((to.tt_[y] - from.tt_[y]) * pCurrentTex_->height * tz) * xwidth1)

#define SL_INCREMENT_Z() \
  z += mz
#define SL_INCREMENT_C() \
  r += mr; \
  g += mg; \
  b += mb; \
  a += ma
#define SL_INCREMENT_TA() \
  ts += mts; \
  tt += mtt
#define SL_INCREMENT_TP() \
  tz += mtz; \
  ts += mts; \
  tt += mtt


//-----------------------------------------------------------------------------
inline bool
CSoftGLESFloat::testAndSetDepth(GLfloat z, uint32_t index)
{
/*
  if((z >= zNear_) && (z <= zFar_))
  {
    //uint32_t zval = (uint32_t)((zA_ + (zB_ / z)) * 0xffffffff);
    uint32_t zval = (uint32_t)(((z - zNear_) / (zFar_ - zNear_)) * 0xffffffff);

    switch(depthFunction_)
    {
      case GL_LESS:     if(zval <  pZBuffer_[index]){pZBuffer_[index] = zval; return true;} break;
      case GL_EQUAL:    if(zval == pZBuffer_[index]){pZBuffer_[index] = zval; return true;} break;
      case GL_LEQUAL:   if(zval <= pZBuffer_[index]){pZBuffer_[index] = zval; return true;} break;
      case GL_GREATER:  if(zval >  pZBuffer_[index]){pZBuffer_[index] = zval; return true;} break;
      case GL_NOTEQUAL: if(zval != pZBuffer_[index]){pZBuffer_[index] = zval; return true;} break;
      case GL_GEQUAL:   if(zval >= pZBuffer_[index]){pZBuffer_[index] = zval; return true;} break;
      case GL_ALWAYS:                                pZBuffer_[index] = zval; return true;  break;
      case GL_NEVER:                                                      return false; break;
    };
  }
*/
  return false;
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill
inline void
CSoftGLESFloat::hline(CEdgeF & from, CEdgeF & to, GLint y, SColorF c)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    color_t color = SL_COLOR();
    GLuint index  = SL_PIXEL_INDEX();

    while(xwidth--)
    {
      switch(renderSurface->mode.bpp)
      {
        case 8:
          ((uint8_t  *)renderSurface->p)[index] = color;
          break;
        case 16:
          ((uint16_t *)renderSurface->p)[index] = color;
          break;
        case 32:
          ((uint32_t *)renderSurface->p)[index] = color;
          break;
      };
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test
inline void
CSoftGLESFloat::hlineZ(CEdgeF & from, CEdgeF & to, GLint y, SColorF c)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_Z();
    color_t color = SL_COLOR();
    GLuint index  = SL_PIXEL_INDEX();

    while(xwidth--)
    {
      if(testAndSetDepth(z, index) == true)
      {
        switch(renderSurface->mode.bpp)
        {
          case 8:
            ((uint8_t  *)renderSurface->p)[index] = color;
            break;
          case 16:
            ((uint16_t *)renderSurface->p)[index] = color;
            break;
          case 32:
            ((uint32_t *)renderSurface->p)[index] = color;
            break;
        };
      }
      SL_INCREMENT_Z();
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors
inline void
CSoftGLESFloat::hlineC(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_C();
    GLuint index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
      switch(renderSurface->mode.bpp)
      {
        case 8:
          ((uint8_t  *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->mode.format, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
          break;
        case 16:
          ((uint16_t *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->mode.format, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
          break;
        case 32:
          ((uint32_t *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->mode.format, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
          break;
      };
      SL_INCREMENT_C();
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test, smooth colors
inline void
CSoftGLESFloat::hlineZC(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_Z();
    SL_INTERPOLATE_C();
    GLuint index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
      if(testAndSetDepth(z, index) == true)
      {
        switch(renderSurface->mode.bpp)
        {
          case 8:
            ((uint8_t  *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->mode.format, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
            break;
          case 16:
            ((uint16_t *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->mode.format, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
            break;
          case 32:
            ((uint32_t *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->mode.format, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
            break;
        };
      }
      SL_INCREMENT_Z();
      SL_INCREMENT_C();
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, affine texture mapped
inline void
CSoftGLESFloat::hlineTa(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_TA();
    GLuint index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
      switch(renderSurface->mode.bpp)
      {
        case 8:
          ((uint8_t  *)renderSurface->p)[index] = ((uint8_t  *)pCurrentTex_->data)[(((GLint)(tt) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((GLint)(ts) & pCurrentTex_->maskWidth)];
          break;
        case 16:
          ((uint16_t *)renderSurface->p)[index] = ((uint16_t *)pCurrentTex_->data)[(((GLint)(tt) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((GLint)(ts) & pCurrentTex_->maskWidth)];
          break;
        case 32:
          ((uint32_t *)renderSurface->p)[index] = ((uint32_t *)pCurrentTex_->data)[(((GLint)(tt) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((GLint)(ts) & pCurrentTex_->maskWidth)];
          break;
      };
      SL_INCREMENT_TA();
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test, affine texture mapped
inline void
CSoftGLESFloat::hlineZTa(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_Z();
    SL_INTERPOLATE_TA();
    GLuint index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
      if((depthTestEnabled_ == false) || (testAndSetDepth(z, index) == true))
      {
        switch(renderSurface->mode.bpp)
        {
          case 8:
            ((uint8_t  *)renderSurface->p)[index] = ((uint8_t  *)pCurrentTex_->data)[(((GLint)(tt) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((GLint)(ts) & pCurrentTex_->maskWidth)];
            break;
          case 16:
            ((uint16_t *)renderSurface->p)[index] = ((uint16_t *)pCurrentTex_->data)[(((GLint)(tt) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((GLint)(ts) & pCurrentTex_->maskWidth)];
            break;
          case 32:
            ((uint32_t *)renderSurface->p)[index] = ((uint32_t *)pCurrentTex_->data)[(((GLint)(tt) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((GLint)(ts) & pCurrentTex_->maskWidth)];
            break;
        };
      }
      SL_INCREMENT_Z();
      SL_INCREMENT_TA();
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, perspective correct texture mapped
inline void
CSoftGLESFloat::hlineZTp(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_Z();
    SL_INTERPOLATE_TP();
    GLuint index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
      if((depthTestEnabled_ == false) || (testAndSetDepth(z, index) == true))
      {
        GLfloat recip = 1.0f / tz;
        GLfloat s     = ts * recip;
        GLfloat t     = tt * recip;
        switch(renderSurface->mode.bpp)
        {
          case 8:
            ((uint8_t  *)renderSurface->p)[index] = ((uint8_t  *)pCurrentTex_->data)[(((GLint)(t) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((GLint)(s) & pCurrentTex_->maskWidth)];
            break;
          case 16:
            ((uint16_t *)renderSurface->p)[index] = ((uint16_t *)pCurrentTex_->data)[(((GLint)(t) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((GLint)(s) & pCurrentTex_->maskWidth)];
            break;
          case 32:
            ((uint32_t *)renderSurface->p)[index] = ((uint32_t *)pCurrentTex_->data)[(((GLint)(t) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((GLint)(s) & pCurrentTex_->maskWidth)];
            break;
        };
      }
      SL_INCREMENT_Z();
      SL_INCREMENT_TP();
      index++;
    }
  }
}
