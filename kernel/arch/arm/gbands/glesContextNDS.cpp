#include "glesContextNDS.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
#include "math.h"


#define fpgl_to_ndsRGB555(r,g,b) \
  ((((b*31) >>  6) & 0x00007c00) | \
   (((g*31) >> 11) & 0x000003e0) | \
   (((r*31) >> 16) & 0x0000001f))
#define fp_to_ndsRGB555(r,g,b) \
  ((((b.value*31) >>  6) & 0x00007c00) | \
   (((g.value*31) >> 11) & 0x000003e0) | \
   (((r.value*31) >> 16) & 0x0000001f))
#define fp_to_ndsRGBA5555(r,g,b,a) \
  ((((a.value*31)      ) & 0x001f0000) | \
   (((b.value*31) >>  6) & 0x00007c00) | \
   (((g.value*31) >> 11) & 0x000003e0) | \
   (((r.value*31) >> 16) & 0x0000001f))
#define fp_to_ndsNormal(x,y,z) \
  (((gl_to_ndsn(z.value) & 0x3ff) << 20) | \
   ((gl_to_ndsn(y.value) & 0x3ff) << 10) | \
   ((gl_to_ndsn(x.value) & 0x3ff)      ))
#define fp_to_ndsTexCoord(s,t) \
  (((gl_to_ndst(s.value) & 0xffff) << 16) | \
   ((gl_to_ndst(t.value) & 0xffff)      ))

// mode bits
#define NDS_RGB32_A3   1 // 32 color palette, 3 bits of alpha
#define NDS_RGB4       2 // 4 color palette
#define NDS_RGB16      3 // 16 color palette
#define NDS_RGB256     4 // 256 color palette
#define NDS_COMPRESSED 5 // compressed texture
#define NDS_RGB8_A5    6 // 8 color palette, 5 bits of alpha
#define NDS_RGBA       7 // 15 bit direct color, 1 bit of alpha
#define NDS_RGB        8 // 15 bit direct color, alpha bit autoset to 1

#define NDS_REPEAT_S    (1<<16)
#define NDS_REPEAT_T    (1<<17)
#define NDS_FLIP_S      (1<<18)
#define NDS_FLIP_T      (1<<19)

#define TEXGEN_OFF      (0<<30)  //unmodified texcoord
#define TEXGEN_TEXCOORD (1<<30)  //texcoord * texture-matrix
#define TEXGEN_NORMAL   (2<<30)  //normal * texture-matrix
#define TEXGEN_POSITION (3<<30)  //vertex * texture-matrix


bool      CNDSGLESContext::bInitialized_(false);
NDSfixed  CNDSGLESContext::fpSin_[360];
NDSfixed  CNDSGLESContext::fpCos_[360];


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CNDSGLESContext::CNDSGLESContext()
 : CASoftGLESFixed()
 , matrixMode_(GL_MODELVIEW)
 , ndsCurrentMatrixId_(NDS_MODELVIEW)
 , pCurrentTex_(NULL)
{
  zMax_ = 0x7fff;

  // Power control
  REG_POWCNT |= POWER_LCD | POWER_2D_TOP | POWER_2D_BOTTOM | POWER_3D_CORE | POWER_3D_MATRIX;
  // Display control
  REG_DISPCNT = MODE_0 | BG0_ENABLE | ENABLE_BG03D | DISP_SOURCE_ENGINE;

  iNDSGFXControl_ = 0;//NDS_SHADING_HIGHLIGHT;
  iNDSPolyFormat_ = NDS_POLY_ALPHA(31) | NDS_CULL_BACK;// | NDS_PM_MODULATION | NDS_POLY_FOG;

  GFX_CONTROL     = iNDSGFXControl_;
  GFX_POLY_FORMAT = iNDSPolyFormat_;
  GFX_CLEAR_DEPTH = zMax_;

  // Set the fog density table
  for(int i(0); i < 32; i++)
    GFX_FOG_TABLE[i] = (i << 2);

  if(bInitialized_ == false)
  {
    bInitialized_ = true;
    for(int i(0); i < 360; i++)
    {
      fpSin_[i] = nds_fpfromf(sin(static_cast<float>(i) * M_PI / 180.0f));
      fpCos_[i] = nds_fpfromf(cos(static_cast<float>(i) * M_PI / 180.0f));
    }
  }

  for(GLuint idx(0); idx < MAX_TEXTURE_COUNT; idx++)
    textures_[idx].used = false;
}

//-----------------------------------------------------------------------------
CNDSGLESContext::~CNDSGLESContext()
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glBindTexture(GLenum target, GLuint texture)
{
  if(target != GL_TEXTURE_2D)
  {
    setError(GL_INVALID_ENUM);
    return;
  }
  if(texture >= MAX_TEXTURE_COUNT)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  pCurrentTex_ = &textures_[texture];
  GFX_TEX_FORMAT = pCurrentTex_->format;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClear(GLbitfield mask)
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  CASoftGLESFixed::glClearColorx(red, green, blue, alpha);

  GFX_CLEAR_COLOR = fp_to_ndsRGBA5555(clClear.r, clClear.g, clClear.b, clClear.a);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClearDepthx(GLclampx depth)
{
  CASoftGLESFixed::glClearDepthx(depth);

  GFX_CLEAR_DEPTH = zClearValue_;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glCullFace(GLenum mode)
{
  CASoftGLESFixed::glCullFace(mode);

  if(cullFaceEnabled_ == true)
  {
    switch(cullFaceMode_)
    {
      case GL_FRONT:          iNDSPolyFormat_ = (iNDSPolyFormat_ & ~NDS_CULL_MASK) | NDS_CULL_FRONT;          break;
      case GL_BACK:           iNDSPolyFormat_ = (iNDSPolyFormat_ & ~NDS_CULL_MASK) | NDS_CULL_BACK;           break;
      case GL_FRONT_AND_BACK: iNDSPolyFormat_ = (iNDSPolyFormat_ & ~NDS_CULL_MASK) | NDS_CULL_BACK_AND_FRONT; break;
    };
    GFX_POLY_FORMAT = iNDSPolyFormat_;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  if(n < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  for(GLsizei i(0); i < n; i++)
    if(textures[i] < MAX_TEXTURE_COUNT)
      textures_[textures[i]].used = false;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glDisable(GLenum cap)
{
  CASoftGLESFixed::glDisable(cap);

  switch(cap)
  {
    case GL_LIGHTING:
    case GL_LIGHT0:
    case GL_LIGHT1:
    case GL_LIGHT2:
    case GL_LIGHT3:
      updateLights();
      break;

    case GL_CULL_FACE:  iNDSPolyFormat_ &= ~NDS_CULL_MASK;  GFX_POLY_FORMAT = iNDSPolyFormat_; break;
    case GL_FOG:        iNDSGFXControl_ &= ~NDS_FOG;        GFX_CONTROL     = iNDSGFXControl_; break;
    case GL_TEXTURE_2D: iNDSGFXControl_ &= ~NDS_TEXTURE_2D; GFX_CONTROL     = iNDSGFXControl_; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glEnable(GLenum cap)
{
  CASoftGLESFixed::glEnable(cap);

  switch(cap)
  {
    case GL_LIGHTING:
    case GL_LIGHT0:
    case GL_LIGHT1:
    case GL_LIGHT2:
    case GL_LIGHT3:
      updateLights();
      break;

    case GL_CULL_FACE:  glCullFace(cullFaceMode_);                                            break;
    case GL_FOG:        iNDSGFXControl_ |= NDS_FOG;        GFX_CONTROL     = iNDSGFXControl_; break;
    case GL_TEXTURE_2D: iNDSGFXControl_ |= NDS_TEXTURE_2D; GFX_CONTROL     = iNDSGFXControl_; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFlush(void)
{
  CASoftGLESFixed::glFlush();

  GFX_FLUSH = 2;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFogx(GLenum pname, GLfixed param)
{
  CASoftGLESFixed::glFogx(pname, param);

  updateFog();
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFogxv(GLenum pname, const GLfixed * params)
{
  CASoftGLESFixed::glFogxv(pname, params);

  switch(pname)
  {
    case GL_FOG_COLOR:
      GFX_FOG_COLOR = fp_to_ndsRGBA5555(fogColor_.r, fogColor_.g, fogColor_.b, fogColor_.a);
      break;
  };
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glLightx(GLenum light, GLenum pname, GLfixed param)
{
  CASoftGLESFixed::glLightx(light, pname, param);

  updateLights();
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
  CASoftGLESFixed::glLightxv(light, pname, params);

  updateLights();
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
  if(pname == GL_SHININESS)
  {
    // Setup shinyness table
    uint8_t * shiny8 = (uint8_t *)ndsMatShinyness_;
    for(int i(0); i < (128 * 2); i += 2)
      shiny8[i>>1] = i;
    for(int i(0); i < (128 / 4); i++)
      GFX_SHININESS = ndsMatShinyness_[i];
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glMaterialxv(GLenum face, GLenum pname, const GLfixed * params)
{
  switch(pname)
  {
    case GL_AMBIENT:
      ndsMatColorAmbient_ = fpgl_to_ndsRGB555(params[0], params[1], params[2]);
      GFX_DIFFUSE_AMBIENT = (ndsMatColorDiffuse_  << 16) | ndsMatColorAmbient_;
      break;
    case GL_DIFFUSE:
      ndsMatColorDiffuse_ = fpgl_to_ndsRGB555(params[0], params[1], params[2]);
      GFX_DIFFUSE_AMBIENT = (ndsMatColorDiffuse_  << 16) | ndsMatColorAmbient_;
      break;
    case GL_SPECULAR:
      ndsMatColorSpecular_  = 0x8000 | fpgl_to_ndsRGB555(params[0], params[1], params[2]);
      GFX_SPECULAR_EMISSION = (ndsMatColorSpecular_ << 16) | ndsMatColorEmission_;
      break;
    case GL_EMISSION:
      ndsMatColorEmission_  = fpgl_to_ndsRGB555(params[0], params[1], params[2]);
      GFX_SPECULAR_EMISSION = (ndsMatColorSpecular_ << 16) | ndsMatColorEmission_;
      break;
    case GL_SHININESS:
//      matShininess_.value = params[0];
      break;
    case GL_AMBIENT_AND_DIFFUSE:
      ndsMatColorAmbient_ = ndsMatColorDiffuse_ = fpgl_to_ndsRGB555(params[0], params[1], params[2]);
      GFX_DIFFUSE_AMBIENT = (ndsMatColorDiffuse_  << 16) | ndsMatColorAmbient_;
      break;
    default:
      return;
  }
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv((zNear << 1), (right - left)));
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv((right + left), (right - left)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv((zNear << 1), (top - bottom)));
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv((top + bottom), (top - bottom)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((zFar + zNear), (zFar - zNear)));
  MATRIX_MULT4x4 = nds_fpfromi(-1);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

//  MATRIX_STORE = ndsCurrentMatrixId_;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glGenTextures(GLsizei n, GLuint *textures)
{
  if(n < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  for(GLsizei i(0); i < n; i++)
  {
    bool bFound(false);

    for(GLuint idx(0); idx < MAX_TEXTURE_COUNT; idx++)
    {
      if(textures_[idx].used == false)
      {
        textures_[idx].used = true;
        textures[i] = idx;
        bFound = true;
        break;
      }
    }

    if(bFound = false)
      break;
  }
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glLoadIdentity(void)
{
  MATRIX_IDENTITY = 0;
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glLoadMatrixx(const GLfixed *m)
{
  MATRIX_LOAD4x4 = gl_to_ndsv(m[0]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[1]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[2]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[3]);

  MATRIX_LOAD4x4 = gl_to_ndsv(m[4]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[5]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[6]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[7]);

  MATRIX_LOAD4x4 = gl_to_ndsv(m[8]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[9]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[10]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[11]);

  MATRIX_LOAD4x4 = gl_to_ndsv(m[12]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[13]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[14]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[15]);
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_PROJECTION: ndsCurrentMatrixId_ = NDS_PROJECTION; break;
    //case GL_???:        ndsCurrentMatrixId_ = NDS_POSITION;   break;
    case GL_MODELVIEW:  ndsCurrentMatrixId_ = NDS_MODELVIEW;  break;
    case GL_TEXTURE:    ndsCurrentMatrixId_ = NDS_TEXTURE;    break;
  };

  MATRIX_CONTROL = ndsCurrentMatrixId_;
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glMultMatrixx(const GLfixed *m)
{
  MATRIX_MULT4x4 = gl_to_ndsv(m[0]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[1]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[2]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[3]);

  MATRIX_MULT4x4 = gl_to_ndsv(m[4]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[5]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[6]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[7]);

  MATRIX_MULT4x4 = gl_to_ndsv(m[8]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[9]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[10]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[11]);

  MATRIX_MULT4x4 = gl_to_ndsv(m[12]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[13]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[14]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[15]);
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv(gl_fpfromi(2), (right - left)));
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((right + left), (right - left)));

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv(gl_fpfromi(2), (top - bottom)));
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((top + bottom), (top - bottom)));

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((zFar + zNear), (zFar - zNear)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear)));
  MATRIX_MULT4x4 = nds_fpfromi(1);

//  MATRIX_STORE = ndsCurrentMatrixId_;
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  // Normalize the angle
  angle = angle - gl_fpfromi((gl_fptoi(angle) / 360) * 360);
  if(angle < 0)
    angle += gl_fpfromi(360);

  // Get sin and cos from lookup table
  NDSfixed iSin = fpSin_[gl_fptoi(angle) % 360];
  NDSfixed iCos = fpCos_[gl_fptoi(angle) % 360];

  // Convert from gl fixed to nds fixed
  iSin = -iSin; // Why???
  x = gl_to_ndsv(x);
  y = gl_to_ndsv(y);
  z = gl_to_ndsv(z);

  long flags(((z != 0) << 2) | ((y != 0) << 1) | (x != 0));
  switch(flags)
  {
    case 0x00:
    {
      break;
    }
    case 0x01:
    {
      // X Rotation only
      MATRIX_MULT3x3 = nds_fpfromi(1);
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = -iSin;

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iSin;
      MATRIX_MULT3x3 = iCos;
      break;
    }
    case 0x02:
    {
      // Y Rotation only
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iSin;

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(1);
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = -iSin;
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iCos;
      break;
    }
    case 0x04:
    {
      // Z Rotation only
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = iSin;
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = -iSin;
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(1);
      break;
    }
    default:
    {
      // Mixed Rotation
      NDSfixed iMinCos = nds_fpfromi(1) - iCos;
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(x, x), iMinCos) + iCos;
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(x, y), iMinCos) - nds_fpmul(z, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(x, z), iMinCos) + nds_fpmul(y, iSin);

      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(y, x), iMinCos) + nds_fpmul(z, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(y, y), iMinCos) + iCos;
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(y, z), iMinCos) - nds_fpmul(x, iSin);

      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(z, x), iMinCos) - nds_fpmul(y, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(z, y), iMinCos) + nds_fpmul(x, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(z, z), iMinCos) + iCos;
    }
  };
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_SCALE = gl_to_ndsv(x);
  MATRIX_SCALE = gl_to_ndsv(y);
  MATRIX_SCALE = gl_to_ndsv(z);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
  if(target != GL_TEXTURE_2D)
  {
    setError(GL_INVALID_ENUM);
    return;
  }
  if(level < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  // FIXME: MipMaps not supported
  if(level > 0)
    return;

  if((pCurrentTex_ != 0) && (pCurrentTex_->used == true))
  {
    uint32_t idxWidth, idxHeight;
    switch(width)
    {
      case    8: idxWidth = 0; break;
      case   16: idxWidth = 1; break;
      case   32: idxWidth = 2; break;
      case   64: idxWidth = 3; break;
      case  128: idxWidth = 4; break;
      case  256: idxWidth = 5; break;
      case  512: idxWidth = 6; break;
      case 1024: idxWidth = 7; break;
      default:
        setError(GL_INVALID_VALUE);
        return;
    };
    switch(height)
    {
      case    8: idxHeight = 0; break;
      case   16: idxHeight = 1; break;
      case   32: idxHeight = 2; break;
      case   64: idxHeight = 3; break;
      case  128: idxHeight = 4; break;
      case  256: idxHeight = 5; break;
      case  512: idxHeight = 6; break;
      case 1024: idxHeight = 7; break;
      default:
        setError(GL_INVALID_VALUE);
        return;
    };

    pCurrentTex_->width  = width;
    pCurrentTex_->height = height;
    pCurrentTex_->format = TEXGEN_OFF | NDS_REPEAT_S | NDS_REPEAT_T | (idxWidth << 20) | (idxHeight << 23) | (((uint32_t)0x6840000 >> 3) & 0xFFFF) | (NDS_RGBA << 26);
    pCurrentTex_->data   = (void *)0x6840000;

    // Unlock texture memory
    REG_VRAM_C_CR = VRAM_ENABLE | VRAM_TYPE_LCD;

    // Copy to texture memory
    // Convert everything to cfA1B5G5R5 (native NDS format)
    switch(type)
    {
      case GL_UNSIGNED_BYTE:
        for(int i(0); i < (width*height); i++)
          ((uint16_t *)0x6840000)[i] = BxColorFormat_Convert(cfA8B8G8R8, cfA1B5G5R5, ((uint32_t *)pixels)[i]);
        break;
      case GL_UNSIGNED_SHORT_5_6_5:
        for(int i(0); i < (width*height); i++)
          ((uint16_t *)0x6840000)[i] = BxColorFormat_Convert(cfR5G6B5,   cfA1B5G5R5, ((uint16_t *)pixels)[i]);
        break;
      case GL_UNSIGNED_SHORT_4_4_4_4:
        for(int i(0); i < (width*height); i++)
          ((uint16_t *)0x6840000)[i] = BxColorFormat_Convert(cfA4B4G4R4, cfA1B5G5R5, ((uint16_t *)pixels)[i]);
        break;
      case GL_UNSIGNED_SHORT_5_5_5_1:
        //memcpy((void *)0x6840000, pixels, width * height * 2);
        for(int i(0); i < (width*height); i++)
          ((uint16_t *)0x6840000)[i] = ((uint16_t *)pixels)[i] | 0x8000;
        break;
    };

    // Restore texture memory
    REG_VRAM_C_CR = VRAM_ENABLE | VRAM_TYPE_TEXTURE;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  if(target != GL_TEXTURE_2D)
  {
    setError(GL_INVALID_ENUM);
    return;
  }

  if(pCurrentTex_ != 0)
  {
    switch(pname)
    {
      case GL_TEXTURE_WRAP_S:
        switch((GLint)param)
        {
          //case GL_CLAMP:
          case GL_CLAMP_TO_EDGE:
            pCurrentTex_->format &= ~NDS_REPEAT_S;
            break;
          case GL_REPEAT:
            pCurrentTex_->format |= NDS_REPEAT_S;
            break;
        };
        break;
      case GL_TEXTURE_WRAP_T:
        switch((GLint)param)
        {
          //case GL_CLAMP:
          case GL_CLAMP_TO_EDGE:
            pCurrentTex_->format &= ~NDS_REPEAT_T;
            break;
          case GL_REPEAT:
            pCurrentTex_->format |= NDS_REPEAT_T;
            break;
        };
        break;
    };
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
  glTexParameterf(target, pname, gl_fptof(param));
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_TRANSLATE = gl_to_ndsv(x);
  MATRIX_TRANSLATE = gl_to_ndsv(y);
  MATRIX_TRANSLATE = gl_to_ndsv(z);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  CASoftGLESFixed::glViewport(x, y, width, height);

  GFX_VIEWPORT = x + (y << 8) + ((width-1) << 16) + ((height-1) << 24);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glBegin(GLenum mode)
{
  rasterMode_ = mode;

  switch(mode)
  {
    case GL_TRIANGLES:
      GFX_BEGIN = NDS_TRIANGLES;
      break;
    case GL_TRIANGLE_STRIP:
      GFX_BEGIN = NDS_TRIANGLE_STRIP;
      break;
    // Simulate triangle fan using triangles
    case GL_TRIANGLE_FAN:
      GFX_BEGIN = NDS_TRIANGLES;
      break;
  };
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glEnd()
{
  GFX_END = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CNDSGLESContext::vertexShaderTransform(SVertexFx & v)
{
  // Color or normal
  if(lightingEnabled_ == true)
    GFX_NORMAL = fp_to_ndsNormal(v.n.x,  v.n.y,  v.n.z);
  else
    GFX_COLOR  = fp_to_ndsRGB555(v.cl.r, v.cl.g, v.cl.b);

  // Textures
  if(texturesEnabled_ == true)
    GFX_TEX_COORD = fp_to_ndsTexCoord(v.t[0], v.t[1]);

  // Vertex
  GFX_VERTEX16 = ((gl_to_ndsv(v.vo[1].value) << 16) & 0xffff0000) | (gl_to_ndsv(v.vo[0].value) & 0xffff);
  GFX_VERTEX16 = gl_to_ndsv(v.vo[2].value) & 0xffff;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::zbuffer(bool enable)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CNDSGLESContext::updateLights()
{
  iNDSPolyFormat_ &= ~(NDS_LIGHT0 | NDS_LIGHT1 | NDS_LIGHT2 | NDS_LIGHT3);
  if(lightingEnabled_ == true)
  {
    // Set color to white
    GFX_COLOR = 0x7fff;

    for(int iLight(0); iLight < 4; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        iNDSPolyFormat_ |= 1 << iLight;

        GFX_LIGHT_VECTOR = (iLight << 30) | fp_to_ndsNormal(lights_[iLight].direction.x, lights_[iLight].direction.y, lights_[iLight].direction.z);
        GFX_LIGHT_COLOR  = (iLight << 30) | fp_to_ndsRGB555(lights_[iLight].ambient.r,   lights_[iLight].ambient.g,   lights_[iLight].ambient.b);
      }
    }
  }
  GFX_POLY_FORMAT = iNDSPolyFormat_;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::updateFog()
{
//  GFX_FOG_OFFSET = fogStart_ --> 0..0x7fff;

//  fogDensity_
//  fogEnd_
}
