#include "softGLFx.h"
#include "matrix.h"
#include "fixedPoint.h"


//-----------------------------------------------------------------------------
inline bool
CSoftGLESFixed::testAndSetDepth(GLfixed z, uint32_t index)
{
/*
  if((z >= zNear_) && (z <= zFar_))
  {
    //uint32_t zval = z;
    uint16_t zval = z >> zLoss_;

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
CSoftGLESFixed::hline(CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    uint32_t color(fpRGB(c.r, c.g, c.b));
    GLuint index(((y + viewportYOffset) * renderSurface->mode.xpitch) + ((int)from.x_[y] + viewportXOffset));
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
//        switch(renderSurface->bpp_)
//        {
//          case 8:
//            ((uint8_t  *)renderSurface->p)[index] = color;
//            break;
//          case 16:
            ((uint16_t *)renderSurface->p)[index] = color;
//            break;
//          case 32:
//            ((uint32_t *)renderSurface->p)[index] = color;
//            break;
//        };
//      }
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test
inline void
CSoftGLESFixed::hlineZ(CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    CFixed xwidth1 = CFixed(1) / xwidth;

    // Depth interpolation
    CFixed z(from.z_[y]);
    CFixed mz((to.z_[y] - from.z_[y]).ipMul(xwidth1));

    uint32_t color(fpRGB(c.r, c.g, c.b));

    GLuint index(((y + viewportYOffset) * renderSurface->mode.xpitch) + ((int)from.x_[y] + viewportXOffset));
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        if(testAndSetDepth(z, index) == true)
        {
//          switch(renderSurface->bpp_)
//          {
//            case 8:
//              ((uint8_t  *)renderSurface->p)[index] = color;
//              break;
//            case 16:
              ((uint16_t *)renderSurface->p)[index] = color;
//              break;
//            case 32:
//              ((uint32_t *)renderSurface->p)[index] = color;
//              break;
//          };
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
CSoftGLESFixed::hlineC(CEdgeFx & from, CEdgeFx & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    CFixed xwidth1 = CFixed(1) / xwidth;

    // Color interpolation
    CFixed r(from.r_[y]);
    CFixed g(from.g_[y]);
    CFixed b(from.b_[y]);
//    CFixed a(from.a_[y]);
    CFixed mr((to.r_[y] - r).ipipMul(xwidth1));
    CFixed mg((to.g_[y] - g).ipipMul(xwidth1));
    CFixed mb((to.b_[y] - b).ipipMul(xwidth1));
//    CFixed ma((to.a_[y] - a).ipipMul(xwidth1));

    GLuint index(((y + viewportYOffset) * renderSurface->mode.xpitch) + ((int)from.x_[y] + viewportXOffset));
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
//        switch(renderSurface->bpp_)
//        {
//          case 8:
//            ((uint8_t  *)renderSurface->p)[index] = fpRGB(r.value, g.value, b.value);
//            break;
//          case 16:
            ((uint16_t *)renderSurface->p)[index] = fpRGB(r.value, g.value, b.value);
//            break;
//          case 32:
//            ((uint32_t *)renderSurface->p)[index] = fpRGB(r.value, g.value, b.value);
//            break;
//        };
//      }
      r += mr;
      g += mg;
      b += mb;
//      a += ma;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test, smooth colors
inline void
CSoftGLESFixed::hlineZC(CEdgeFx & from, CEdgeFx & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    CFixed xwidth1 = CFixed(1) / xwidth;

    // Depth interpolation
    CFixed z(from.z_[y]);
    CFixed mz((to.z_[y] - from.z_[y]).ipMul(xwidth1));

    // Color interpolation
    CFixed r(from.r_[y]);
    CFixed g(from.g_[y]);
    CFixed b(from.b_[y]);
//    CFixed a(from.a_[y]);
    CFixed mr((to.r_[y] - r).ipipMul(xwidth1));
    CFixed mg((to.g_[y] - g).ipipMul(xwidth1));
    CFixed mb((to.b_[y] - b).ipipMul(xwidth1));
//    CFixed ma((to.a_[y] - a).ipipMul(xwidth1));

    GLuint index(((y + viewportYOffset) * renderSurface->mode.xpitch) + ((int)from.x_[y] + viewportXOffset));
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        if(testAndSetDepth(z, index) == true)
        {
//          switch(renderSurface->bpp_)
//          {
//            case 8:
//              ((uint8_t  *)renderSurface->p)[index] = fpRGB(r.value, g.value, b.value);
//              break;
//            case 16:
              ((uint16_t *)renderSurface->p)[index] = fpRGB(r.value, g.value, b.value);
//              break;
//            case 32:
//              ((uint32_t *)renderSurface->p)[index] = fpRGB(r.value, g.value, b.value);
//              break;
//          };
        }
//      }
      z += mz;
      r += mr;
      g += mg;
      b += mb;
//      a += ma;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, affine texture mapped
inline void
CSoftGLESFixed::hlineTa(CEdgeFx & from, CEdgeFx & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    CFixed xwidth1 = CFixed(1) / xwidth;

    // Texture coordinate interpolation
    CFixed ts(from.ts_[y]);
    CFixed tt(from.tt_[y]);
    CFixed mts = (to.ts_[y] - from.ts_[y]).ipMul(xwidth1);
    CFixed mtt = (to.tt_[y] - from.tt_[y]).ipMul(xwidth1);

    ts  = ts  * pCurrentTex_->width  + 0.5f;
    tt  = tt  * pCurrentTex_->height + 0.5f;
    mts = mts * pCurrentTex_->width;
    mtt = mtt * pCurrentTex_->height;

    GLuint index(((y + viewportYOffset) * renderSurface->mode.xpitch) + ((int)from.x_[y] + viewportXOffset));
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
//        switch(renderSurface->bpp_)
//        {
//          case 8:
//            ((uint8_t  *)renderSurface->p)[index] = ((uint8_t  *)pCurrentTex_->data)[(((int32_t)tt & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((int32_t)ts & pCurrentTex_->maskWidth)];
//            break;
//          case 16:
            ((uint16_t *)renderSurface->p)[index] = ((uint16_t *)pCurrentTex_->data)[(((int32_t)tt & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((int32_t)ts & pCurrentTex_->maskWidth)];
//            break;
//          case 32:
//            ((uint32_t *)renderSurface->p)[index] = ((uint32_t *)pCurrentTex_->data)[(((int32_t)tt & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((int32_t)ts & pCurrentTex_->maskWidth)];
//            break;
//        };
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
CSoftGLESFixed::hlineZTa(CEdgeFx & from, CEdgeFx & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    CFixed xwidth1 = CFixed(1) / xwidth;

    // Depth interpolation
    CFixed z(from.z_[y]);
    CFixed mz((to.z_[y] - from.z_[y]).ipMul(xwidth1));

    // Texture coordinate interpolation
    CFixed ts(from.ts_[y]);
    CFixed tt(from.tt_[y]);
    CFixed mts = (to.ts_[y] - from.ts_[y]).ipMul(xwidth1);
    CFixed mtt = (to.tt_[y] - from.tt_[y]).ipMul(xwidth1);

    ts  = ts  * pCurrentTex_->width  + 0.5f;
    tt  = tt  * pCurrentTex_->height + 0.5f;
    mts = mts * pCurrentTex_->width;
    mtt = mtt * pCurrentTex_->height;

    GLuint index(((y + viewportYOffset) * renderSurface->mode.xpitch) + ((int)from.x_[y] + viewportXOffset));
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        if(testAndSetDepth(z, index) == true)
        {
//          switch(renderSurface->bpp_)
//          {
//            case 8:
//              ((uint8_t  *)renderSurface->p)[index] = ((uint8_t  *)pCurrentTex_->data)[(((int32_t)tt & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((int32_t)ts & pCurrentTex_->maskWidth)];
//              break;
//            case 16:
              ((uint16_t *)renderSurface->p)[index] = ((uint16_t *)pCurrentTex_->data)[(((int32_t)tt & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((int32_t)ts & pCurrentTex_->maskWidth)];
//              break;
//            case 32:
//              ((uint32_t *)renderSurface->p)[index] = ((uint32_t *)pCurrentTex_->data)[(((int32_t)tt & pCurrentTex_->maskHeight) * pCurrentTex_->width) + ((int32_t)ts & pCurrentTex_->maskWidth)];
//              break;
//          };
        }
//      }
      z  += mz;
      ts += mts;
      tt += mtt;
      index++;
    }
  }
}
/*
//-----------------------------------------------------------------------------
// Horizontal Line Fill, perspective correct texture mapped
inline void
CSoftGLESFixed::hlineZTp(CEdgeFx & from, CEdgeFx & to, GLint y)
{
  GLint xwidth(to.x_[y] - from.x_[y]);

  if(xwidth > 0)
  {
    CFixed xwidth1 = CFixed(1) / xwidth;

    // Depth interpolation
    CFixed z(from.z_[y]);
    CFixed mz((to.z_[y] - from.z_[y]).ipMul(xwidth1));

    // Texture coordinate interpolation
    GLfixed tz(gl_fpdiv(gl_fpfromi(1), from.z_[y]));
    GLfixed ts(gl_fpmul(from.ts_[y] * pCurrentTex_->width,  tz));
    GLfixed tt(gl_fpmul(from.tt_[y] * pCurrentTex_->height, tz));
    GLfixed mtz(((gl_fpdiv(gl_fpfromi(1), to.z_[y]) - tz)) / xwidth);
    GLfixed mts((gl_fpmul((to.ts_[y] - from.ts_[y]) * pCurrentTex_->width,  tz)) / xwidth);
    GLfixed mtt((gl_fpmul((to.tt_[y] - from.tt_[y]) * pCurrentTex_->height, tz)) / xwidth);

    GLuint index(((y + viewportYOffset) * renderSurface->mode.xpitch) + ((int)from.x_[y] + viewportXOffset));
//    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    while(xwidth--)
    {
//      if(x >= viewportWidth)
//        break;

//      if(x >= 0)
//      {
        if(testAndSetDepth(z, index) == true)
        {
          GLfixed recip = gl_fpdiv(gl_fpfromi(1), tz);
          GLfixed s     = gl_fpmul(ts, recip);
          GLfixed t     = gl_fpmul(tt, recip);
//          switch(renderSurface->bpp_)
//          {
//            case 8:
//              ((uint8_t  *)renderSurface->p)[index] = ((uint8_t  *)pCurrentTex_->data)[((gl_fptoi(t) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + (gl_fptoi(s) & pCurrentTex_->maskWidth)];
//              break;
//            case 16:
              ((uint16_t *)renderSurface->p)[index] = ((uint16_t *)pCurrentTex_->data)[((gl_fptoi(t) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + (gl_fptoi(s) & pCurrentTex_->maskWidth)];
//              break;
//            case 32:
//              ((uint32_t *)renderSurface->p)[index] = ((uint32_t *)pCurrentTex_->data)[((gl_fptoi(t) & pCurrentTex_->maskHeight) * pCurrentTex_->width) + (gl_fptoi(s) & pCurrentTex_->maskWidth)];
//              break;
//          };
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
*/
