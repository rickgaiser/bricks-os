#include "softGLF.h"
#include "matrix.h"
#include "fixedPoint.h"

#include "stdlib.h"
typedef unsigned int wint_t;
#include <math.h>


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoftGLESFloat::CSoftGLESFloat()
 : CAGLESBuffers()
 , renderSurface(0)
 , depthTestEnabled_(false)
 , depthFunction_(GL_LESS)
 , depthClear_(1.0f)
 , zClearValue_(0xffffffff)
 , zbuffer(0)
 , shadingModel_(GL_FLAT)
 , cullFaceEnabled_(false)
 , bCullBack_(true)
 , cullFaceMode_(GL_BACK)
 , matrixMode_(GL_MODELVIEW)
 , pCurrentMatrix_(&matrixModelView)
 , lightingEnabled_(false)
 , fogEnabled_(false)
 , edge1(0)
 , edge2(0)
 , viewportXOffset(0)
 , viewportYOffset(0)
 , viewportPixelCount(0)
 , viewportByteCount(0)
 , viewportWidth(0)
 , viewportHeight(0)
 , fpFieldofviewXScalar(1.0f)
 , fpFieldofviewYScalar(1.0f)
{
  clCurrent.r = 0.0f;
  clCurrent.g = 0.0f;
  clCurrent.b = 0.0f;
  clCurrent.a = 1.0f;

  clClear.r = 0.0f;
  clClear.g = 0.0f;
  clClear.b = 0.0f;
  clClear.a = 1.0f;

  for(int iLight(0); iLight < 8; iLight++)
  {
    lights_[iLight].ambient.r = 0.0f;
    lights_[iLight].ambient.g = 0.0f;
    lights_[iLight].ambient.b = 0.0f;
    lights_[iLight].ambient.a = 1.0f;

    lights_[iLight].diffuse.r = 0.0f;
    lights_[iLight].diffuse.g = 0.0f;
    lights_[iLight].diffuse.b = 0.0f;
    lights_[iLight].diffuse.a = 1.0f;

    lights_[iLight].specular.r = 0.0f;
    lights_[iLight].specular.g = 0.0f;
    lights_[iLight].specular.b = 0.0f;
    lights_[iLight].specular.a = 1.0f;

    lights_[iLight].enabled = false;
  }

  zFar_  = 100.0f;
  zNear_ =   2.0f;
  zA_    = zFar_ / (zFar_ - zNear_);
  zB_    = (zFar_ * zNear_) / (zNear_ - zFar_);
}

//-----------------------------------------------------------------------------
CSoftGLESFloat::~CSoftGLESFloat()
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::setSurface(CSurface * surface)
{
  renderSurface = surface;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glClear(GLbitfield mask)
{
  long iCount(viewportByteCount >> 1);

  if(mask & GL_COLOR_BUFFER_BIT)
  {
    color_t color = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(clClear.r * 255), (uint8_t)(clClear.g * 255), (uint8_t)(clClear.b * 255), 255);

    for(int i(0); i < iCount; i++)
      ((uint32_t *)renderSurface->p)[i] = color;
  }
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    for(int i(0); i < iCount; i++)
      zbuffer[i] = zClearValue_;
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  clClear.r = clampf(red);
  clClear.g = clampf(green);
  clClear.b = clampf(blue);
  clClear.a = clampf(alpha);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glClearDepthf(GLclampf depth)
{
  depthClear_ = clampf(depth);
  zClearValue_ = (uint32_t)(depthClear_ * 0xffffffff);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  clCurrent.r = (GLfloat)red   / 255.0f;
  clCurrent.g = (GLfloat)green / 255.0f;
  clCurrent.b = (GLfloat)blue  / 255.0f;
  clCurrent.a = (GLfloat)alpha / 255.0f;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  clCurrent.r = red;
  clCurrent.g = green;
  clCurrent.b = blue;
  clCurrent.a = alpha;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
  normal_[0] = nx;
  normal_[1] = ny;
  normal_[2] = nz;
  normal_[3] = 1.0f;

  //if((enableCapabilities_ & GL_NORMALIZE) == GL_NORMALIZE)
  //{
  //  // FIXME: Normalize normal
  //}
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glCullFace(GLenum mode)
{
  cullFaceMode_ = mode;
  bCullBack_ = (cullFaceMode_ == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glDepthFunc(GLenum func)
{
  depthFunction_ = func;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glDisable(GLenum cap)
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
CSoftGLESFloat::addVertexToTriangle(SVertexF & v)
{
  static SPolygonF polygon;
  static SVertexF vertices[3];
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
  polygon.v[iVCount_]->v1[0] = v.v1[0];
  polygon.v[iVCount_]->v1[1] = v.v1[1];
  polygon.v[iVCount_]->v1[2] = v.v1[2];
  polygon.v[iVCount_]->v1[3] = v.v1[3];
  polygon.v[iVCount_]->n1[0] = v.n1[0];
  polygon.v[iVCount_]->n1[1] = v.n1[1];
  polygon.v[iVCount_]->n1[2] = v.n1[2];
  polygon.v[iVCount_]->n1[3] = v.n1[3];
  polygon.v[iVCount_]->c1    = v.c1;

  if(iVCount_ == 2)
    plotPoly(polygon);

  iVCount_ = (iVCount_ + 1) % 3;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::addVertexToTriangleStrip(SVertexF & v)
{
  static SPolygonF polygon;
  static SVertexF vertices[3];
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
  polygon.v[iVCount_]->v1[0] = v.v1[0];
  polygon.v[iVCount_]->v1[1] = v.v1[1];
  polygon.v[iVCount_]->v1[2] = v.v1[2];
  polygon.v[iVCount_]->v1[3] = v.v1[3];
  polygon.v[iVCount_]->n1[0] = v.n1[0];
  polygon.v[iVCount_]->n1[1] = v.n1[1];
  polygon.v[iVCount_]->n1[2] = v.n1[2];
  polygon.v[iVCount_]->n1[3] = v.n1[3];
  polygon.v[iVCount_]->c1    = v.c1;

  if(iVCount_ == 2)
  {
    plotPoly(polygon);

    if(bFlipFlop == true)
    {
      SVertexF * pTemp = polygon.v[0];
      polygon.v[0] = polygon.v[2];
      polygon.v[2] = pTemp;
    }
    else
    {
      SVertexF * pTemp = polygon.v[1];
      polygon.v[1] = polygon.v[2];
      polygon.v[2] = pTemp;
    }
  }
  else
    iVCount_++;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::addVertexToTriangleFan(SVertexF & v)
{
  static SPolygonF polygon;
  static SVertexF vertices[3];
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
  polygon.v[iVCount_]->v1[0] = v.v1[0];
  polygon.v[iVCount_]->v1[1] = v.v1[1];
  polygon.v[iVCount_]->v1[2] = v.v1[2];
  polygon.v[iVCount_]->v1[3] = v.v1[3];
  polygon.v[iVCount_]->n1[0] = v.n1[0];
  polygon.v[iVCount_]->n1[1] = v.n1[1];
  polygon.v[iVCount_]->n1[2] = v.n1[2];
  polygon.v[iVCount_]->n1[3] = v.n1[3];
  polygon.v[iVCount_]->c1    = v.c1;

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
CSoftGLESFloat::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex(first * bufVertex_.size);
  GLint idxColor (first * bufColor_.size);
  GLint idxNormal(first * bufNormal_.size);

  SVertexF v;
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
        v.v1[0] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.v1[1] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.v1[2] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.v1[3] = 1.0f;
        break;
      case GL_FIXED:
        v.v1[0] = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.v1[1] = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.v1[2] = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.v1[3] = 1.0f;
        break;
    };

    // Normal
    if(bBufNormalEnabled_ == true)
    {
      switch(bufColor_.type)
      {
        case GL_FLOAT:
          v.n1[0] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n1[1] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n1[2] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n1[3] = 1.0f;
          break;
        case GL_FIXED:
          v.n1[0] = gl_fptof(((GLfixed *)bufNormal_.pointer)[idxNormal++]);
          v.n1[1] = gl_fptof(((GLfixed *)bufNormal_.pointer)[idxNormal++]);
          v.n1[2] = gl_fptof(((GLfixed *)bufNormal_.pointer)[idxNormal++]);
          v.n1[3] = 1.0f;
          break;
      };
    }

    // Color
    if(bBufColorEnabled_ == true)
    {
      switch(bufColor_.type)
      {
        case GL_FLOAT:
          v.c1.r = ((GLfloat *)bufColor_.pointer)[idxColor++];
          v.c1.g = ((GLfloat *)bufColor_.pointer)[idxColor++];
          v.c1.b = ((GLfloat *)bufColor_.pointer)[idxColor++];
          v.c1.a = ((GLfloat *)bufColor_.pointer)[idxColor++];
          break;
        case GL_FIXED:
          v.c1.r = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
          v.c1.g = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
          v.c1.b = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
          v.c1.a = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
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
CSoftGLESFloat::glEnable(GLenum cap)
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
CSoftGLESFloat::glFlush(void)
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glFogf(GLenum pname, GLfloat param)
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
CSoftGLESFloat::glFogfv(GLenum pname, const GLfloat * params)
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
CSoftGLESFloat::glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  CMatrixF m;

  m.matrix[0][0] = (2.0f * zNear) / (right - left);
  m.matrix[0][1] = 0.0f;
  m.matrix[0][2] = (right + left) / (right - left);
  m.matrix[0][3] = 0.0f;

  m.matrix[1][0] = 0.0f;
  m.matrix[1][1] = (2.0f * zNear) / (top - bottom);
  m.matrix[1][2] = (top + bottom) / (top - bottom);
  m.matrix[1][3] = 0.0f;

  m.matrix[2][0] = 0.0f;
  m.matrix[2][1] = 0.0f;
  m.matrix[2][2] = -((zFar + zNear) / (zFar - zNear));
  m.matrix[2][3] = -((2.0f * zFar * zNear) / (zFar - zNear));

  m.matrix[3][0] = 0.0f;
  m.matrix[3][1] = 0.0f;
  m.matrix[3][2] = -1.0f;
  m.matrix[3][3] = 0.0f;

  (*pCurrentMatrix_) *= m;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glLightf(GLenum light, GLenum pname, GLfloat param)
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
  SLightF * pLight = 0;
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

  SColorF * pColor = 0;
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
CSoftGLESFloat::glLoadIdentity()
{
  pCurrentMatrix_->loadIdentity();

  // FIXME
  if(lightingEnabled_ == true)
    matrixRotation.loadIdentity();
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_MODELVIEW:  pCurrentMatrix_ = &matrixModelView;  break;
    case GL_PROJECTION: pCurrentMatrix_ = &matrixProjection; break;
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->rotate(angle, x, y, z);

  // FIXME
  if(lightingEnabled_ == true)
    matrixRotation.rotate(angle, x, y, z);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->scale(x, y, z);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->translate(x, y, z);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
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
  viewportByteCount  = width * height * 2;
  zbuffer            = new uint32_t[width * height];
  edge1              = new CEdgeF(viewportHeight);
  edge2              = new CEdgeF(viewportHeight);

  // Calculate field of view scalars
  //fpFieldofviewXScalar = gl_fpfromf(static_cast<float>(width)  / tan(80)); // 5.67 == tan(80)
  //fpFieldofviewYScalar = gl_fpfromf(static_cast<float>(height) / tan(80)); // 5.67 == tan(80)
  fpFieldofviewYScalar = viewportHeight;
  fpFieldofviewXScalar = viewportWidth;
  matrixPerspective.loadIdentity();
}

//-----------------------------------------------------------------------------
bool
CSoftGLESFloat::testAndSetDepth(GLfloat z, uint32_t index)
{
  if((z >= zNear_) && (z <= zFar_))
  {
    uint32_t zval = (uint32_t)((zA_ + (zB_ / z)) * 0xffffffff);

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
CSoftGLESFloat::hline(CEdgeF & from, CEdgeF & to, GLint & y, SColorF c)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLfloat mz((to.z_[y] - from.z_[y]) / dx);
    GLfloat z(from.z_[y]);
    color_t color = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(c.r * 255), (uint8_t)(c.g * 255), (uint8_t)(c.b * 255), 255);

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        if((depthTestEnabled_ == false) || (testAndSetDepth(z, index) == true))
          ((uint32_t *)renderSurface->p)[index] = color;
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
CSoftGLESFloat::hline_s(CEdgeF & from, CEdgeF & to, GLint & y)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLfloat mz((to.z_[y] - from.z_[y]) / dx);
    GLfloat z(from.z_[y]);

    GLfloat mr((to.c_[y].r - from.c_[y].r) / dx);
    GLfloat mg((to.c_[y].g - from.c_[y].g) / dx);
    GLfloat mb((to.c_[y].b - from.c_[y].b) / dx);
    GLfloat r(from.c_[y].r);
    GLfloat g(from.c_[y].g);
    GLfloat b(from.c_[y].b);

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        if((depthTestEnabled_ == false) || (testAndSetDepth(z, index) == true))
          ((uint32_t *)renderSurface->p)[index] = BxColorFormat_FromRGBA(renderSurface->format_, (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), 255);
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
CSoftGLESFloat::plotPoly(SPolygonF & poly)
{
  for(int i(0); i < 3; i++)
  {
    if(poly.v[i]->bProcessed == false)
    {
      // ModelView Transformation
      matrixModelView.transform(poly.v[i]->v1, poly.v[i]->v2);

      // Projection Transformation
      matrixProjection.transform(poly.v[i]->v2, poly.v[i]->v2);

      // Eye coordinates to clipping coordinates
      matrixPerspective.transform(poly.v[i]->v2, poly.v[i]->v2);

      // Get normalized device coordinates
      poly.v[i]->sx = (GLint)(((poly.v[i]->v2[0] / -poly.v[i]->v2[3]) + 0.5f) * viewportWidth);
      poly.v[i]->sy = (GLint)(((poly.v[i]->v2[1] / -poly.v[i]->v2[3]) + 0.5f) * viewportHeight);

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
    matrixRotation.transform(poly.v[0]->n1, poly.v[0]->n2);
    matrixRotation.transform(poly.v[1]->n1, poly.v[1]->n2);
    matrixRotation.transform(poly.v[2]->n1, poly.v[2]->n2);
    // FIXME: Light value of normal
    GLfloat normal[3] = {abs(poly.v[0]->n2[2]), abs(poly.v[1]->n2[2]), abs(poly.v[2]->n2[2])};

    for(int iLight(0); iLight < 8; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        SColorF & ambient = lights_[iLight].ambient;
        SColorF & diffuse = lights_[iLight].diffuse;

        poly.v[0]->c2.r = clampf((poly.v[0]->c1.r * ambient.r) + ((poly.v[0]->c1.r * normal[0]) * diffuse.r));
        poly.v[0]->c2.g = clampf((poly.v[0]->c1.g * ambient.g) + ((poly.v[0]->c1.g * normal[0]) * diffuse.g));
        poly.v[0]->c2.b = clampf((poly.v[0]->c1.b * ambient.b) + ((poly.v[0]->c1.b * normal[0]) * diffuse.b));

        if(shadingModel_ == GL_SMOOTH)
        {
          poly.v[1]->c2.r = clampf((poly.v[1]->c1.r * ambient.r) + ((poly.v[1]->c1.r * normal[1]) * diffuse.r));
          poly.v[1]->c2.g = clampf((poly.v[1]->c1.g * ambient.g) + ((poly.v[1]->c1.g * normal[1]) * diffuse.g));
          poly.v[1]->c2.b = clampf((poly.v[1]->c1.b * ambient.b) + ((poly.v[1]->c1.b * normal[1]) * diffuse.b));

          poly.v[2]->c2.r = clampf((poly.v[2]->c1.r * ambient.r) + ((poly.v[2]->c1.r * normal[2]) * diffuse.r));
          poly.v[2]->c2.g = clampf((poly.v[2]->c1.g * ambient.g) + ((poly.v[2]->c1.g * normal[2]) * diffuse.g));
          poly.v[2]->c2.b = clampf((poly.v[2]->c1.b * ambient.b) + ((poly.v[2]->c1.b * normal[2]) * diffuse.b));
        }
      }
    }
  }
  else
  {
    // No lighting, lust copy colors
    for(int i(0); i < 3; i++)
    {
      poly.v[i]->c2.r = poly.v[i]->c1.r;
      poly.v[i]->c2.g = poly.v[i]->c1.g;
      poly.v[i]->c2.b = poly.v[i]->c1.b;
      poly.v[i]->c2.a = poly.v[i]->c1.a;
    }
  }

  // Fog
  if(fogEnabled_ == true)
  {
    for(int i(0); i < 3; i++)
    {
      GLfloat partFog   = clampf((abs(poly.v[i]->v2[2]) - fogStart_) / (fogEnd_ - fogStart_));
      GLfloat partColor = 1.0f - partFog;
      poly.v[i]->c2.r = clampf((poly.v[i]->c2.r * partColor) + (fogColor_.r * partFog));
      poly.v[i]->c2.g = clampf((poly.v[i]->c2.g * partColor) + (fogColor_.g * partFog));
      poly.v[i]->c2.b = clampf((poly.v[i]->c2.b * partColor) + (fogColor_.b * partFog));
    }
  }

  rasterPoly(poly);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::rasterPoly(SPolygonF & poly)
{
  // Bubble sort the 3 vertexes
  SVertexF * vtemp;
  SVertexF * vhi(poly.v[0]);
  SVertexF * vmi(poly.v[1]);
  SVertexF * vlo(poly.v[2]);

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
      edge1->addZC(vlo->sx, vlo->sy, vlo->v2[3], vlo->c2, vhi->sx, vhi->sy, vhi->v2[3], vhi->c2);
      edge2->addZC(vlo->sx, vlo->sy, vlo->v2[3], vlo->c2, vmi->sx, vmi->sy, vmi->v2[3], vmi->c2);
      edge2->addZC(vmi->sx, vmi->sy, vmi->v2[3], vmi->c2, vhi->sx, vhi->sy, vhi->v2[3], vhi->c2);
    }
    else
    {
      edge1->addZ(vlo->sx, vlo->sy, vlo->v2[3], vhi->sx, vhi->sy, vhi->v2[3]);
      edge2->addZ(vlo->sx, vlo->sy, vlo->v2[3], vmi->sx, vmi->sy, vmi->v2[3]);
      edge2->addZ(vmi->sx, vmi->sy, vmi->v2[3], vhi->sx, vhi->sy, vhi->v2[3]);
    }
  }
  else
  {
    if(shadingModel_ == GL_SMOOTH)
    {
      edge1->addC(vlo->sx, vlo->sy, vlo->c2, vhi->sx, vhi->sy, vhi->c2);
      edge2->addC(vlo->sx, vlo->sy, vlo->c2, vmi->sx, vmi->sy, vmi->c2);
      edge2->addC(vmi->sx, vmi->sy, vmi->c2, vhi->sx, vhi->sy, vhi->c2);
    }
    else
    {
      edge1->add(vlo->sx, vlo->sy, vhi->sx, vhi->sy);
      edge2->add(vlo->sx, vlo->sy, vmi->sx, vmi->sy);
      edge2->add(vmi->sx, vmi->sy, vhi->sx, vhi->sy);
    }
  }

  CEdgeF * pEdgeLeft  = edge1;
  CEdgeF * pEdgeRight = edge2;
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
        hline(*pEdgeLeft, *pEdgeRight, y, poly.v[2]->c2);
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
