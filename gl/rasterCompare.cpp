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

#include "rasterCompare.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"


namespace raster
{


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CRasterizerCompare::CRasterizerCompare(IRasterizer * pTest, IRasterizer * pRef)
 : pTest_(pTest)
 , pRef_(pRef)
{
}

//-----------------------------------------------------------------------------
CRasterizerCompare::~CRasterizerCompare()
{
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::setSurface(CSurface * surface)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  renderSurface = surface;

  CSurface * pTestSurface = new CSurface;
  CSurface * pRefSurface  = new CSurface;

  pTestSurface->mode = surface->mode;
  pRefSurface->mode  = surface->mode;

  int iPixelCount = surface->width() * surface->height();
  pTestSurface->p = new uint32_t[iPixelCount];
  pRefSurface->p  = new uint32_t[iPixelCount];

  pTest_->setSurface(pTestSurface);
  pRef_->setSurface(pRefSurface);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::compareRasterTest(IRasterizer * pTest)
{
  pTest_ = pTest;
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::compareRasterRef(IRasterizer * pRef)
{
  pRef_ = pRef;
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::enableDepthTest(bool enable)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->enableDepthTest(enable);
  pRef_->enableDepthTest(enable);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::enableSmoothShading(bool enable)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->enableSmoothShading(enable);
  pRef_->enableSmoothShading(enable);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::enableTextures(bool enable)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->enableTextures(enable);
  pRef_->enableTextures(enable);
}
//-----------------------------------------------------------------------------
void
CRasterizerCompare::enableBlending(bool enable)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->enableBlending(enable);
  pRef_->enableBlending(enable);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::enableAlphaTest(bool enable)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->enableAlphaTest(enable);
  pRef_->enableAlphaTest(enable);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::clearDepthf(GLclampf depth)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->clearDepthf(depth);
  pRef_->clearDepthf(depth);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::depthRange(GLclampf zNear, GLclampf zFar)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->depthRange(zNear, zFar);
  pRef_->depthRange(zNear, zFar);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::depthFunc(GLenum func)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->depthFunc(func);
  pRef_->depthFunc(func);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::depthMask(GLboolean flag)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->depthMask(flag);
  pRef_->depthMask(flag);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::bindTexture(GLenum target, GLuint texture)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->bindTexture(target, texture);
  pRef_->bindTexture(target, texture);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::deleteTextures(GLsizei n, const GLuint *textures)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->deleteTextures(n, textures);
  pRef_->deleteTextures(n, textures);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::genTextures(GLsizei n, GLuint *textures)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->genTextures(n, textures);
  pRef_->genTextures(n, textures);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->texImage2D(target, level, internalformat, width, height, border, format, type, pixels);
  pRef_->texImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::texParameterf(GLenum target, GLenum pname, GLfloat param)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->texParameterf(target, pname, param);
  pRef_->texParameterf(target, pname, param);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::texEnvf(GLenum target, GLenum pname, GLfloat param)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->texEnvf(target, pname, param);
  pRef_->texEnvf(target, pname, param);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::texEnvfv(GLenum target, GLenum pname, const GLfloat * params)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->texEnvfv(target, pname, params);
  pRef_->texEnvfv(target, pname, params);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::colorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->colorTable(target, internalformat, width, format, type, table);
  pRef_->colorTable(target, internalformat, width, format, type, table);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::alphaFunc(GLenum func, GLclampf ref)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->alphaFunc(func, ref);
  pRef_->alphaFunc(func, ref);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::blendFunc(GLenum sfactor, GLenum dfactor)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->blendFunc(sfactor, dfactor);
  pRef_->blendFunc(sfactor, dfactor);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->clearColor(red, green, blue, alpha);
  pRef_->clearColor(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::clear(GLbitfield mask)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->clear(mask);
  pRef_->clear(mask);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->viewport(x, y, width, height);
  pRef_->viewport(x, y, width, height);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::setUsePixelCenter(bool bCenter)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->setUsePixelCenter(bCenter);
  pRef_->setUsePixelCenter(bCenter);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::begin(GLenum mode)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->begin(mode);
  pRef_->begin(mode);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::end()
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->end();
  pRef_->end();
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2)
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->rasterTriangle(v0, v1, v2);
  pRef_->rasterTriangle(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CRasterizerCompare::flush()
{
  if((pTest_ == NULL) || (pRef_ == NULL))
    return;

  pTest_->flush();
  pRef_->flush();

  CSurface * pTestSurface = pTest_->getSurface();
  CSurface * pRefSurface  = pRef_->getSurface();
  CSurface * pThisSurface = this->getSurface();

  uint32_t * pTestData = (uint32_t *)pTestSurface->p;
  uint32_t * pRefData  = (uint32_t *)pRefSurface->p;
  uint32_t * pThisData = (uint32_t *)pThisSurface->p;

  bError_ = false;
  int iPixelCount = pThisSurface->width() * pThisSurface->height();
  for(int i(0); i < iPixelCount; i++)
  {
    if(*pTestData == *pRefData)
    {
      // Display test color
      //*pThisData = *pTestData;
      *pThisData = COLOR_BLACK;
    }
    else
    {
      //printf("CRasterizerCompare: Difference at x=%d, y=%d\n", i % pThisSurface->width(), i / pThisSurface->width());

      bError_ = true;
      if(*pRefData == COLOR_BLACK)
      {
        // Test should not have written that pixel
        *pThisData = COLOR_H_RED;
      }
      else
      {
        // Test should have written that pixel
        *pThisData = COLOR_H_GREEN;
      }
    }

    pTestData++;
    pRefData++;
    pThisData++;
  }
}


};
