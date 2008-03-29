#include "softGLF.h"
#include "matrix.h"
#include "fixedPoint.h"

#include "stdlib.h"
typedef unsigned int wint_t;
#include <math.h>


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoftGLESFloat::CSoftGLESFloat()
 : CAGLESFxToFloatContext()
 , CAGLESBuffers()
 , CAGLESCull()
 , CAGLESMatrixF()
 , CAGLESTextures()

 , depthTestEnabled_(false)
 , depthFunction_(GL_LESS)
 , depthClear_(1.0f)
 , zClearValue_(0xffffffff)
 , zbuffer(0)
 , zNear_(0.0f)
 , zFar_(1.0f)

 , shadingModel_(GL_FLAT)

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
}

//-----------------------------------------------------------------------------
CSoftGLESFloat::~CSoftGLESFloat()
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glClear(GLbitfield mask)
{
  if(mask & GL_COLOR_BUFFER_BIT)
  {
    color_t color = BxColorFormat_FromRGBA(renderSurface->mode.format, (uint8_t)(clClear.r * 255), (uint8_t)(clClear.g * 255), (uint8_t)(clClear.b * 255), (uint8_t)(clClear.a * 255));

    switch(renderSurface->mode.bpp)
    {
      case 8:
      {
        for(uint32_t y(0); y < renderSurface->mode.height; y++)
          for(uint32_t x(0); x < renderSurface->mode.width; x++)
            ((uint8_t  *)renderSurface->p)[(y + viewportYOffset) * renderSurface->mode.xpitch + (x + viewportXOffset)] = color;
        break;
      }
      case 16:
      {
        for(uint32_t y(0); y < renderSurface->mode.height; y++)
          for(uint32_t x(0); x < renderSurface->mode.width; x++)
            ((uint16_t *)renderSurface->p)[(y + viewportYOffset) * renderSurface->mode.xpitch + (x + viewportXOffset)] = color;
        break;
      }
      case 32:
      {
        for(uint32_t y(0); y < renderSurface->mode.height; y++)
          for(uint32_t x(0); x < renderSurface->mode.width; x++)
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
  clCurrent.r = (GLfloat)red   * (1.0f/255.0f);
  clCurrent.g = (GLfloat)green * (1.0f/255.0f);
  clCurrent.b = (GLfloat)blue  * (1.0f/255.0f);
  clCurrent.a = (GLfloat)alpha * (1.0f/255.0f);
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
CSoftGLESFloat::glDepthRangef(GLclampf zNear, GLclampf zFar)
{
  zNear_ = clampf(zNear);
  zFar_  = clampf(zFar);
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
    case GL_FOG:        fogEnabled_       = false; break;
    case GL_TEXTURE_2D: texturesEnabled_  = false; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex  (first * bufVertex_.size);
  GLint idxColor   (first * bufColor_.size);
  GLint idxNormal  (first * bufNormal_.size);
  GLint idxTexCoord(first * bufTexCoord_.size);

  SVertexF * polygon[3];
  SVertexF   vertices[3];
  bool bFlipFlop(true);
  polygon[0] = &vertices[0];
  polygon[1] = &vertices[1];
  polygon[2] = &vertices[2];
  GLint idx(0);

  // Process all vertices
  for(GLint i(0); i < count; i++)
  {
    SVertexF & v = *polygon[idx];
    v.bProcessed = false;

    // Vertex
    switch(bufVertex_.type)
    {
      case GL_FLOAT:
        v.v[0] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.v[1] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.v[2] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.v[3] = 1.0f;
        break;
      case GL_FIXED:
        v.v[0] = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.v[1] = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.v[2] = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.v[3] = 1.0f;
        break;
    };

    // Normal
    if(bBufNormalEnabled_ == true)
    {
      switch(bufNormal_.type)
      {
        case GL_FLOAT:
          v.n[0] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n[1] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n[2] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n[3] = 1.0f;
          break;
        case GL_FIXED:
          v.n[0] = gl_fptof(((GLfixed *)bufNormal_.pointer)[idxNormal++]);
          v.n[1] = gl_fptof(((GLfixed *)bufNormal_.pointer)[idxNormal++]);
          v.n[2] = gl_fptof(((GLfixed *)bufNormal_.pointer)[idxNormal++]);
          v.n[3] = 1.0f;
          break;
      };
    }

    // Textures/Colors
    if(texturesEnabled_ == true)
    {
      // Textures
      if(bBufTexCoordEnabled_ == true)
      {
        switch(bufTexCoord_.type)
        {
          case GL_FLOAT:
            v.ts =         ((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++];
            v.tt = (1.0f - ((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++]);
            break;
          case GL_FIXED:
            v.ts =         gl_fptof(((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]);
            v.tt = (1.0f - gl_fptof(((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]));
            break;
        };
      }
    }
    else
    {
      // Color
      if(bBufColorEnabled_ == true)
      {
        switch(bufColor_.type)
        {
          case GL_FLOAT:
            v.c.r = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v.c.g = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v.c.b = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v.c.a = ((GLfloat *)bufColor_.pointer)[idxColor++];
            break;
          case GL_FIXED:
            v.c.r = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
            v.c.g = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
            v.c.b = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
            v.c.a = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
            break;
        };
      }
    }

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
            SVertexF * pTemp = polygon[0];
            polygon[0] = polygon[2];
            polygon[2] = pTemp;
          }
          else
          {
            SVertexF * pTemp = polygon[1];
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
    case GL_TEXTURE_2D: texturesEnabled_   = true; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glFinish(void)
{
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
CSoftGLESFloat::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
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
  zbuffer            = new uint32_t[width * height];
  edge1              = new CEdgeF(viewportHeight);
  edge2              = new CEdgeF(viewportHeight);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CSoftGLESFloat::plotPoly(SVertexF * vtx[3])
{
  for(int i(0); i < 3; i++)
  {
    if(vtx[i]->bProcessed == false)
    {
      GLfloat * v = vtx[i]->v;

      // Model-View matrix
      //   from 'object coordinates' to 'eye coordinates'
      matrixModelView.transform(v, v);
      // Projection matrix
      //   from 'eye coordinates' to 'clip coordinates'
      matrixProjection.transform(v, v);
      // Perspective division
      //   from 'clip coordinates' to 'normalized device coordinates'
      v[0] /= v[3];
      v[1] /= v[3];
      v[2] /= v[3];
      // Viewport transformation
      //   from 'normalized device coordinates' to 'window coordinates'
      vtx[i]->sx = (GLint)(( v[0] + 1.0f) * (viewportWidth  >> 1));
      vtx[i]->sy = (GLint)((-v[1] + 1.0f) * (viewportHeight >> 1));
//      vtx[i]->sz = (GLint)(((zFar - zNear) / (2.0f * v[2])) + ((zNear + zFar) / 2.0f));

      vtx[i]->bProcessed = true;
    }
  }

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

/*
  if(texturesEnabled_ == false)
  {
    // Lighting
    if(lightingEnabled_ == true)
    {
      // Normal Rotation
      matrixRotation.transform(vtx[0]->n, vtx[0]->n);
      matrixRotation.transform(vtx[1]->n, vtx[1]->n);
      matrixRotation.transform(vtx[2]->n, vtx[2]->n);
      // FIXME: Light value of normal
      GLfloat normal[3] = {abs(vtx[0]->n[2]), abs(vtx[1]->n[2]), abs(vtx[2]->n[2])};

      for(int iLight(0); iLight < 8; iLight++)
      {
        if(lights_[iLight].enabled == true)
        {
          SColorF & ambient = lights_[iLight].ambient;
          SColorF & diffuse = lights_[iLight].diffuse;

          vtx[0]->c.r = clampf((vtx[0]->c.r * ambient.r) + ((vtx[0]->c.r * normal[0]) * diffuse.r));
          vtx[0]->c.g = clampf((vtx[0]->c.g * ambient.g) + ((vtx[0]->c.g * normal[0]) * diffuse.g));
          vtx[0]->c.b = clampf((vtx[0]->c.b * ambient.b) + ((vtx[0]->c.b * normal[0]) * diffuse.b));

          vtx[1]->c.r = clampf((vtx[1]->c.r * ambient.r) + ((vtx[1]->c.r * normal[1]) * diffuse.r));
          vtx[1]->c.g = clampf((vtx[1]->c.g * ambient.g) + ((vtx[1]->c.g * normal[1]) * diffuse.g));
          vtx[1]->c.b = clampf((vtx[1]->c.b * ambient.b) + ((vtx[1]->c.b * normal[1]) * diffuse.b));

          vtx[2]->c.r = clampf((vtx[2]->c.r * ambient.r) + ((vtx[2]->c.r * normal[2]) * diffuse.r));
          vtx[2]->c.g = clampf((vtx[2]->c.g * ambient.g) + ((vtx[2]->c.g * normal[2]) * diffuse.g));
          vtx[2]->c.b = clampf((vtx[2]->c.b * ambient.b) + ((vtx[2]->c.b * normal[2]) * diffuse.b));
        }
      }
    }

    // Fog
    if(fogEnabled_ == true)
    {
      for(int i(0); i < 3; i++)
      {
        GLfloat partFog   = clampf((abs(vtx[i]->v[2]) - fogStart_) / (fogEnd_ - fogStart_));
        GLfloat partColor = 1.0f - partFog;
        vtx[i]->c.r = clampf((vtx[i]->c.r * partColor) + (fogColor_.r * partFog));
        vtx[i]->c.g = clampf((vtx[i]->c.g * partColor) + (fogColor_.g * partFog));
        vtx[i]->c.b = clampf((vtx[i]->c.b * partColor) + (fogColor_.b * partFog));
      }
    }
  }
*/

  rasterPoly(vtx);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::rasterPoly(SVertexF * vtx[3])
{
  // Bubble sort the 3 vertexes
  SVertexF * vtemp;
  SVertexF * vhi(vtx[0]);
  SVertexF * vmi(vtx[1]);
  SVertexF * vlo(vtx[2]);

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
      edge1->addZC(vlo->sx, vlo->sy, vlo->v[3], vlo->c, vhi->sx, vhi->sy, vhi->v[3], vhi->c);
      edge2->addZC(vlo->sx, vlo->sy, vlo->v[3], vlo->c, vmi->sx, vmi->sy, vmi->v[3], vmi->c);
      edge2->addZC(vmi->sx, vmi->sy, vmi->v[3], vmi->c, vhi->sx, vhi->sy, vhi->v[3], vhi->c);
    }
    else
    {
      edge1->addC(vlo->sx, vlo->sy, vlo->c, vhi->sx, vhi->sy, vhi->c);
      edge2->addC(vlo->sx, vlo->sy, vlo->c, vmi->sx, vmi->sy, vmi->c);
      edge2->addC(vmi->sx, vmi->sy, vmi->c, vhi->sx, vhi->sy, vhi->c);
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
        hlineZ(*pEdgeLeft, *pEdgeRight, y, vtx[2]->c);
    else
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hline(*pEdgeLeft, *pEdgeRight, y, vtx[2]->c);
  }
}
