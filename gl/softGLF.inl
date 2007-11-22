#include "softGLF.h"
#include "matrix.h"
#include "fixedPoint.h"


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
      case GL_LESS:     if(zval <  zbuffer[index]){zbuffer[index] = zval; return true;} break;
      case GL_EQUAL:    if(zval == zbuffer[index]){zbuffer[index] = zval; return true;} break;
      case GL_LEQUAL:   if(zval <= zbuffer[index]){zbuffer[index] = zval; return true;} break;
      case GL_GREATER:  if(zval >  zbuffer[index]){zbuffer[index] = zval; return true;} break;
      case GL_NOTEQUAL: if(zval != zbuffer[index]){zbuffer[index] = zval; return true;} break;
      case GL_GEQUAL:   if(zval >= zbuffer[index]){zbuffer[index] = zval; return true;} break;
      case GL_ALWAYS:                              zbuffer[index] = zval; return true;  break;
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
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    color_t color = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(c.r * 255), (uint8_t)(c.g * 255), (uint8_t)(c.b * 255), (uint8_t)(c.a * 255));

    GLuint index((y * viewportWidth) + from.x_[y]);
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        switch(renderSurface->bpp_)
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
//      }
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test
inline void
CSoftGLESFloat::hlineZ(CEdgeF & from, CEdgeF & to, GLint y, SColorF c)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    GLfloat dx(1.0f / (GLfloat)(to.x_[y] - from.x_[y]));

    // Depth interpolation
    GLfloat z(from.z_[y]);
    GLfloat mz((to.z_[y] - from.z_[y]) * dx);

    color_t color = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(c.r * 255), (uint8_t)(c.g * 255), (uint8_t)(c.b * 255), (uint8_t)(c.a * 255));

    GLuint index((y * viewportWidth) + from.x_[y]);
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        if(testAndSetDepth(z, index) == true)
        {
          switch(renderSurface->bpp_)
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
//      }
      z += mz;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors
inline void
CSoftGLESFloat::hlineC(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    GLfloat dx(1.0f / (GLfloat)(to.x_[y] - from.x_[y]));

    // Color interpolation
    GLfloat r(from.c_[y].r);
    GLfloat g(from.c_[y].g);
    GLfloat b(from.c_[y].b);
    GLfloat a(from.c_[y].a);
    GLfloat mr((to.c_[y].r - from.c_[y].r) * dx);
    GLfloat mg((to.c_[y].g - from.c_[y].g) * dx);
    GLfloat mb((to.c_[y].b - from.c_[y].b) * dx);
    GLfloat ma((to.c_[y].a - from.c_[y].a) * dx);

    GLuint index((y * viewportWidth) + from.x_[y]);
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        switch(renderSurface->bpp_)
        {
          case 8:
            ((uint8_t  *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
            break;
          case 16:
            ((uint16_t *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
            break;
          case 32:
            ((uint32_t *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
            break;
        };
//      }
      r += mr;
      g += mg;
      b += mb;
      a += ma;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test, smooth colors
inline void
CSoftGLESFloat::hlineZC(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    GLfloat dx(1.0f / (GLfloat)(to.x_[y] - from.x_[y]));

    // Depth interpolation
    GLfloat z(from.z_[y]);
    GLfloat mz((to.z_[y] - from.z_[y]) * dx);

    // Color interpolation
    GLfloat r(from.c_[y].r);
    GLfloat g(from.c_[y].g);
    GLfloat b(from.c_[y].b);
    GLfloat a(from.c_[y].a);
    GLfloat mr((to.c_[y].r - from.c_[y].r) * dx);
    GLfloat mg((to.c_[y].g - from.c_[y].g) * dx);
    GLfloat mb((to.c_[y].b - from.c_[y].b) * dx);
    GLfloat ma((to.c_[y].a - from.c_[y].a) * dx);

    GLuint index((y * viewportWidth) + from.x_[y]);
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        if(testAndSetDepth(z, index) == true)
        {
          switch(renderSurface->bpp_)
          {
            case 8:
              ((uint8_t  *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
              break;
            case 16:
              ((uint16_t *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
              break;
            case 32:
              ((uint32_t *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
              break;
          };
        }
//      }
      z += mz;
      r += mr;
      g += mg;
      b += mb;
      a += ma;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, affine texture mapped
inline void
CSoftGLESFloat::hlineTa(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    GLfloat dx(1.0f / (GLfloat)(to.x_[y] - from.x_[y]));

    // Texture coordinate interpolation
    GLfloat ts(from.ts_[y]);
    GLfloat tt(from.tt_[y]);
    GLfloat mts((to.ts_[y] - from.ts_[y]) * dx);
    GLfloat mtt((to.tt_[y] - from.tt_[y]) * dx);

    ts  *= pCurrentTex_->width;
    tt  *= pCurrentTex_->height;
    mts *= pCurrentTex_->width;
    mtt *= pCurrentTex_->height;

    GLuint index((y * viewportWidth) + from.x_[y]);
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        switch(renderSurface->bpp_)
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
//      }
      ts += mts;
      tt += mtt;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test, affine texture mapped
inline void
CSoftGLESFloat::hlineZTa(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    GLfloat dx(1.0f / (GLfloat)(to.x_[y] - from.x_[y]));

    // Depth interpolation
    GLfloat z(from.z_[y]);
    GLfloat mz((to.z_[y] - from.z_[y]) * dx);

    // Texture coordinate interpolation
    GLfloat ts(from.ts_[y]);
    GLfloat tt(from.tt_[y]);
    GLfloat mts((to.ts_[y] - from.ts_[y]) * dx);
    GLfloat mtt((to.tt_[y] - from.tt_[y]) * dx);

    ts  *= pCurrentTex_->width;
    tt  *= pCurrentTex_->height;
    mts *= pCurrentTex_->width;
    mtt *= pCurrentTex_->height;

    GLuint index((y * viewportWidth) + from.x_[y]);
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        if((depthTestEnabled_ == false) || (testAndSetDepth(z, index) == true))
        {
          switch(renderSurface->bpp_)
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
//      }
      z  += mz;
      ts += mts;
      tt += mtt;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, perspective correct texture mapped
inline void
CSoftGLESFloat::hlineZTp(CEdgeF & from, CEdgeF & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    GLfloat dx(1.0f / (GLfloat)(to.x_[y] - from.x_[y]));

    // Depth interpolation
    GLfloat z(from.z_[y]);
    GLfloat mz((to.z_[y] - from.z_[y]) * dx);

    // Texture coordinate interpolation
    GLfloat tz(1.0f / from.z_[y]);
    GLfloat ts(from.ts_[y] * pCurrentTex_->width  * tz);
    GLfloat tt(from.tt_[y] * pCurrentTex_->height * tz);
    GLfloat mtz(((1.0f / to.z_[y]) - tz) * dx);
    GLfloat mts(((to.ts_[y] - from.ts_[y]) * pCurrentTex_->width  * tz) * dx);
    GLfloat mtt(((to.tt_[y] - from.tt_[y]) * pCurrentTex_->height * tz) * dx);

    GLuint index((y * viewportWidth) + from.x_[y]);
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        if((depthTestEnabled_ == false) || (testAndSetDepth(z, index) == true))
        {
          GLfloat recip = 1.0f / tz;
          GLfloat s     = ts * recip;
          GLfloat t     = tt * recip;
          switch(renderSurface->bpp_)
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
//      }
      z  += mz;
      tz += mtz;
      ts += mts;
      tt += mtt;
      index++;
    }
  }
}
