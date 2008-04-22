#include "softGLFx.h"
#include "matrix.h"
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
 , zNear_(gl_fpfromi(0))
 , zFar_(gl_fpfromi(1))

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
  clCurrent.r = gl_fpfromi(0);
  clCurrent.g = gl_fpfromi(0);
  clCurrent.b = gl_fpfromi(0);
  clCurrent.a = gl_fpfromi(1);

  clClear.r = gl_fpfromi(0);
  clClear.g = gl_fpfromi(0);
  clClear.b = gl_fpfromi(0);
  clClear.a = gl_fpfromi(1);

  // Light properties
  for(int iLight(0); iLight < 8; iLight++)
  {
    lights_[iLight].ambient.r = gl_fpfromi(0);
    lights_[iLight].ambient.g = gl_fpfromi(0);
    lights_[iLight].ambient.b = gl_fpfromi(0);
    lights_[iLight].ambient.a = gl_fpfromi(1);

    if(iLight == 0)
    {
      lights_[iLight].diffuse.r = gl_fpfromi(1);
      lights_[iLight].diffuse.g = gl_fpfromi(1);
      lights_[iLight].diffuse.b = gl_fpfromi(1);
      lights_[iLight].diffuse.a = gl_fpfromi(1);

      lights_[iLight].specular.r = gl_fpfromi(1);
      lights_[iLight].specular.g = gl_fpfromi(1);
      lights_[iLight].specular.b = gl_fpfromi(1);
      lights_[iLight].specular.a = gl_fpfromi(1);
    }
    else
    {
      lights_[iLight].diffuse.r = gl_fpfromi(0);
      lights_[iLight].diffuse.g = gl_fpfromi(0);
      lights_[iLight].diffuse.b = gl_fpfromi(0);
      lights_[iLight].diffuse.a = gl_fpfromi(0);

      lights_[iLight].specular.r = gl_fpfromi(0);
      lights_[iLight].specular.g = gl_fpfromi(0);
      lights_[iLight].specular.b = gl_fpfromi(0);
      lights_[iLight].specular.a = gl_fpfromi(0);
    }

    lights_[iLight].position.x = gl_fpfromi(0);
    lights_[iLight].position.y = gl_fpfromi(0);
    lights_[iLight].position.z = gl_fpfromi(1);
    lights_[iLight].position.w = gl_fpfromi(0);
    vecInverseFx(lights_[iLight].direction.v, lights_[iLight].position.v);

    lights_[iLight].enabled = false;
  }

  // Material properties
  matColorAmbient_.r  = gl_fpfromf(0.2f);
  matColorAmbient_.r  = gl_fpfromf(0.2f);
  matColorAmbient_.r  = gl_fpfromf(0.2f);
  matColorAmbient_.r  = gl_fpfromf(1.0f);

  matColorDiffuse_.r  = gl_fpfromf(0.8f);
  matColorDiffuse_.r  = gl_fpfromf(0.8f);
  matColorDiffuse_.r  = gl_fpfromf(0.8f);
  matColorDiffuse_.r  = gl_fpfromf(1.0f);

  matColorSpecular_.r = gl_fpfromf(0.0f);
  matColorSpecular_.r = gl_fpfromf(0.0f);
  matColorSpecular_.r = gl_fpfromf(0.0f);
  matColorSpecular_.r = gl_fpfromf(1.0f);

  matColorEmission_.r = gl_fpfromf(0.0f);
  matColorEmission_.r = gl_fpfromf(0.0f);
  matColorEmission_.r = gl_fpfromf(0.0f);
  matColorEmission_.r = gl_fpfromf(1.0f);

  matShininess_       = gl_fpfromf(0.0f);
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
CSoftGLESFixed::glDepthRangex(GLclampx zNear, GLclampx zFar)
{
  zNear_ = clampfx(zNear);
  zFar_  = clampfx(zFar);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  normal_[0] = nx;
  normal_[1] = ny;
  normal_[2] = nz;
  normal_[3] = gl_fpfromi(1);

  if(normalizeEnabled_  == true)
    vecNormalizeFx(normal_, normal_);
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
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex  (first * bufVertex_.size);
  GLint idxColor   (first * bufColor_.size);
  GLint idxNormal  (first * bufNormal_.size);
  GLint idxTexCoord(first * bufTexCoord_.size);

  SVertexFx * polygon[3];
  SVertexFx   vertices[3];
  bool bFlipFlop(true);
  polygon[0] = &vertices[0];
  polygon[1] = &vertices[1];
  polygon[2] = &vertices[2];
  GLint idx(0);

  // Process all vertices
  for(GLint i(0); i < count; i++)
  {
    SVertexFx & v = *polygon[idx];

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
      switch(bufNormal_.type)
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
          v.cr = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v.cg = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v.cb = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v.ca = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          break;
        case GL_FIXED:
          v.cr = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.cg = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.cb = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.ca = ((GLfixed *)bufColor_.pointer)[idxColor++];
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
          v.ts =                  gl_fpfromf(((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++]);
          v.tt = (gl_fpfromi(1) - gl_fpfromf(((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++]));
          break;
        case GL_FIXED:
          v.ts =                  ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++];
          v.tt = (gl_fpfromi(1) - ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]);
          break;
      };
    }

    // -------------
    // Vertex shader
    // -------------
    vertexShader(v);

    // ------------------
    // Primitive Assembly
    // ------------------
    switch(mode)
    {
      case GL_TRIANGLES:
      {
        if(idx == 2)
          plotPoly(polygon);
        idx = (idx + 1) % 3;
        break;
      }
      case GL_TRIANGLE_STRIP:
      {
        if(idx == 2)
        {
          plotPoly(polygon);
          if(bFlipFlop == true)
          {
            SVertexFx * pTemp = polygon[0];
            polygon[0] = polygon[2];
            polygon[2] = pTemp;
          }
          else
          {
            SVertexFx * pTemp = polygon[1];
            polygon[1] = polygon[2];
            polygon[2] = pTemp;
          }
        }
        else
          idx++;
        break;
      }
      case GL_TRIANGLE_FAN:
      {
        if(idx == 2)
        {
          plotPoly(polygon);
          // Swap 3rd and 2nd vertex
          if(polygon[1] == &vertices[1])
          {
            polygon[1] = &vertices[2];
            polygon[2] = &vertices[1];
          }
          else
          {
            polygon[1] = &vertices[1];
            polygon[2] = &vertices[2];
          }
        }
        else
          idx++;
        break;
      }
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
    case GL_POSITION:
      pLight->position.v[0] = params[0];
      pLight->position.v[1] = params[1];
      pLight->position.v[2] = params[2];
      pLight->position.v[3] = params[3];
      // Invert and normalize
      vecInverseFx(pLight->direction.v, pLight->position.v);
      vecNormalizeFx(pLight->direction.v, pLight->direction.v);
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
CSoftGLESFixed::glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
  switch(pname)
  {
    case GL_SHININESS:
      matShininess_ = param;
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
      matColorAmbient_.r = params[0];
      matColorAmbient_.g = params[1];
      matColorAmbient_.b = params[2];
      matColorAmbient_.a = params[3];
      break;
    case GL_DIFFUSE:
      matColorDiffuse_.r = params[0];
      matColorDiffuse_.g = params[1];
      matColorDiffuse_.b = params[2];
      matColorDiffuse_.a = params[3];
      break;
    case GL_SPECULAR:
      matColorSpecular_.r = params[0];
      matColorSpecular_.g = params[1];
      matColorSpecular_.b = params[2];
      matColorSpecular_.a = params[3];
      break;
    case GL_EMISSION:
      matColorEmission_.r = params[0];
      matColorEmission_.g = params[1];
      matColorEmission_.b = params[2];
      matColorEmission_.a = params[3];
      break;
    case GL_SHININESS:
      matShininess_ = params[0];
      break;
    case GL_AMBIENT_AND_DIFFUSE:
      matColorAmbient_.r = params[0];
      matColorAmbient_.g = params[1];
      matColorAmbient_.b = params[2];
      matColorAmbient_.a = params[3];
      matColorDiffuse_.r = params[0];
      matColorDiffuse_.g = params[1];
      matColorDiffuse_.b = params[2];
      matColorDiffuse_.a = params[3];
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
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CSoftGLESFixed::vertexShader(SVertexFx & v)
{
  // --------------
  // Transformation
  // --------------
  // Model-View matrix
  //   from 'object coordinates' to 'eye coordinates'
  matrixModelView.transform(v.v, v.v);
  // Projection matrix
  //   from 'eye coordinates' to 'clip coordinates'
  matrixProjection.transform(v.v, v.v);
  // Perspective division
  //   from 'clip coordinates' to 'normalized device coordinates'
  v.v[0] = gl_fpdiv(v.v[0], v.v[3]);
  v.v[1] = gl_fpdiv(v.v[1], v.v[3]);
  v.v[2] = gl_fpdiv(v.v[2], v.v[3]);
  // Viewport transformation
  //   from 'normalized device coordinates' to 'window coordinates'
  v.sx = gl_fptoi(gl_fpmul(( v.v[0] + gl_fpfromi(1)), gl_fpfromi(viewportWidth  >> 1)));
  v.sy = gl_fptoi(gl_fpmul((-v.v[1] + gl_fpfromi(1)), gl_fpfromi(viewportHeight >> 1)));
//  v.sz = gl_fpdiv(zFar - zNear, v.v[2] << 1) + ((zNear + zFar)>>1);

  // --------
  // Lighting
  // --------
  if(lightingEnabled_ == true)
  {
    GLfloat r(0.0f), g(0.0f), b(0.0f);

    // Normal Rotation
    matrixNormal.transform(v.n, v.n);

    for(int iLight(0); iLight < 8; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        // Ambient light (it's everywhere!)
        r += lights_[iLight].ambient.r;
        g += lights_[iLight].ambient.g;
        b += lights_[iLight].ambient.b;

        // Inner product of normal and light direction
        GLfixed ip = vecInnerProductFx(v.n, lights_[0].direction.v);
        if(ip < 0.0f) ip = -ip;
        // Multiply with light color
        r += gl_fpmul(lights_[iLight].diffuse.r, ip);
        g += gl_fpmul(lights_[iLight].diffuse.g, ip);
        b += gl_fpmul(lights_[iLight].diffuse.b, ip);
      }
    }

    // Multiply vertex color by calculated color
    v.cr = gl_fpmul(v.cr, r);
    v.cg = gl_fpmul(v.cg, g);
    v.cb = gl_fpmul(v.cb, b);
    // Clamp to 0..1
    v.cr = clampfx(v.cr);
    v.cg = clampfx(v.cg);
    v.cb = clampfx(v.cb);
  }

  // ---
  // Fog
  // ---
  if(fogEnabled_ == true)
  {
    GLfixed partFog   = clampfx(gl_fpdiv(abs(v.v[2]) - fogStart_, fogEnd_ - fogStart_));
    GLfixed partColor = gl_fpfromi(1) - partFog;
    v.cr = clampfx(gl_fpmul(v.cr, partColor) + gl_fpmul(fogColor_.r, partFog));
    v.cg = clampfx(gl_fpmul(v.cg, partColor) + gl_fpmul(fogColor_.g, partFog));
    v.cb = clampfx(gl_fpmul(v.cb, partColor) + gl_fpmul(fogColor_.b, partFog));
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::plotPoly(SVertexFx * vtx[3])
{
  // Backface culling
  if(cullFaceEnabled_ == true)
  {
    // Always invisible when culling both front and back
    if(cullFaceMode_ == GL_FRONT_AND_BACK)
      return;

    // Figure out if we need to cull
    if((vtx[1]->sx != vtx[0]->sx) && (vtx[2]->sx != vtx[0]->sx))
    {
      if(((((gl_fpfromi(vtx[1]->sy - vtx[0]->sy) / (vtx[1]->sx - vtx[0]->sx)) - (gl_fpfromi(vtx[2]->sy - vtx[0]->sy) / (vtx[2]->sx - vtx[0]->sx))) < 0) ^ ((vtx[0]->sx <= vtx[1]->sx) == (vtx[0]->sx > vtx[2]->sx))) == bCullCW_)
        return;
    }
    else if((vtx[2]->sx != vtx[1]->sx) && (vtx[0]->sx != vtx[1]->sx))
    {
      if(((((gl_fpfromi(vtx[2]->sy - vtx[1]->sy) / (vtx[2]->sx - vtx[1]->sx)) - (gl_fpfromi(vtx[0]->sy - vtx[1]->sy) / (vtx[0]->sx - vtx[1]->sx))) < 0) ^ ((vtx[1]->sx <= vtx[2]->sx) == (vtx[1]->sx > vtx[0]->sx))) == bCullCW_)
        return;
    }
    else if((vtx[0]->sx != vtx[2]->sx) && (vtx[1]->sx != vtx[2]->sx))
    {
      if(((((gl_fpfromi(vtx[0]->sy - vtx[2]->sy) / (vtx[0]->sx - vtx[2]->sx)) - (gl_fpfromi(vtx[1]->sy - vtx[2]->sy) / (vtx[1]->sx - vtx[2]->sx))) < 0) ^ ((vtx[2]->sx <= vtx[0]->sx) == (vtx[2]->sx > vtx[1]->sx))) == bCullCW_)
        return;
    }
    else
      return; // Triangle invisible
  }

  rasterPoly(vtx);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::rasterPoly(SVertexFx * vtx[3])
{
  // Bubble sort the 3 vertexes
  SVertexFx * vtemp;
  SVertexFx * vhi(vtx[0]);
  SVertexFx * vmi(vtx[1]);
  SVertexFx * vlo(vtx[2]);

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
      edge1->addZT(vlo->sx, vlo->sy, vlo->v[3], vlo->ts, vlo->tt, vhi->sx, vhi->sy, vhi->v[3], vhi->ts, vhi->tt);
      edge2->addZT(vlo->sx, vlo->sy, vlo->v[3], vlo->ts, vlo->tt, vmi->sx, vmi->sy, vmi->v[3], vmi->ts, vmi->tt);
      edge2->addZT(vmi->sx, vmi->sy, vmi->v[3], vmi->ts, vmi->tt, vhi->sx, vhi->sy, vhi->v[3], vhi->ts, vhi->tt);
    }
    else
    {
      edge1->addT(vlo->sx, vlo->sy, vlo->ts, vlo->tt, vhi->sx, vhi->sy, vhi->ts, vhi->tt);
      edge2->addT(vlo->sx, vlo->sy, vlo->ts, vlo->tt, vmi->sx, vmi->sy, vmi->ts, vmi->tt);
      edge2->addT(vmi->sx, vmi->sy, vmi->ts, vmi->tt, vhi->sx, vhi->sy, vhi->ts, vhi->tt);
    }
  }
  else if(shadingModel_ == GL_SMOOTH)
  {
    if(depthTestEnabled_ == true)
    {
      edge1->addZC(vlo->sx, vlo->sy, vlo->v[3], vlo->cl, vhi->sx, vhi->sy, vhi->v[3], vhi->cl);
      edge2->addZC(vlo->sx, vlo->sy, vlo->v[3], vlo->cl, vmi->sx, vmi->sy, vmi->v[3], vmi->cl);
      edge2->addZC(vmi->sx, vmi->sy, vmi->v[3], vmi->cl, vhi->sx, vhi->sy, vhi->v[3], vhi->cl);
    }
    else
    {
      edge1->addC(vlo->sx, vlo->sy, vlo->cl, vhi->sx, vhi->sy, vhi->cl);
      edge2->addC(vlo->sx, vlo->sy, vlo->cl, vmi->sx, vmi->sy, vmi->cl);
      edge2->addC(vmi->sx, vmi->sy, vmi->cl, vhi->sx, vhi->sy, vhi->cl);
    }
  }
  else
  {
    if(depthTestEnabled_ == true)
    {
      edge1->addZ(vlo->sx, vlo->sy, vlo->v[3], vhi->sx, vhi->sy, vhi->v[3]);
      edge2->addZ(vlo->sx, vlo->sy, vlo->v[3], vmi->sx, vmi->sy, vmi->v[3]);
      edge2->addZ(vmi->sx, vmi->sy, vmi->v[3], vhi->sx, vhi->sy, vhi->v[3]);
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
        hlineZ(*pEdgeLeft, *pEdgeRight, y, vtx[2]->cl);
    else
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hline(*pEdgeLeft, *pEdgeRight, y, vtx[2]->cl);
  }
}
