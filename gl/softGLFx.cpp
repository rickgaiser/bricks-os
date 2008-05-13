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
  zNear_ = clampfx(zNear);
  zFar_  = clampfx(zFar);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  normal_.x.value = nx;
  normal_.y.value = ny;
  normal_.z.value = nz;
  normal_.w       = 1;

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
        v.v[0] = (((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v[1] = (((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v[2] = (((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v[3] = 1;
        break;
      case GL_FIXED:
        v.v[0].value = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v[1].value = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v[2].value = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v[3] = 1;
        break;
    };

    // Normal
    if(bBufNormalEnabled_ == true)
    {
      switch(bufNormal_.type)
      {
        case GL_FLOAT:
          v.n[0] = (((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n[1] = (((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n[2] = (((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n[3] = 1;
          break;
        case GL_FIXED:
          v.n[0].value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n[1].value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n[2].value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n[3] = 1;
          break;
      };
    }

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

    // -------------
    // Vertex shader
    // -------------
    vertexShader(v);

    // ------------
    // Raterization
    // ------------
    rasterize(v);
  }

  end();
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
  matrixModelView.transform4(v.v, v.v);
  // Projection matrix
  //   from 'eye coordinates' to 'clip coordinates'
  matrixProjection.transform4(v.v, v.v);
  // Perspective division
  //   from 'clip coordinates' to 'normalized device coordinates'
  v.v[0] /= v.v[3];
  v.v[1] /= v.v[3];
  v.v[2] /= v.v[3];
  // Viewport transformation
  //   from 'normalized device coordinates' to 'window coordinates'
  v.sx = (    v.v[0] + 1) * CFixed(viewportWidth  >> 1) + 0.5f;
  v.sy = (0 - v.v[1] + 1) * CFixed(viewportHeight >> 1) + 0.5f;
//  v.sz = gl_fpdiv(zFar - zNear, v.v[2] << 1) + ((zNear + zFar)>>1);

  // --------
  // Lighting
  // --------
  if(lightingEnabled_ == true)
  {
    SColorFx c(0, 0, 0, 0);

    // Normal Rotation
    matrixNormal.transform3(v.n, v.n);

      if(lights_[iLight].enabled == true)
      {
        // Ambient light (it's everywhere!)
        c += lights_[iLight].ambient * matColorAmbient_;

        // Diffuse light
        CFixed diffuse = -lights_[iLight].direction.dotProduct(v.n);
        if(diffuse >= 0.0f)
        {
          c += lights_[iLight].diffuse * matColorDiffuse_ * diffuse;
        }

        if(matShininess_ >= 0.5f)
        {
          // Specular light
          TVector<CFixed> eye(0, 0, 1, 1);
          CFixed specular = lights_[iLight].direction.getCrossProduct(v.n).dotProduct(eye);
          if(specular >= 0.0f)
          {
            specular = my_pow(specular, (int)(matShininess_ + 0.5f));
            c += lights_[iLight].specular * matColorSpecular_ * specular;
          }
        }
      }

    // Multiply vertex color by calculated color
    v.cl *= c;
    // Clamp to 0..1
    v.cl.clamp();
  }

  // ---
  // Fog
  // ---
  if(fogEnabled_ == true)
  {
    CFixed partFog, partColor;
    partFog.value = clampfx(gl_fpdiv(abs(v.v[2].value) - fogStart_, fogEnd_ - fogStart_));
    partColor = 1 - partFog;
    v.cl = ((v.cl * partColor) + (fogColor_ * partFog)).getClamped();
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::rasterize(SVertexFx & v)
{
  // Copy vertex into vertex buffer
  *polygon[vertIdx_] = v;

  // ------------------
  // Primitive Assembly
  // ------------------
  switch(rasterMode_)
  {
    case GL_TRIANGLES:
    {
      if(vertIdx_ == 2)
        rasterPoly(polygon);
      vertIdx_ = (vertIdx_ + 1) % 3;
      break;
    }
    case GL_TRIANGLE_STRIP:
    {
      if(vertIdx_ == 2)
      {
        rasterPoly(polygon);
        if(bFlipFlop_ == true)
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
        vertIdx_++;
      break;
    }
    case GL_TRIANGLE_FAN:
    {
      if(vertIdx_ == 2)
      {
        rasterPoly(polygon);
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
        vertIdx_++;
      break;
    }
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::begin(GLenum mode)
{
  rasterMode_ = mode;

  // Initialize for default triangle
  polygon[0] = &vertices[0];
  polygon[1] = &vertices[1];
  polygon[2] = &vertices[2];
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
CSoftGLESFixed::rasterPoly(SVertexFx * vtx[3])
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
      edge1->addZT(vlo->sx, vlo->sy, vlo->v[3], vlo->t[0], vlo->t[1], vhi->sx, vhi->sy, vhi->v[3], vhi->t[0], vhi->t[1]);
      edge2->addZT(vlo->sx, vlo->sy, vlo->v[3], vlo->t[0], vlo->t[1], vmi->sx, vmi->sy, vmi->v[3], vmi->t[0], vmi->t[1]);
      edge2->addZT(vmi->sx, vmi->sy, vmi->v[3], vmi->t[0], vmi->t[1], vhi->sx, vhi->sy, vhi->v[3], vhi->t[0], vhi->t[1]);
    }
    else
    {
      edge1->addT(vlo->sx, vlo->sy, vlo->t[0], vlo->t[1], vhi->sx, vhi->sy, vhi->t[0], vhi->t[1]);
      edge2->addT(vlo->sx, vlo->sy, vlo->t[0], vlo->t[1], vmi->sx, vmi->sy, vmi->t[0], vmi->t[1]);
      edge2->addT(vmi->sx, vmi->sy, vmi->t[0], vmi->t[1], vhi->sx, vhi->sy, vhi->t[0], vhi->t[1]);
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
