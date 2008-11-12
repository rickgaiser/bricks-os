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


#include "softGLFx.h"
#include "vhl/matrix.h"
#include "vhl/fixedPoint.h"


#define CURRENT_TEX ((CSoftTexture *)pCurrentTex_)

#define SL_LINE_WIDTH() \
  (to.x_[y] - from.x_[y])
#define SL_PIXEL_INDEX() \
  (((y + viewportYOffset) * renderSurface->mode.xpitch) + ((int)from.x_[y] + viewportXOffset))
#define SL_COLOR() \
  (fpRGB(c.r, c.g, c.b))

#define SL_INTERPOLATE_COMMON() \
  CFixed xwidth1 = CFixed(1) / xwidth
#define SL_INTERPOLATE_Z() \
  CFixed z(from.z_[y]); \
  CFixed mz((to.z_[y] - from.z_[y]).ipMul(xwidth1))
#define SL_INTERPOLATE_C() \
  CFixed r(from.r_[y]); \
  CFixed g(from.g_[y]); \
  CFixed b(from.b_[y]); \
  CFixed mr((to.r_[y] - r).ipipMul(xwidth1)); \
  CFixed mg((to.g_[y] - g).ipipMul(xwidth1)); \
  CFixed mb((to.b_[y] - b).ipipMul(xwidth1))
#define SL_INTERPOLATE_TA() \
  CFixed ts(from.ts_[y]); \
  CFixed tt(from.tt_[y]); \
  CFixed mts = (to.ts_[y] - from.ts_[y]).ipMul(xwidth1); \
  CFixed mtt = (to.tt_[y] - from.tt_[y]).ipMul(xwidth1); \
  ts  *= CURRENT_TEX->width; \
  tt  *= CURRENT_TEX->height; \
  mts *= CURRENT_TEX->width; \
  mtt *= CURRENT_TEX->height
#define SL_INTERPOLATE_TP() \
  GLfixed tz(gl_fpdiv(gl_fpfromi(1), from.z_[y])); \
  GLfixed ts(gl_fpmul(from.ts_[y] * CURRENT_TEX->width,  tz)); \
  GLfixed tt(gl_fpmul(from.tt_[y] * CURRENT_TEX->height, tz)); \
  GLfixed mtz((gl_fpdiv(gl_fpfromi(1), to.z_[y]) - tz)).ipMul(xwidth1); \
  GLfixed mts(gl_fpmul((to.ts_[y] - from.ts_[y]) * CURRENT_TEX->width,  tz)).ipMul(xwidth1); \
  GLfixed mtt(gl_fpmul((to.tt_[y] - from.tt_[y]) * CURRENT_TEX->height, tz)).ipMul(xwidth1)

#define SL_INCREMENT_Z() \
  z += mz
#define SL_INCREMENT_C() \
  r += mr; \
  g += mg; \
  b += mb
#define SL_INCREMENT_TA() \
  ts += mts; \
  tt += mtt
#define SL_INCREMENT_TP() \
  tz += mtz; \
  ts += mts; \
  tt += mtt


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
CSoftGLESFixed::hline(CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    uint32_t color = SL_COLOR();
    GLuint   index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
//      switch(renderSurface->bpp_)
//      {
//        case 8:
//          ((uint8_t  *)renderSurface->p)[index] = color;
//          break;
//        case 16:
          ((uint16_t *)renderSurface->p)[index] = color;
//          break;
//        case 32:
//          ((uint32_t *)renderSurface->p)[index] = color;
//          break;
//      };
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test
inline void
CSoftGLESFixed::hlineZ(CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_Z();
    uint32_t color = SL_COLOR();
    GLuint   index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
      if(testAndSetDepth(z, index) == true)
      {
//        switch(renderSurface->bpp_)
//        {
//          case 8:
//            ((uint8_t  *)renderSurface->p)[index] = color;
//            break;
//          case 16:
            ((uint16_t *)renderSurface->p)[index] = color;
//          break;
//          case 32:
//            ((uint32_t *)renderSurface->p)[index] = color;
//            break;
//        };
      }
      SL_INCREMENT_Z();
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors
inline void
CSoftGLESFixed::hlineC(CEdgeFx & from, CEdgeFx & to, GLint y)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_C();
    GLuint index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
//      switch(renderSurface->bpp_)
//      {
//        case 8:
//          ((uint8_t  *)renderSurface->p)[index] = fpRGB(r, g, b);
//          break;
//        case 16:
          ((uint16_t *)renderSurface->p)[index] = fpRGB(r, g, b);
//          break;
//        case 32:
//          ((uint32_t *)renderSurface->p)[index] = fpRGB(r, g, b);
//          break;
//      };
      SL_INCREMENT_C();
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test, smooth colors
inline void
CSoftGLESFixed::hlineZC(CEdgeFx & from, CEdgeFx & to, GLint y)
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
//        switch(renderSurface->bpp_)
//        {
//          case 8:
//            ((uint8_t  *)renderSurface->p)[index] = fpRGB(r, g, b);
//            break;
//          case 16:
            ((uint16_t *)renderSurface->p)[index] = fpRGB(r, g, b);
//            break;
//          case 32:
//            ((uint32_t *)renderSurface->p)[index] = fpRGB(r, g, b);
//            break;
//        };
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
CSoftGLESFixed::hlineTa(CEdgeFx & from, CEdgeFx & to, GLint y)
{
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_TA();
    GLuint index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
//      switch(renderSurface->bpp_)
//      {
//        case 8:
//          ((uint8_t  *)renderSurface->p)[index] = ((uint8_t  *)CURRENT_TEX->data)[(((int32_t)tt & CURRENT_TEX->maskHeight) * CURRENT_TEX->width) + ((int32_t)ts & CURRENT_TEX->maskWidth)];
//          break;
//        case 16:
          ((uint16_t *)renderSurface->p)[index] = ((uint16_t *)CURRENT_TEX->data)[(((int32_t)tt & CURRENT_TEX->maskHeight) * CURRENT_TEX->width) + ((int32_t)ts & CURRENT_TEX->maskWidth)];
//          break;
//        case 32:
//          ((uint32_t *)renderSurface->p)[index] = ((uint32_t *)CURRENT_TEX->data)[(((int32_t)tt & CURRENT_TEX->maskHeight) * CURRENT_TEX->width) + ((int32_t)ts & CURRENT_TEX->maskWidth)];
//          break;
//      };
      SL_INCREMENT_TA();
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test, affine texture mapped
inline void
CSoftGLESFixed::hlineZTa(CEdgeFx & from, CEdgeFx & to, GLint y)
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
      if(testAndSetDepth(z, index) == true)
      {
//        switch(renderSurface->bpp_)
//        {
//          case 8:
//            ((uint8_t  *)renderSurface->p)[index] = ((uint8_t  *)CURRENT_TEX->data)[(((int32_t)tt & CURRENT_TEX->maskHeight) * CURRENT_TEX->width) + ((int32_t)ts & CURRENT_TEX->maskWidth)];
//            break;
//          case 16:
            ((uint16_t *)renderSurface->p)[index] = ((uint16_t *)CURRENT_TEX->data)[(((int32_t)tt & CURRENT_TEX->maskHeight) * CURRENT_TEX->width) + ((int32_t)ts & CURRENT_TEX->maskWidth)];
//            break;
//          case 32:
//            ((uint32_t *)renderSurface->p)[index] = ((uint32_t *)CURRENT_TEX->data)[(((int32_t)tt & CURRENT_TEX->maskHeight) * CURRENT_TEX->width) + ((int32_t)ts & CURRENT_TEX->maskWidth)];
//            break;
//        };
      }
      SL_INCREMENT_Z();
      SL_INCREMENT_TA();
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
  GLint xwidth = SL_LINE_WIDTH();

  if(xwidth > 0)
  {
    SL_INTERPOLATE_COMMON();
    SL_INTERPOLATE_Z();
    SL_INTERPOLATE_TP();
    GLuint index = SL_PIXEL_INDEX();

    while(xwidth--)
    {
      if(testAndSetDepth(z, index) == true)
      {
        GLfixed recip = gl_fpdiv(gl_fpfromi(1), tz);
        GLfixed s     = gl_fpmul(ts, recip);
        GLfixed t     = gl_fpmul(tt, recip);
//        switch(renderSurface->bpp_)
//        {
//          case 8:
//            ((uint8_t  *)renderSurface->p)[index] = ((uint8_t  *)CURRENT_TEX->data)[((gl_fptoi(t) & CURRENT_TEX->maskHeight) * CURRENT_TEX->width) + (gl_fptoi(s) & CURRENT_TEX->maskWidth)];
//            break;
//          case 16:
            ((uint16_t *)renderSurface->p)[index] = ((uint16_t *)CURRENT_TEX->data)[((gl_fptoi(t) & CURRENT_TEX->maskHeight) * CURRENT_TEX->width) + (gl_fptoi(s) & CURRENT_TEX->maskWidth)];
//            break;
//          case 32:
//            ((uint32_t *)renderSurface->p)[index] = ((uint32_t *)CURRENT_TEX->data)[((gl_fptoi(t) & CURRENT_TEX->maskHeight) * CURRENT_TEX->width) + (gl_fptoi(s) & CURRENT_TEX->maskWidth)];
//            break;
//        };
      }
      SL_INCREMENT_Z();
      SL_INCREMENT_TP();
      index++;
    }
  }
}
*/
