#include "softGLFx.h"
#include "fixedPoint.h"

#include "stdlib.h"
typedef unsigned int wint_t;
#include <math.h>


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoftGLESFixed::CSoftGLESFixed()
 : CAGLESFloatToFxContext()
 , CAGLESBuffers()
 , CAGLESCull()
 , CAGLESMatrixFx()
 , CAGLESTextures()

 , depthTestEnabled_(false)
 , depthFunction_(GL_LESS)
 , depthClear_(gl_fpfromi(1))
 , zClearValue_(0xffff)
 , zbuffer(0)
 , zNear_(0)
 , zFar_(1)

 , shadingModel_(GL_FLAT)

 , lightingEnabled_(false)
 , normalizeEnabled_(false)
 , fogEnabled_(false)
 , edge1(0)
 , edge2(0)
 , viewportXOffset(0)
 , viewportYOffset(0)
 , viewportPixelCount(0)
 , viewportWidth(0)
 , viewportHeight(0)
{
  clCurrent.r = 1;
  clCurrent.g = 1;
  clCurrent.b = 1;
  clCurrent.a = 1;

  clClear.r = 0;
  clClear.g = 0;
  clClear.b = 0;
  clClear.a = 0;

  // Light properties
  for(int iLight(0); iLight < 8; iLight++)
  {
    lights_[iLight].ambient.r = 0;
    lights_[iLight].ambient.g = 0;
    lights_[iLight].ambient.b = 0;
    lights_[iLight].ambient.a = 1;

    if(iLight == 0)
    {
      lights_[iLight].diffuse.r = 1;
      lights_[iLight].diffuse.g = 1;
      lights_[iLight].diffuse.b = 1;
      lights_[iLight].diffuse.a = 1;

      lights_[iLight].specular.r = 1;
      lights_[iLight].specular.g = 1;
      lights_[iLight].specular.b = 1;
      lights_[iLight].specular.a = 1;
    }
    else
    {
      lights_[iLight].diffuse.r = 0;
      lights_[iLight].diffuse.g = 0;
      lights_[iLight].diffuse.b = 0;
      lights_[iLight].diffuse.a = 0;

      lights_[iLight].specular.r = 0;
      lights_[iLight].specular.g = 0;
      lights_[iLight].specular.b = 0;
      lights_[iLight].specular.a = 0;
    }

    lights_[iLight].position.x = 0;
    lights_[iLight].position.y = 0;
    lights_[iLight].position.z = 1;
    lights_[iLight].position.w = 0;
    lights_[iLight].direction  = lights_[iLight].position;
    lights_[iLight].direction.invert();

    lights_[iLight].enabled = false;
  }

  // Material properties
  matColorAmbient_.r  = 0.2f;
  matColorAmbient_.g  = 0.2f;
  matColorAmbient_.b  = 0.2f;
  matColorAmbient_.a  = 1.0f;

  matColorDiffuse_.r  = 0.8f;
  matColorDiffuse_.g  = 0.8f;
  matColorDiffuse_.b  = 0.8f;
  matColorDiffuse_.a  = 1.0f;

  matColorSpecular_.r = 0.0f;
  matColorSpecular_.g = 0.0f;
  matColorSpecular_.b = 0.0f;
  matColorSpecular_.a = 1.0f;

  matColorEmission_.r = 0.0f;
  matColorEmission_.g = 0.0f;
  matColorEmission_.b = 0.0f;
  matColorEmission_.a = 1.0f;

  matShininess_       = 0.0f;

  zA_ = (zFar_ - zNear_) / 2;
  zB_ = (zFar_ + zNear_) / 2;
}

//-----------------------------------------------------------------------------
CSoftGLESFixed::~CSoftGLESFixed()
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glClear(GLbitfield mask)
{
  if(mask & GL_COLOR_BUFFER_BIT)
  {
    color_t color(fpRGB(clClear.r, clClear.g, clClear.b));

    switch(renderSurface->mode.bpp)
    {
      case 8:
      {
        for(uint32_t y(0); y < renderSurface->mode.width; y++)
          for(uint32_t x(0); x < renderSurface->mode.height; x++)
            ((uint8_t  *)renderSurface->p)[(y + viewportYOffset) * renderSurface->mode.xpitch + (x + viewportXOffset)] = color;
        break;
      }
      case 16:
      {
        for(uint32_t y(0); y < renderSurface->mode.width; y++)
          for(uint32_t x(0); x < renderSurface->mode.height; x++)
            ((uint16_t *)renderSurface->p)[(y + viewportYOffset) * renderSurface->mode.xpitch + (x + viewportXOffset)] = color;
        break;
      }
      case 32:
      {
        for(uint32_t y(0); y < renderSurface->mode.width; y++)
          for(uint32_t x(0); x < renderSurface->mode.height; x++)
            ((uint32_t *)renderSurface->p)[(y + viewportYOffset) * renderSurface->mode.xpitch + (x + viewportXOffset)] = color;
        break;
      }
    };
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
  clClear.r.value = clampfx(red);
  clClear.g.value = clampfx(green);
  clClear.b.value = clampfx(blue);
  clClear.a.value = clampfx(alpha);
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
  clCurrent.r.value = gl_fpfromi(red  ) / 255;
  clCurrent.g.value = gl_fpfromi(green) / 255;
  clCurrent.b.value = gl_fpfromi(blue ) / 255;
  clCurrent.a.value = gl_fpfromi(alpha) / 255;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  clCurrent.r.value = red;
  clCurrent.g.value = green;
  clCurrent.b.value = blue;
  clCurrent.a.value = alpha;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glDepthRangex(GLclampx zNear, GLclampx zFar)
{
  zNear_.value = clampfx(zNear);
  zFar_.value  = clampfx(zFar);

  zA_ = (zFar_ - zNear_) / 2;
  zB_ = (zFar_ + zNear_) / 2;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  normal_.x.value = nx;
  normal_.y.value = ny;
  normal_.z.value = nz;

  if(normalizeEnabled_  == true)
    normal_.normalize();
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
    case GL_FOG:        fogEnabled_       = false; break;
    case GL_TEXTURE_2D: texturesEnabled_  = false; break;
    case GL_NORMALIZE:  normalizeEnabled_ = false; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  _glDrawArrays(mode, first, count);
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
    case GL_TEXTURE_2D: texturesEnabled_   = true; break;
    case GL_NORMALIZE:  normalizeEnabled_  = true; break;

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
      fogColor_.r.value = params[0];
      fogColor_.g.value = params[1];
      fogColor_.b.value = params[2];
      fogColor_.a.value = params[3];
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
    case GL_POSITION:
      pLight->position.x.value = params[0];
      pLight->position.y.value = params[1];
      pLight->position.z.value = params[2];
      pLight->position.w.value = params[3];
      // Invert and normalize
      pLight->direction = pLight->position;
      pLight->direction.invert();
      pLight->direction.normalize();
    default:
      return;
  }

  pColor->r.value = params[0];
  pColor->g.value = params[1];
  pColor->b.value = params[2];
  pColor->a.value = params[3];
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
  switch(pname)
  {
    case GL_SHININESS:
      matShininess_.value = param;
      break;
    default:
      return;
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glMaterialxv(GLenum face, GLenum pname, const GLfixed *params)
{
  switch(pname)
  {
    case GL_AMBIENT:
      matColorAmbient_.r.value = params[0];
      matColorAmbient_.g.value = params[1];
      matColorAmbient_.b.value = params[2];
      matColorAmbient_.a.value = params[3];
      break;
    case GL_DIFFUSE:
      matColorDiffuse_.r.value = params[0];
      matColorDiffuse_.g.value = params[1];
      matColorDiffuse_.b.value = params[2];
      matColorDiffuse_.a.value = params[3];
      break;
    case GL_SPECULAR:
      matColorSpecular_.r.value = params[0];
      matColorSpecular_.g.value = params[1];
      matColorSpecular_.b.value = params[2];
      matColorSpecular_.a.value = params[3];
      break;
    case GL_EMISSION:
      matColorEmission_.r.value = params[0];
      matColorEmission_.g.value = params[1];
      matColorEmission_.b.value = params[2];
      matColorEmission_.a.value = params[3];
      break;
    case GL_SHININESS:
      matShininess_.value = params[0];
      break;
    case GL_AMBIENT_AND_DIFFUSE:
      matColorAmbient_.r.value = params[0];
      matColorAmbient_.g.value = params[1];
      matColorAmbient_.b.value = params[2];
      matColorAmbient_.a.value = params[3];
      matColorDiffuse_.r.value = params[0];
      matColorDiffuse_.g.value = params[1];
      matColorDiffuse_.b.value = params[2];
      matColorDiffuse_.a.value = params[3];
      break;
    default:
      return;
  }
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

  xA_ = (viewportWidth  >> 1);
  xB_ = CFixed(viewportWidth  >> 1) + 0.5f;
  yA_ = -(viewportHeight >> 1);
  yB_ = CFixed(viewportHeight >> 1) + 0.5f;
}

//-----------------------------------------------------------------------------
inline CFixed
my_pow(CFixed x, int y)
{
  CFixed rv(x);
  for(int i(1); i < y; i++)
    rv *= x;
  return rv;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::vertexShaderTransform(SVertexFx & v)
{
  _vertexShaderTransform(v);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::vertexShaderLight(SVertexFx & v)
{
  _vertexShaderLight(v);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::primitiveAssembly(SVertexFx & v)
{
  _primitiveAssembly(v);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::rasterTriangle(STriangleFx & tri)
{
  _rasterTriangle(tri);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::begin(GLenum mode)
{
  rasterMode_ = mode;

  // Initialize for default triangle
  triangle_.v[0] = &vertices[0];
  triangle_.v[1] = &vertices[1];
  triangle_.v[2] = &vertices[2];
  bFlipFlop_ = true;
  vertIdx_   = 0;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::end()
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex  (first * bufVertex_.size);
  GLint idxColor   (first * bufColor_.size);
  GLint idxNormal  (first * bufNormal_.size);
  GLint idxTexCoord(first * bufTexCoord_.size);
  SVertexFx v;

  begin(mode);

  // Process all vertices
  for(GLint i(0); i < count; i++)
  {
    // Vertex
    switch(bufVertex_.type)
    {
      case GL_FLOAT:
        v.vo[0] = (((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.vo[1] = (((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.vo[2] = (((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.vo[3] = 1;
        break;
      case GL_FIXED:
        v.vo[0].value = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.vo[1].value = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.vo[2].value = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.vo[3] = 1;
        break;
    };

    // Normal
    if(bBufNormalEnabled_ == true)
    {
      switch(bufNormal_.type)
      {
        case GL_FLOAT:
          v.n.x = (((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n.y = (((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n.z = (((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          break;
        case GL_FIXED:
          v.n.x.value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n.y.value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n.z.value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          break;
      };
    }
    else
      v.n = normal_;

    // Color
    if(bBufColorEnabled_ == true)
    {
      switch(bufColor_.type)
      {
        case GL_FLOAT:
          v.cl.r = ((GLfloat *)bufColor_.pointer)[idxColor++];
          v.cl.g = ((GLfloat *)bufColor_.pointer)[idxColor++];
          v.cl.b = ((GLfloat *)bufColor_.pointer)[idxColor++];
          v.cl.a = ((GLfloat *)bufColor_.pointer)[idxColor++];
          break;
        case GL_FIXED:
          v.cl.r.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.cl.g.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.cl.b.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.cl.a.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
          break;
      };
    }
    else
      v.cl = clCurrent;

    // Textures
    if((texturesEnabled_ == true) && (bBufTexCoordEnabled_ == true))
    {
      switch(bufTexCoord_.type)
      {
        case GL_FLOAT:
          v.t[0] =     CFixed(((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++]);
          v.t[1] = 1 - CFixed(((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++]);
          break;
        case GL_FIXED:
          v.t[0].value =                  ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++];
          v.t[1].value = (gl_fpfromi(1) - ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]);
          break;
      };
    }

    v.processed = false;

    // --------------
    // Transformation
    // --------------
    // Model-View matrix
    //   from 'object coordinates' to 'eye coordinates'
    matrixModelView.transform4(v.vo, v.ve);

    // ------------------
    // Primitive Assembly
    // ------------------
    primitiveAssembly(v);
  }

  end();
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::_vertexShaderTransform(SVertexFx & v)
{
  // --------------
  // Transformation
  // --------------
  // Projection matrix
  //   from 'eye coordinates' to 'clip coordinates'
  matrixProjection.transform4(v.ve, v.vc);
  // Perspective division
  //   from 'clip coordinates' to 'normalized device coordinates'
  CFixed iw = 1 / v.vc[3];
  v.vd[0] = v.vc[0] * iw;
  v.vd[1] = v.vc[1] * iw;
  v.vd[2] = v.vc[2] * iw;
  // Viewport transformation
  //   from 'normalized device coordinates' to 'window coordinates'
  v.sx = (GLint)((xA_ * v.vd[0]) + xB_);
  v.sy = (GLint)((yA_ * v.vd[1]) + yB_);
  v.sz =        ((zA_ * v.vd[2]) + zB_).value - 1; // 16bit z-buffer
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::_vertexShaderLight(SVertexFx & v)
{
  // --------
  // Lighting
  // --------
  if(lightingEnabled_ == true)
  {
    SColorFx c(0, 0, 0, 0);

    // Normal Rotation
    matrixNormal.transform3(v.n, v.n);

    for(int iLight(0); iLight < 8; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        // Ambient light (it's everywhere!)
        c += lights_[iLight].ambient * matColorAmbient_;

        // Diffuse light
        CFixed diffuse = 0 - lights_[iLight].direction.dotProduct(v.n);
        if(diffuse >= 0.0f)
        {
          c += lights_[iLight].diffuse * matColorDiffuse_ * diffuse;
        }

        if(matShininess_ >= 0.5f)
        {
          // Specular light
          TVector3<CFixed> eye(0, 0, 1);
          CFixed specular = lights_[iLight].direction.getCrossProduct(v.n).dotProduct(eye);
          if(specular >= 0.0f)
          {
            specular = my_pow(specular, (int)(matShininess_ + 0.5f));
            c += lights_[iLight].specular * matColorSpecular_ * specular;
          }
        }
      }
    }

    // Multiply vertex color by calculated color
    v.cl2 = v.cl * c;
    // Clamp to 0..1
    v.cl2.clamp();
  }
  else
  {
    v.cl2 = v.cl;
  }

  // ---
  // Fog
  // ---
  if(fogEnabled_ == true)
  {
    CFixed partFog, partColor;
    partFog.value = clampfx(gl_fpdiv(abs(v.ve[2].value) - fogStart_, fogEnd_ - fogStart_));
    partColor = 1 - partFog;
    v.cl2 = ((v.cl2 * partColor) + (fogColor_ * partFog)).getClamped();
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::_primitiveAssembly(SVertexFx & v)
{
  // Copy vertex into vertex buffer
  *triangle_.v[vertIdx_] = v;

  // ------------------
  // Primitive Assembly
  // ------------------
  switch(rasterMode_)
  {
    case GL_TRIANGLES:
    {
      if(vertIdx_ == 2)
        rasterTriangle(triangle_);
      vertIdx_++;
      if(vertIdx_ > 2)
        vertIdx_ = 0;
      break;
    }
    case GL_TRIANGLE_STRIP:
    {
      if(vertIdx_ == 2)
      {
        rasterTriangle(triangle_);
        // Swap 3rd with 1st or 2nd vertex pointer
        if(bFlipFlop_ == true)
        {
          SVertexFx * pTemp = triangle_.v[0];
          triangle_.v[0] = triangle_.v[2];
          triangle_.v[2] = pTemp;
        }
        else
        {
          SVertexFx * pTemp = triangle_.v[1];
          triangle_.v[1] = triangle_.v[2];
          triangle_.v[2] = pTemp;
        }
        bFlipFlop_ = !bFlipFlop_;
      }
      else
        vertIdx_++;
      break;
    }
    case GL_TRIANGLE_FAN:
    {
      if(vertIdx_ == 2)
      {
        rasterTriangle(triangle_);
        // Swap 3rd with 2nd vertex pointer
        SVertexFx * pTemp = triangle_.v[1];
        triangle_.v[1] = triangle_.v[2];
        triangle_.v[2] = pTemp;
      }
      else
        vertIdx_++;
      break;
    }
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::_rasterTriangle(STriangleFx & tri)
{
  // -------
  // Culling
  // -------
  if(cullFaceEnabled_ == true)
  {
    // Always invisible when culling both front and back
    if(cullFaceMode_ == GL_FRONT_AND_BACK)
      return;

    TVector3<CFixed> V0(tri.v[0]->ve);
    TVector3<CFixed> V1(tri.v[1]->ve);
    TVector3<CFixed> V2(tri.v[2]->ve);
    TVector3<CFixed> normal;

    normal = (V0 - V1).getCrossProduct(V2 - V1);
    if((normal.z.value < 0) == bCullCW_)
      return;

//    normal.normalize();
//    tri.v[0]->n = normal;
//    tri.v[1]->n = normal;
//    tri.v[2]->n = normal;
  }

  // -------------
  // Vertex shader
  // -------------
  if(tri.v[0]->processed == false)
  {
    vertexShaderTransform(*tri.v[0]);
    if(shadingModel_ == GL_SMOOTH)
      vertexShaderLight(*tri.v[0]);
    tri.v[0]->processed = true;
  }
  if(tri.v[1]->processed == false)
  {
    vertexShaderTransform(*tri.v[1]);
    if(shadingModel_ == GL_SMOOTH)
      vertexShaderLight(*tri.v[1]);
    tri.v[1]->processed = true;
  }
  if(tri.v[2]->processed == false)
  {
    vertexShaderTransform(*tri.v[2]);
    vertexShaderLight(*tri.v[2]);
    tri.v[2]->processed = true;
  }

  // Bubble sort the 3 vertexes
  SVertexFx * vtemp;
  SVertexFx * vhi(tri.v[0]);
  SVertexFx * vmi(tri.v[1]);
  SVertexFx * vlo(tri.v[2]);

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
  if(texturesEnabled_ == true)
  {
    if(depthTestEnabled_ == true)
    {
      edge1->addZT(*vlo, *vhi);
      edge2->addZT(*vlo, *vmi);
      edge2->addZT(*vmi, *vhi);
    }
    else
    {
      edge1->addT(*vlo, *vhi);
      edge2->addT(*vlo, *vmi);
      edge2->addT(*vmi, *vhi);
    }
  }
  else if(shadingModel_ == GL_SMOOTH)
  {
    if(depthTestEnabled_ == true)
    {
      edge1->addZC(*vlo, *vhi);
      edge2->addZC(*vlo, *vmi);
      edge2->addZC(*vmi, *vhi);
    }
    else
    {
      edge1->addC(*vlo, *vhi);
      edge2->addC(*vlo, *vmi);
      edge2->addC(*vmi, *vhi);
    }
  }
  else
  {
    if(depthTestEnabled_ == true)
    {
      edge1->addZ(*vlo, *vhi);
      edge2->addZ(*vlo, *vmi);
      edge2->addZ(*vmi, *vhi);
    }
    else
    {
      edge1->add(*vlo, *vhi);
      edge2->add(*vlo, *vmi);
      edge2->add(*vmi, *vhi);
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
  if(texturesEnabled_ == true)
  {
    if(depthTestEnabled_ == true)
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hlineZTa(*pEdgeLeft, *pEdgeRight, y);
    else
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hlineTa(*pEdgeLeft, *pEdgeRight, y);
  }
  else if(shadingModel_ == GL_SMOOTH)
  {
    if(depthTestEnabled_ == true)
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hlineZC(*pEdgeLeft, *pEdgeRight, y);
    else
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hlineC(*pEdgeLeft, *pEdgeRight, y);
  }
  else
  {
    if(depthTestEnabled_ == true)
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hlineZ(*pEdgeLeft, *pEdgeRight, y, tri.v[2]->cl2);
    else
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hline(*pEdgeLeft, *pEdgeRight, y, tri.v[2]->cl2);
  }
}
