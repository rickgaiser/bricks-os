#include "glesContext.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "gif.h"
#include "stdlib.h"


#define clamp(f)   (f < 0.0f ? 0.0f : (f > 1.0f ? 1.0f : f))
DECLARE_GS_PACKET(dma_buf,1000);
extern uint32_t   gs_mem_current;
extern uint16_t   gs_origin_x;
extern uint16_t   gs_origin_y;
uint32_t          ps2TexturesStart_(0);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPS2GLESContext::CPS2GLESContext()
 : CAGLESFxToFloatContext()
 , CAGLESBuffers()
 , CAGLESCull()
 , CAGLESMatrixF()
 , CAGLESTexturesPS2()

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
 , viewportXOffset(0)
 , viewportYOffset(0)
 , viewportPixelCount(0)
 , viewportWidth(0)
 , viewportHeight(0)

 , ps2Shading_(SHADE_FLAT)
 , ps2Textures_(TEXTURES_OFF)
 , ps2Fog_(FOG_OFF)
 , ps2AlphaBlend_(ALPHABLEND_OFF)
 , ps2Aliasing_(ALIASING_ON)
 , ps2DepthFunction_(ZTST_GREATER)
 , ps2DepthInvert_(true)
 , ps2ZMax_(0xffff)
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

  // Initialize the DMA buffer
  BEGIN_GS_PACKET(dma_buf);
  GIF_TAG_AD(dma_buf, 1, 0, 0, 0);
}

//-----------------------------------------------------------------------------
CPS2GLESContext::~CPS2GLESContext()
{
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glClear(GLbitfield mask)
{
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    GIF_DATA_AD(dma_buf, test_1, GS_TEST(0, 0, 0, 0, 0, 0, 1, ZTST_ALWAYS));
  }

  if(mask & GL_COLOR_BUFFER_BIT)
  {
    uint8_t r = (uint8_t)(clClear.r * 255);
    uint8_t g = (uint8_t)(clClear.g * 255);
    uint8_t b = (uint8_t)(clClear.b * 255);

    GIF_DATA_AD(dma_buf, prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
    GIF_DATA_AD(dma_buf, rgbaq, GS_RGBAQ(r, g, b, 0x80, 0));
    GIF_DATA_AD(dma_buf, xyz2, GS_XYZ2((gs_origin_x+0)<<4, (gs_origin_x+0)<<4, 0));
    GIF_DATA_AD(dma_buf, xyz2, GS_XYZ2((gs_origin_x+viewportWidth+1)<<4, (gs_origin_x+viewportHeight+1)<<4, 0));
  }

  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    GIF_DATA_AD(dma_buf, test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  clClear.r = red;
  clClear.g = green;
  clClear.b = blue;
  clClear.a = alpha;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glClearDepthf(GLclampf depth)
{
  depthClear_ = depth;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  clCurrent.r = (GLfloat)red   / 255.0f;
  clCurrent.g = (GLfloat)green / 255.0f;
  clCurrent.b = (GLfloat)blue  / 255.0f;
  clCurrent.a = (GLfloat)alpha / 255.0f;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  clCurrent.r = red;
  clCurrent.g = green;
  clCurrent.b = blue;
  clCurrent.a = alpha;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glDepthRangef(GLclampf zNear, GLclampf zFar)
{
  zNear_ = clampf(zNear);
  zFar_  = clampf(zFar);
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
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
CPS2GLESContext::glDepthFunc(GLenum func)
{
  depthFunction_ = func;

  switch(func)
  {
    case GL_LESS:     ps2DepthFunction_ = ZTST_GREATER; ps2DepthInvert_ = true;  break;
//    case GL_EQUAL:    ps2DepthFunction_ = ??; break;
    case GL_LEQUAL:   ps2DepthFunction_ = ZTST_GEQUAL;  ps2DepthInvert_ = true;  break;
    case GL_GREATER:  ps2DepthFunction_ = ZTST_GREATER; ps2DepthInvert_ = false; break;
//    case GL_NOTEQUAL: ps2DepthFunction_ = ??; break;
    case GL_GEQUAL:   ps2DepthFunction_ = ZTST_GEQUAL;  ps2DepthInvert_ = false; break;
    case GL_NEVER:    ps2DepthFunction_ = ZTST_NEVER;   ps2DepthInvert_ = false; break;
    case GL_ALWAYS:
    default:          ps2DepthFunction_ = ZTST_ALWAYS;  ps2DepthInvert_ = false; break;
  }

  if(depthTestEnabled_ == true)
  {
    GIF_DATA_AD(dma_buf, test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glDisable(GLenum cap)
{
  switch(cap)
  {
    case GL_LIGHTING:   lightingEnabled_   = false; break;
    case GL_LIGHT0:     lights_[0].enabled = false; break;
    case GL_LIGHT1:     lights_[1].enabled = false; break;
    case GL_LIGHT2:     lights_[2].enabled = false; break;
    case GL_LIGHT3:     lights_[3].enabled = false; break;
    case GL_LIGHT4:     lights_[4].enabled = false; break;
    case GL_LIGHT5:     lights_[5].enabled = false; break;
    case GL_LIGHT6:     lights_[6].enabled = false; break;
    case GL_LIGHT7:     lights_[7].enabled = false; break;

    case GL_DEPTH_TEST:
    {
      depthTestEnabled_ = false;
      // Z-Buffer
      GIF_DATA_AD(dma_buf, zbuf_1, GS_ZBUF(gs_mem_current >> 13, GRAPH_PSM_16, ZMSK_DISABLE));
      // Z-Buffer test
      GIF_DATA_AD(dma_buf, test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
      break;
    }
    case GL_CULL_FACE:  cullFaceEnabled_ = false; break;
    case GL_FOG:        fogEnabled_      = false; break;
    case GL_TEXTURE_2D: texturesEnabled_ = false; ps2Textures_ = TEXTURES_OFF; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex  (first * bufVertex_.size);
  GLint idxColor   (first * bufColor_.size);
  GLint idxNormal  (first * bufNormal_.size);
  GLint idxTexCoord(first * bufTexCoord_.size);

  SVertexF v;

  switch(mode)
  {
    case GL_TRIANGLES:
      GIF_DATA_AD(dma_buf, prim, GS_PRIM(PRIM_TRI, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
      break;
    case GL_TRIANGLE_STRIP:
      GIF_DATA_AD(dma_buf, prim, GS_PRIM(PRIM_TRI_STRIP, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
      break;
    case GL_TRIANGLE_FAN:
      GIF_DATA_AD(dma_buf, prim, GS_PRIM(PRIM_TRI_FAN, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
      break;
  };

  // Process all vertices
  for(GLint i(0); i < count; i++)
  {
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

    // Textures
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
    else
      v.c = clCurrent;

    // Model-View matrix
    //   from 'object coordinates' to 'eye coordinates'
    matrixModelView.transform(v.v, v.v);
    // Projection matrix
    //   from 'eye coordinates' to 'clip coordinates'
    matrixProjection.transform(v.v, v.v);
    // Perspective division
    //   from 'clip coordinates' to 'normalized device coordinates'
    v.v[0] /= v.v[3];
    v.v[1] /= v.v[3];
    v.v[2] /= v.v[3];
    // Viewport transformation
    //   from 'normalized device coordinates' to 'window coordinates'
    v.sx = (GLint)(( v.v[0] + 1.0f) * (viewportWidth  / 2)) + viewportXOffset;
    v.sy = (GLint)((-v.v[1] + 1.0f) * (viewportHeight / 2)) + viewportYOffset;
    v.sz = (GLint)((-v.v[2] + 1.0f) * 0.5f * ps2ZMax_);

/*
    // Lighting
    if(lightingEnabled_ == true)
    {
      // Normal Rotation
      matrixRotation.transform(v.n, v.n);
      // FIXME: Light value of normal
      GLfloat normal = clampf(-v.n[2]);

      for(int iLight(0); iLight < 8; iLight++)
      {
        if(lights_[iLight].enabled == true)
        {
          SColorF & ambient = lights_[iLight].ambient;
          SColorF & diffuse = lights_[iLight].diffuse;
          v.c.r = clamp((v.c.r * ambient.r) + ((v.c.r * normal) * diffuse.r));
          v.c.g = clamp((v.c.g * ambient.g) + ((v.c.g * normal) * diffuse.g));
          v.c.b = clamp((v.c.b * ambient.b) + ((v.c.b * normal) * diffuse.b));
        }
      }
    }

    // Fog
    if(fogEnabled_ == true)
    {
      GLfloat partFog   = clamp((abs(v.v[3]) - fogStart_) / (fogEnd_ - fogStart_));
      GLfloat partColor = 1.0f - partFog;
      v.c.r = clamp((v.c.r * partColor) + (fogColor_.r * partFog));
      v.c.g = clamp((v.c.g * partColor) + (fogColor_.g * partFog));
      v.c.b = clamp((v.c.b * partColor) + (fogColor_.b * partFog));
    }
*/

    // Calculate Z
    uint32_t z = (ps2DepthInvert_ == false) ? (ps2ZMax_ - v.sz) : v.sz;

    // Add to message
    if(texturesEnabled_ == true)
    {
      v.ts /= v.v[3];
      v.tt /= v.v[3];
      float tq = 1.0f / v.v[3];

      GIF_DATA_AD(dma_buf, st, GS_ST(*(uint32_t *)(&v.ts), *(uint32_t *)(&v.tt)));
      GIF_DATA_AD(dma_buf, rgbaq, GS_RGBAQ((uint8_t)(v.c.r*255), (uint8_t)(v.c.g*255), (uint8_t)(v.c.b*255), 100, *(uint32_t *)(&tq)));
    }
    else
    {
      GIF_DATA_AD(dma_buf, rgbaq, GS_RGBAQ((uint8_t)(v.c.r*255), (uint8_t)(v.c.g*255), (uint8_t)(v.c.b*255), 100, 0));
    }
    GIF_DATA_AD(dma_buf, xyz2, GS_XYZ2((gs_origin_x+v.sx)<<4, (gs_origin_y+v.sy)<<4, z));
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glEnable(GLenum cap)
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

    case GL_DEPTH_TEST:
    {
      depthTestEnabled_ = true;
      // Z-Buffer
      switch(renderSurface->bpp_)
      {
        case 16:
        {
          GIF_DATA_AD(dma_buf, zbuf_1, GS_ZBUF(gs_mem_current >> 13, GRAPH_PSM_16, ZMSK_ENABLE));
          ps2TexturesStart_ = gs_mem_current + (renderSurface->width_ * renderSurface->height_ * 2);
          ps2ZMax_ = 0xffff;
          break;
        }
        case 24:
        {
          GIF_DATA_AD(dma_buf, zbuf_1, GS_ZBUF(gs_mem_current >> 13, GRAPH_PSM_24, ZMSK_ENABLE));
          ps2TexturesStart_ = gs_mem_current + (renderSurface->width_ * renderSurface->height_ * 3);
          ps2ZMax_ = 0xffffff;
          break;
        }
        case 32:
        {
          GIF_DATA_AD(dma_buf, zbuf_1, GS_ZBUF(gs_mem_current >> 13, GRAPH_PSM_32, ZMSK_ENABLE));
          ps2TexturesStart_ = gs_mem_current + (renderSurface->width_ * renderSurface->height_ * 4);
          ps2ZMax_ = 0xffffffff;
          break;
        }
      };
      // Z-Buffer test
      GIF_DATA_AD(dma_buf, test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
      break;
    }
    case GL_CULL_FACE:  cullFaceEnabled_   = true; break;
    case GL_FOG:        fogEnabled_        = true; break;
    case GL_TEXTURE_2D: texturesEnabled_   = true; ps2Textures_ = TEXTURES_ON; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glFinish(void)
{
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glFlush(void)
{
  // Kick off the DMA transfer to the GS
  SEND_GS_PACKET(dma_buf);

  // Initialize the DMA buffer
  BEGIN_GS_PACKET(dma_buf);
  GIF_TAG_AD(dma_buf, 1, 0, 0, 0);
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glFogf(GLenum pname, GLfloat param)
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
CPS2GLESContext::glFogfv(GLenum pname, const GLfloat * params)
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
CPS2GLESContext::glLightf(GLenum light, GLenum pname, GLfloat param)
{
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
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
CPS2GLESContext::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
  switch(mode)
  {
    case GL_FLAT:   ps2Shading_ = SHADE_FLAT;    break;
    case GL_SMOOTH: ps2Shading_ = SHADE_GOURAUD; break;
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  viewportXOffset    = x;
  viewportYOffset    = y;
  viewportWidth      = width;
  viewportHeight     = height;
  viewportPixelCount = width * height;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IGLESContext *
getGLESContext()
{
  return new CPS2GLESContext;
}
