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
/*
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
*/
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
            ((uint8_t  *)renderSurface->p)[y * renderSurface->mode.xpitch + x] = color;
        break;
      }
      case 16:
      {
        for(uint32_t y(0); y < renderSurface->mode.width; y++)
          for(uint32_t x(0); x < renderSurface->mode.height; x++)
            ((uint16_t *)renderSurface->p)[y * renderSurface->mode.xpitch + x] = color;
        break;
      }
      case 32:
      {
        for(uint32_t y(0); y < renderSurface->mode.width; y++)
          for(uint32_t x(0); x < renderSurface->mode.height; x++)
            ((uint32_t *)renderSurface->p)[y * renderSurface->mode.xpitch + x] = color;
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

  //if((enableCapabilities_ & GL_NORMALIZE) == GL_NORMALIZE)
  //{
  //  // FIXME: Normalize normal
  //}
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
    v.bProcessed = false;

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

    // Textures/Colors
    if(texturesEnabled_ == true)
    {
      // Textures
      if(bBufTexCoordEnabled_ == true)
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
    }
    else
    {
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
void
CSoftGLESFixed::plotPoly(SVertexFx * vtx[3])
{
  for(int i(0); i < 3; i++)
  {
    if(vtx[i]->bProcessed == false)
    {
      GLfixed * v = vtx[i]->v;

      // Model-View matrix
      //   from 'object coordinates' to 'eye coordinates'
      matrixModelView.transform(v, v);
      // Projection matrix
      //   from 'eye coordinates' to 'clip coordinates'
      matrixProjection.transform(v, v);
      // Perspective division
      //   from 'clip coordinates' to 'normalized device coordinates'
      v[0] = gl_fpdiv(v[0], v[3]);
      v[1] = gl_fpdiv(v[1], v[3]);
      v[2] = gl_fpdiv(v[2], v[3]);
      // Viewport transformation
      //   from 'normalized device coordinates' to 'window coordinates'
      vtx[i]->sx = gl_fptoi(gl_fpmul(( v[0] + gl_fpfromi(1)), gl_fpfromi(viewportWidth  >> 1))) + viewportXOffset;
      vtx[i]->sy = gl_fptoi(gl_fpmul((-v[1] + gl_fpfromi(1)), gl_fpfromi(viewportHeight >> 1))) + viewportYOffset;
//      vtx[i]->sz = gl_fpdiv(zFar - zNear, v[2] << 1) + ((zNear + zFar)>>1);

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
      GLfixed normal[3] = {abs(vtx[0]->n[2]), abs(vtx[1]->n[2]), abs(vtx[2]->n[2])};

      for(int iLight(0); iLight < 8; iLight++)
      {
        if(lights_[iLight].enabled == true)
        {
          SColorFx & ambient = lights_[iLight].ambient;
          SColorFx & diffuse = lights_[iLight].diffuse;

          vtx[0]->c.r = clampfx(gl_fpmul(vtx[0]->c.r, ambient.r) + gl_fpmul(gl_fpmul(vtx[0]->c.r, normal[0]), diffuse.r));
          vtx[0]->c.g = clampfx(gl_fpmul(vtx[0]->c.g, ambient.g) + gl_fpmul(gl_fpmul(vtx[0]->c.g, normal[0]), diffuse.g));
          vtx[0]->c.b = clampfx(gl_fpmul(vtx[0]->c.b, ambient.b) + gl_fpmul(gl_fpmul(vtx[0]->c.b, normal[0]), diffuse.b));

          vtx[1]->c.r = clampfx(gl_fpmul(vtx[1]->c.r, ambient.r) + gl_fpmul(gl_fpmul(vtx[1]->c.r, normal[1]), diffuse.r));
          vtx[1]->c.g = clampfx(gl_fpmul(vtx[1]->c.g, ambient.g) + gl_fpmul(gl_fpmul(vtx[1]->c.g, normal[1]), diffuse.g));
          vtx[1]->c.b = clampfx(gl_fpmul(vtx[1]->c.b, ambient.b) + gl_fpmul(gl_fpmul(vtx[1]->c.b, normal[1]), diffuse.b));

          vtx[2]->c.r = clampfx(gl_fpmul(vtx[2]->c.r, ambient.r) + gl_fpmul(gl_fpmul(vtx[2]->c.r, normal[2]), diffuse.r));
          vtx[2]->c.g = clampfx(gl_fpmul(vtx[2]->c.g, ambient.g) + gl_fpmul(gl_fpmul(vtx[2]->c.g, normal[2]), diffuse.g));
          vtx[2]->c.b = clampfx(gl_fpmul(vtx[2]->c.b, ambient.b) + gl_fpmul(gl_fpmul(vtx[2]->c.b, normal[2]), diffuse.b));
        }
      }
    }

    // Fog
    if(fogEnabled_ == true)
    {
      for(int i(0); i < 3; i++)
      {
        GLfixed partFog   = clampfx(gl_fpdiv(abs(vtx[i]->v[2]) - fogStart_, fogEnd_ - fogStart_));
        GLfixed partColor = gl_fpfromi(1) - partFog;
        vtx[i]->c.r = clampfx(gl_fpmul(vtx[i]->c.r, partColor) + gl_fpmul(fogColor_.r, partFog));
        vtx[i]->c.g = clampfx(gl_fpmul(vtx[i]->c.g, partColor) + gl_fpmul(fogColor_.g, partFog));
        vtx[i]->c.b = clampfx(gl_fpmul(vtx[i]->c.b, partColor) + gl_fpmul(fogColor_.b, partFog));
      }
    }
  }
*/

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
        hlineZ(*pEdgeLeft, *pEdgeRight, y, vtx[2]->c);
    else
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hline(*pEdgeLeft, *pEdgeRight, y, vtx[2]->c);
  }
}
