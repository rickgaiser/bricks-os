#include "softGLFx.h"
#include "matrix.h"
#include "fixedPoint.h"

#include "stdlib.h"
typedef unsigned int wint_t;
#include <math.h>


#define fpRGB(r,g,b) (0x8000 | \
                      (((b*255) >>  9) & 0x7c00) | \
                      (((g*255) >> 14) & 0x03e0) | \
                      (((r*255) >> 19) & 0x001f))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoftGLESFixed::CSoftGLESFixed()
 : CAGLESFloatToFxContext()
 , CAGLESBuffers()
 , CAGLESMatrixFx()

 , renderSurface(0)
 , depthTestEnabled_(false)
 , depthFunction_(GL_LESS)
 , depthClear_(gl_fpfromi(1))
 , zClearValue_(0xffff)
 , zbuffer(0)
 , shadingModel_(GL_FLAT)
 , cullFaceEnabled_(false)
 , bCullBack_(true)
 , cullFaceMode_(GL_BACK)
 , lightingEnabled_(false)
 , fogEnabled_(false)
 , edge1(0)
 , edge2(0)
 , viewportXOffset(0)
 , viewportYOffset(0)
 , viewportPixelCount(0)
 , viewportWidth(0)
 , viewportHeight(0)
{
  clCurrent.r = gl_fpfromi(0);
  clCurrent.g = gl_fpfromi(0);
  clCurrent.b = gl_fpfromi(0);
  clCurrent.a = gl_fpfromi(1);

  clClear.r = gl_fpfromi(0);
  clClear.g = gl_fpfromi(0);
  clClear.b = gl_fpfromi(0);
  clClear.a = gl_fpfromi(1);

  for(int iLight(0); iLight < 8; iLight++)
  {
    lights_[iLight].ambient.r = gl_fpfromi(0);
    lights_[iLight].ambient.g = gl_fpfromi(0);
    lights_[iLight].ambient.b = gl_fpfromi(0);
    lights_[iLight].ambient.a = gl_fpfromi(1);

    lights_[iLight].diffuse.r = gl_fpfromi(0);
    lights_[iLight].diffuse.g = gl_fpfromi(0);
    lights_[iLight].diffuse.b = gl_fpfromi(0);
    lights_[iLight].diffuse.a = gl_fpfromi(1);

    lights_[iLight].specular.r = gl_fpfromi(0);
    lights_[iLight].specular.g = gl_fpfromi(0);
    lights_[iLight].specular.b = gl_fpfromi(0);
    lights_[iLight].specular.a = gl_fpfromi(1);

    lights_[iLight].enabled = false;
  }

  zLoss_ = 0;
  GLfixed zmax = zFar_ - zNear_;
  uint16_t newz = zmax;
  if(newz != zmax)
  {
    // We need to lose some depth precision :-(
    for(zLoss_ = 1; zLoss_ < 32; zLoss_++)
    {
      newz = zmax >> zLoss_;
      if(newz == (zmax >> zLoss_))
        break;
    }
  }
}

//-----------------------------------------------------------------------------
CSoftGLESFixed::~CSoftGLESFixed()
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::setSurface(CSurface * surface)
{
  renderSurface = surface;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glClear(GLbitfield mask)
{
  if(mask & GL_COLOR_BUFFER_BIT)
  {
    color_t color(fpRGB(clClear.r, clClear.g, clClear.b));

    for(int i(0); i < viewportPixelCount; i++)
      ((uint16_t *)renderSurface->p)[i] = color;
  }
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    for(int i(0); i < viewportPixelCount; i++)
      zbuffer[i] = zClearValue_;
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  clClear.r = clampfx(red);
  clClear.g = clampfx(green);
  clClear.b = clampfx(blue);
  clClear.a = clampfx(alpha);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glClearDepthx(GLclampx depth)
{
  depthClear_ = clampfx(depth);
  zClearValue_ = gl_fptoi(depthClear_ * 0xffff);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  clCurrent.r = gl_fpfromi(red  ) / 255;
  clCurrent.g = gl_fpfromi(green) / 255;
  clCurrent.b = gl_fpfromi(blue ) / 255;
  clCurrent.a = gl_fpfromi(alpha) / 255;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  clCurrent.r = red;
  clCurrent.g = green;
  clCurrent.b = blue;
  clCurrent.a = alpha;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  normal_[0] = nx;
  normal_[1] = ny;
  normal_[2] = nz;
  normal_[3] = gl_fpfromi(1);

  //if((enableCapabilities_ & GL_NORMALIZE) == GL_NORMALIZE)
  //{
  //  // FIXME: Normalize normal
  //}
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glCullFace(GLenum mode)
{
  cullFaceMode_ = mode;
  bCullBack_ = (cullFaceMode_ == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glDepthFunc(GLenum func)
{
  depthFunction_ = func;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glDisable(GLenum cap)
{
  switch(cap)
  {
    case GL_LIGHTING: lightingEnabled_ = false; break;
    case GL_LIGHT0: lights_[0].enabled = false; break;
    case GL_LIGHT1: lights_[1].enabled = false; break;
    case GL_LIGHT2: lights_[2].enabled = false; break;
    case GL_LIGHT3: lights_[3].enabled = false; break;
    case GL_LIGHT4: lights_[4].enabled = false; break;
    case GL_LIGHT5: lights_[5].enabled = false; break;
    case GL_LIGHT6: lights_[6].enabled = false; break;
    case GL_LIGHT7: lights_[7].enabled = false; break;

    case GL_DEPTH_TEST: depthTestEnabled_ = false; break;
    case GL_CULL_FACE:  cullFaceEnabled_  = false; break;
    case GL_FOG:        fogEnabled_ = false; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex(first * bufVertex_.size);
  GLint idxColor (first * bufColor_.size);
  GLint idxNormal(first * bufNormal_.size);

  SVertexFx v;
  v.bProcessed = false;

  // Reset vertex count for strips/fans/...
  iVCount_ = 0;

  // Process all vertices
  for(GLint i(0); i < count; i++)
  {
    // Vertex
    switch(bufVertex_.type)
    {
      case GL_FLOAT:
        v.v[0] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v[1] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v[2] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v[3] = gl_fpfromi(1);
        break;
      case GL_FIXED:
        v.v[0] = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v[1] = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v[2] = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v[3] = gl_fpfromi(1);
        break;
    };

    // Normal
    if(bBufNormalEnabled_ == true)
    {
      switch(bufColor_.type)
      {
        case GL_FLOAT:
          v.n[0] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n[1] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n[2] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n[3] = gl_fpfromi(1);
          break;
        case GL_FIXED:
          v.n[0] = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n[1] = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n[2] = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n[3] = gl_fpfromi(1);
          break;
      };
    }

    // Color
    if(bBufColorEnabled_ == true)
    {
      switch(bufColor_.type)
      {
        case GL_FLOAT:
          v.c.r = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v.c.g = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v.c.b = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v.c.a = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          break;
        case GL_FIXED:
          v.c.r = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.c.g = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.c.b = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.c.a = ((GLfixed *)bufColor_.pointer)[idxColor++];
          break;
      };
    }

    switch(mode)
    {
      case GL_TRIANGLES:      addVertexToTriangle(v);      break;
      case GL_TRIANGLE_STRIP: addVertexToTriangleStrip(v); break;
      case GL_TRIANGLE_FAN:   addVertexToTriangleFan(v);   break;
    };
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glEnable(GLenum cap)
{
  switch(cap)
  {
    case GL_LIGHTING:   lightingEnabled_   = true; break;
    case GL_LIGHT0:     lights_[0].enabled = true; break;
    case GL_LIGHT1:     lights_[1].enabled = true; break;
    case GL_LIGHT2:     lights_[2].enabled = true; break;
    case GL_LIGHT3:     lights_[3].enabled = true; break;
    case GL_LIGHT4:     lights_[4].enabled = true; break;
    case GL_LIGHT5:     lights_[5].enabled = true; break;
    case GL_LIGHT6:     lights_[6].enabled = true; break;
    case GL_LIGHT7:     lights_[7].enabled = true; break;

    case GL_DEPTH_TEST: depthTestEnabled_  = true; break;
    case GL_CULL_FACE:  cullFaceEnabled_   = true; break;
    case GL_FOG:        fogEnabled_        = true; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glFinish(void)
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glFlush(void)
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glFogx(GLenum pname, GLfixed param)
{
  switch(pname)
  {
    case GL_FOG_DENSITY: fogDensity_ = param; break;
    case GL_FOG_START:   fogStart_   = param; break;
    case GL_FOG_END:     fogEnd_     = param; break;
    case GL_FOG_MODE:                         break;
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glFogxv(GLenum pname, const GLfixed * params)
{
  switch(pname)
  {
    case GL_FOG_COLOR:
      fogColor_.r = params[0];
      fogColor_.g = params[1];
      fogColor_.b = params[2];
      fogColor_.a = params[3];
      break;
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glLightx(GLenum light, GLenum pname, GLfixed param)
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
  SLightFx * pLight = 0;
  switch(light)
  {
    case GL_LIGHT0: pLight = &lights_[0]; break;
    case GL_LIGHT1: pLight = &lights_[1]; break;
    case GL_LIGHT2: pLight = &lights_[2]; break;
    case GL_LIGHT3: pLight = &lights_[3]; break;
    case GL_LIGHT4: pLight = &lights_[4]; break;
    case GL_LIGHT5: pLight = &lights_[5]; break;
    case GL_LIGHT6: pLight = &lights_[6]; break;
    case GL_LIGHT7: pLight = &lights_[7]; break;
    default:
      return;
  }

  SColorFx * pColor = 0;
  switch(pname)
  {
    case GL_AMBIENT:  pColor = &pLight->ambient; break;
    case GL_DIFFUSE:  pColor = &pLight->diffuse; break;
    case GL_SPECULAR: pColor = &pLight->specular; break;
    default:
      return;
  }

  pColor->r = params[0];
  pColor->g = params[1];
  pColor->b = params[2];
  pColor->a = params[3];
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(zbuffer)
    delete zbuffer;
  if(edge1)
    delete edge1;
  if(edge2)
    delete edge2;

  viewportXOffset    = x;
  viewportYOffset    = y;
  viewportWidth      = width;
  viewportHeight     = height;
  viewportPixelCount = width * height;
  zbuffer            = new uint16_t[width * height];
  edge1              = new CEdgeFx(viewportHeight);
  edge2              = new CEdgeFx(viewportHeight);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool
CSoftGLESFixed::testAndSetDepth(GLfixed z, uint32_t index)
{
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
  return false;
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, flat colors
void
CSoftGLESFixed::hline(CEdgeFx & from, CEdgeFx & to, GLint & y, SColorFx c)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLfixed mz((to.z_[y] - from.z_[y]) / dx);
    GLfixed z(from.z_[y]);
    color_t color(fpRGB(c.r, c.g, c.b));

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        if((depthTestEnabled_ == false) || (testAndSetDepth(z, index) == true))
          ((uint16_t *)renderSurface->p)[index] = color;
      }
      if(depthTestEnabled_ == true)
        z += mz;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors
void
CSoftGLESFixed::hline_s(CEdgeFx & from, CEdgeFx & to, GLint & y)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLfixed mz((to.z_[y] - from.z_[y]) / dx);
    GLfixed z(from.z_[y]);

    GLfixed mr((to.c_[y].r - from.c_[y].r) / dx);
    GLfixed mg((to.c_[y].g - from.c_[y].g) / dx);
    GLfixed mb((to.c_[y].b - from.c_[y].b) / dx);
    GLfixed r(from.c_[y].r);
    GLfixed g(from.c_[y].g);
    GLfixed b(from.c_[y].b);

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        if((depthTestEnabled_ == false) || (testAndSetDepth(z, index) == true))
          ((uint16_t *)renderSurface->p)[index] = fpRGB(r, g, b);
      }
      if(depthTestEnabled_ == true)
        z += mz;
      r += mr;
      g += mg;
      b += mb;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::addVertexToTriangle(SVertexFx & v)
{
  static SPolygonFx polygon;
  static SVertexFx vertices[3];
  static bool bInitialized(false);
  if(bInitialized == false)
  {
    polygon.v[0] = &vertices[0];
    polygon.v[1] = &vertices[1];
    polygon.v[2] = &vertices[2];
    bInitialized = true;
  }

  polygon.v[iVCount_]->bProcessed = false;
  // Copy vertex
  polygon.v[iVCount_]->v[0] = v.v[0];
  polygon.v[iVCount_]->v[1] = v.v[1];
  polygon.v[iVCount_]->v[2] = v.v[2];
  polygon.v[iVCount_]->v[3] = v.v[3];
  polygon.v[iVCount_]->n[0] = v.n[0];
  polygon.v[iVCount_]->n[1] = v.n[1];
  polygon.v[iVCount_]->n[2] = v.n[2];
  polygon.v[iVCount_]->n[3] = v.n[3];
  polygon.v[iVCount_]->c    = v.c;

  if(iVCount_ == 2)
    plotPoly(polygon);

  iVCount_ = (iVCount_ + 1) % 3;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::addVertexToTriangleStrip(SVertexFx & v)
{
  static SPolygonFx polygon;
  static SVertexFx vertices[3];
  static bool bInitialized(false);
  static bool bFlipFlop(true);
  if(bInitialized == false)
  {
    polygon.v[0] = &vertices[0];
    polygon.v[1] = &vertices[1];
    polygon.v[2] = &vertices[2];
    bInitialized = true;
  }

  polygon.v[iVCount_]->bProcessed = false;
  // Copy vertex
  polygon.v[iVCount_]->v[0] = v.v[0];
  polygon.v[iVCount_]->v[1] = v.v[1];
  polygon.v[iVCount_]->v[2] = v.v[2];
  polygon.v[iVCount_]->v[3] = v.v[3];
  polygon.v[iVCount_]->n[0] = v.n[0];
  polygon.v[iVCount_]->n[1] = v.n[1];
  polygon.v[iVCount_]->n[2] = v.n[2];
  polygon.v[iVCount_]->n[3] = v.n[3];
  polygon.v[iVCount_]->c    = v.c;

  if(iVCount_ == 2)
  {
    plotPoly(polygon);

    if(bFlipFlop == true)
    {
      SVertexFx * pTemp = polygon.v[0];
      polygon.v[0] = polygon.v[2];
      polygon.v[2] = pTemp;
    }
    else
    {
      SVertexFx * pTemp = polygon.v[1];
      polygon.v[1] = polygon.v[2];
      polygon.v[2] = pTemp;
    }
  }
  else
    iVCount_++;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::addVertexToTriangleFan(SVertexFx & v)
{
  static SPolygonFx polygon;
  static SVertexFx vertices[3];
  static bool bInitialized(false);
  if(bInitialized == false)
  {
    polygon.v[0] = &vertices[0];
    polygon.v[1] = &vertices[1];
    polygon.v[2] = &vertices[2];
    bInitialized = true;
  }

  polygon.v[iVCount_]->bProcessed = false;
  // Copy vertex
  polygon.v[iVCount_]->v[0] = v.v[0];
  polygon.v[iVCount_]->v[1] = v.v[1];
  polygon.v[iVCount_]->v[2] = v.v[2];
  polygon.v[iVCount_]->v[3] = v.v[3];
  polygon.v[iVCount_]->n[0] = v.n[0];
  polygon.v[iVCount_]->n[1] = v.n[1];
  polygon.v[iVCount_]->n[2] = v.n[2];
  polygon.v[iVCount_]->n[3] = v.n[3];
  polygon.v[iVCount_]->c    = v.c;

  if(iVCount_ == 2)
  {
    plotPoly(polygon);

    // Swap 3rd and 2nd vertex
    if(polygon.v[1] == &vertices[1])
    {
      polygon.v[1] = &vertices[2];
      polygon.v[2] = &vertices[1];
    }
    else
    {
      polygon.v[1] = &vertices[1];
      polygon.v[2] = &vertices[2];
    }
  }
  else
    iVCount_++;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::plotPoly(SPolygonFx & poly)
{
  for(int i(0); i < 3; i++)
  {
    if(poly.v[i]->bProcessed == false)
    {
      // ModelView Transformation
      matrixModelView.transform(poly.v[i]->v, poly.v[i]->v);
      // Projection Transformation
      matrixProjection.transform(poly.v[i]->v, poly.v[i]->v);

      // Divide x and y by linear depth: w
      poly.v[i]->v[0] = gl_fpdiv(poly.v[i]->v[0], poly.v[i]->v[3]);
      poly.v[i]->v[1] = gl_fpdiv(poly.v[i]->v[1], poly.v[i]->v[3]);

      // From normalized device coordinates to window coordinates
      poly.v[i]->sx = gl_fptoi(gl_fpmul(( poly.v[i]->v[0] + gl_fpfromi(1)), gl_fpfromi(viewportWidth  / 2))) + viewportXOffset;
      poly.v[i]->sy = gl_fptoi(gl_fpmul((-poly.v[i]->v[1] + gl_fpfromi(1)), gl_fpfromi(viewportHeight / 2))) + viewportYOffset;

      poly.v[i]->bProcessed = true;
    }
  }

  // Backface culling
  if(cullFaceEnabled_ == true)
  {
    // Always invisible when culling both front and back
    if(cullFaceMode_ == GL_FRONT_AND_BACK)
      return;

    // Figure out if we need to cull
    if((poly.v[1]->sx != poly.v[0]->sx) && (poly.v[2]->sx != poly.v[0]->sx))
    {
      if(((((gl_fpfromi(poly.v[1]->sy - poly.v[0]->sy) / (poly.v[1]->sx - poly.v[0]->sx)) - (gl_fpfromi(poly.v[2]->sy - poly.v[0]->sy) / (poly.v[2]->sx - poly.v[0]->sx))) < 0) ^ ((poly.v[0]->sx <= poly.v[1]->sx) == (poly.v[0]->sx > poly.v[2]->sx))) == bCullBack_)
        return;
    }
    else if((poly.v[2]->sx != poly.v[1]->sx) && (poly.v[0]->sx != poly.v[1]->sx))
    {
      if(((((gl_fpfromi(poly.v[2]->sy - poly.v[1]->sy) / (poly.v[2]->sx - poly.v[1]->sx)) - (gl_fpfromi(poly.v[0]->sy - poly.v[1]->sy) / (poly.v[0]->sx - poly.v[1]->sx))) < 0) ^ ((poly.v[1]->sx <= poly.v[2]->sx) == (poly.v[1]->sx > poly.v[0]->sx))) == bCullBack_)
        return;
    }
    else if((poly.v[0]->sx != poly.v[2]->sx) && (poly.v[1]->sx != poly.v[2]->sx))
    {
      if(((((gl_fpfromi(poly.v[0]->sy - poly.v[2]->sy) / (poly.v[0]->sx - poly.v[2]->sx)) - (gl_fpfromi(poly.v[1]->sy - poly.v[2]->sy) / (poly.v[1]->sx - poly.v[2]->sx))) < 0) ^ ((poly.v[2]->sx <= poly.v[0]->sx) == (poly.v[2]->sx > poly.v[1]->sx))) == bCullBack_)
        return;
    }
    else
      return; // Triangle invisible
  }

  // Lighting
  if(lightingEnabled_ == true)
  {
    // Normal Rotation
    matrixRotation.transform(poly.v[0]->n, poly.v[0]->n);
    matrixRotation.transform(poly.v[1]->n, poly.v[1]->n);
    matrixRotation.transform(poly.v[2]->n, poly.v[2]->n);
    // FIXME: Light value of normal
    GLfixed normal[3] = {abs(poly.v[0]->n[2]), abs(poly.v[1]->n[2]), abs(poly.v[2]->n[2])};

    for(int iLight(0); iLight < 8; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        SColorFx & ambient = lights_[iLight].ambient;
        SColorFx & diffuse = lights_[iLight].diffuse;

        poly.v[0]->c.r = clampfx(gl_fpmul(poly.v[0]->c.r, ambient.r) + gl_fpmul(gl_fpmul(poly.v[0]->c.r, normal[0]), diffuse.r));
        poly.v[0]->c.g = clampfx(gl_fpmul(poly.v[0]->c.g, ambient.g) + gl_fpmul(gl_fpmul(poly.v[0]->c.g, normal[0]), diffuse.g));
        poly.v[0]->c.b = clampfx(gl_fpmul(poly.v[0]->c.b, ambient.b) + gl_fpmul(gl_fpmul(poly.v[0]->c.b, normal[0]), diffuse.b));

        poly.v[1]->c.r = clampfx(gl_fpmul(poly.v[1]->c.r, ambient.r) + gl_fpmul(gl_fpmul(poly.v[1]->c.r, normal[1]), diffuse.r));
        poly.v[1]->c.g = clampfx(gl_fpmul(poly.v[1]->c.g, ambient.g) + gl_fpmul(gl_fpmul(poly.v[1]->c.g, normal[1]), diffuse.g));
        poly.v[1]->c.b = clampfx(gl_fpmul(poly.v[1]->c.b, ambient.b) + gl_fpmul(gl_fpmul(poly.v[1]->c.b, normal[1]), diffuse.b));

        poly.v[2]->c.r = clampfx(gl_fpmul(poly.v[2]->c.r, ambient.r) + gl_fpmul(gl_fpmul(poly.v[2]->c.r, normal[2]), diffuse.r));
        poly.v[2]->c.g = clampfx(gl_fpmul(poly.v[2]->c.g, ambient.g) + gl_fpmul(gl_fpmul(poly.v[2]->c.g, normal[2]), diffuse.g));
        poly.v[2]->c.b = clampfx(gl_fpmul(poly.v[2]->c.b, ambient.b) + gl_fpmul(gl_fpmul(poly.v[2]->c.b, normal[2]), diffuse.b));
      }
    }
  }

  // Fog
  if(fogEnabled_ == true)
  {
    for(int i(0); i < 3; i++)
    {
      GLfixed partFog   = clampfx(gl_fpdiv(abs(poly.v[i]->v[2]) - fogStart_, fogEnd_ - fogStart_));
      GLfixed partColor = gl_fpfromi(1) - partFog;
      poly.v[i]->c.r = clampfx(gl_fpmul(poly.v[i]->c.r, partColor) + gl_fpmul(fogColor_.r, partFog));
      poly.v[i]->c.g = clampfx(gl_fpmul(poly.v[i]->c.g, partColor) + gl_fpmul(fogColor_.g, partFog));
      poly.v[i]->c.b = clampfx(gl_fpmul(poly.v[i]->c.b, partColor) + gl_fpmul(fogColor_.b, partFog));
    }
  }

  rasterPoly(poly);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::rasterPoly(SPolygonFx & poly)
{
  // Bubble sort the 3 vertexes
  SVertexFx * vtemp;
  SVertexFx * vhi(poly.v[0]);
  SVertexFx * vmi(poly.v[1]);
  SVertexFx * vlo(poly.v[2]);

  // Swap bottom with middle?
  if(vlo->sy > vmi->sy)
  {
    vtemp = vmi;
    vmi   = vlo;
    vlo   = vtemp;
  }
  // Swap middle with top?
  if(vmi->sy > vhi->sy)
  {
    vtemp = vhi;
    vhi   = vmi;
    vmi   = vtemp;
    // Swap bottom with middle again?
    if(vlo->sy > vmi->sy)
    {
      vtemp = vmi;
      vmi   = vlo;
      vlo   = vtemp;
    }
  }

  // Create edge lists
  if(depthTestEnabled_ == true)
  {
    if(shadingModel_ == GL_SMOOTH)
    {
      edge1->addZC(vlo->sx, vlo->sy, vlo->v[3], vlo->c, vhi->sx, vhi->sy, vhi->v[3], vhi->c);
      edge2->addZC(vlo->sx, vlo->sy, vlo->v[3], vlo->c, vmi->sx, vmi->sy, vmi->v[3], vmi->c);
      edge2->addZC(vmi->sx, vmi->sy, vmi->v[3], vmi->c, vhi->sx, vhi->sy, vhi->v[3], vhi->c);
    }
    else
    {
      edge1->addZ(vlo->sx, vlo->sy, vlo->v[3], vhi->sx, vhi->sy, vhi->v[3]);
      edge2->addZ(vlo->sx, vlo->sy, vlo->v[3], vmi->sx, vmi->sy, vmi->v[3]);
      edge2->addZ(vmi->sx, vmi->sy, vmi->v[3], vhi->sx, vhi->sy, vhi->v[3]);
    }
  }
  else
  {
    if(shadingModel_ == GL_SMOOTH)
    {
      edge1->addC(vlo->sx, vlo->sy, vlo->c, vhi->sx, vhi->sy, vhi->c);
      edge2->addC(vlo->sx, vlo->sy, vlo->c, vmi->sx, vmi->sy, vmi->c);
      edge2->addC(vmi->sx, vmi->sy, vmi->c, vhi->sx, vhi->sy, vhi->c);
    }
    else
    {
      edge1->add(vlo->sx, vlo->sy, vhi->sx, vhi->sy);
      edge2->add(vlo->sx, vlo->sy, vmi->sx, vmi->sy);
      edge2->add(vmi->sx, vmi->sy, vhi->sx, vhi->sy);
    }
  }

  CEdgeFx * pEdgeLeft  = edge1;
  CEdgeFx * pEdgeRight = edge2;
  GLint my(vlo->sy + ((vhi->sy - vlo->sy)/2));
  if(edge1->x_[my] > edge2->x_[my])
  {
    // Swap
    pEdgeLeft  = edge2;
    pEdgeRight = edge1;
  }

  // Display triangle (horizontal lines forming the triangle)
  switch(shadingModel_)
  {
    case GL_FLAT:
    {
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hline(*pEdgeLeft, *pEdgeRight, y, poly.v[2]->c);
      break;
    }
    case GL_SMOOTH:
    {
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hline_s(*pEdgeLeft, *pEdgeRight, y);
      break;
    }
  }
}
