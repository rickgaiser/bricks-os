#include "texturesNDS.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
#include "string.h"


// mode bits
#define NDS_RGB32_A3   1 // 32 color palette, 3 bits of alpha
#define NDS_RGB4       2 // 4 color palette
#define NDS_RGB16      3 // 16 color palette
#define NDS_RGB256     4 // 256 color palette
#define NDS_COMPRESSED 5 // compressed texture
#define NDS_RGB8_A5    6 // 8 color palette, 5 bits of alpha
#define NDS_RGBA       7 // 15 bit direct color, 1 bit of alpha
#define NDS_RGB        8 // 15 bit direct color, alpha bit autoset to 1

#define TEXGEN_OFF      (0<<30)  //unmodified texcoord
#define TEXGEN_TEXCOORD (1<<30)  //texcoord * texture-matrix
#define TEXGEN_NORMAL   (2<<30)  //normal * texture-matrix
#define TEXGEN_POSITION (3<<30)  //vertex * texture-matrix


//-----------------------------------------------------------------------------
CAGLESTexturesNDS::CAGLESTexturesNDS()
 : pCurrentTex_(0)
{
  for(GLuint idx(0); idx < MAX_TEXTURE_COUNT; idx++)
    textures_[idx].used = false;
}

//-----------------------------------------------------------------------------
CAGLESTexturesNDS::~CAGLESTexturesNDS()
{
}

//-----------------------------------------------------------------------------
void
CAGLESTexturesNDS::glBindTexture(GLenum target, GLuint texture)
{
  if(target == GL_TEXTURE_2D)
  {
    pCurrentTex_ = &textures_[texture];
    GFX_TEX_FORMAT = pCurrentTex_->format;
  }
}

//-----------------------------------------------------------------------------
void
CAGLESTexturesNDS::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  for(GLsizei i(0); i < n; i++)
    if(textures[i] < MAX_TEXTURE_COUNT)
      textures_[textures[i]].used = false;
}

//-----------------------------------------------------------------------------
void
CAGLESTexturesNDS::glGenTextures(GLsizei n, GLuint *textures)
{
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

//-----------------------------------------------------------------------------
void
CAGLESTexturesNDS::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
  if(target == GL_TEXTURE_2D)
  {
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
          return; // ERROR, invalid width
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
          return; // ERROR, invalid height
      };

      pCurrentTex_->width  = width;
      pCurrentTex_->height = height;
      pCurrentTex_->format = TEXGEN_TEXCOORD | (idxWidth << 20) | (idxHeight << 23) | (((uint32_t)0x6840000 >> 3) & 0xFFFF) | (NDS_RGBA << 26);
      pCurrentTex_->data   = (void *)0x6840000;

      // Set current texture format
      GFX_TEX_FORMAT = pCurrentTex_->format;

      // Unlock texture memory
      REG_VRAM_C_CR = VRAM_ENABLE | VRAM_TYPE_LCD;

      // Copy to texture memory
      //memcpy((void *)0x6840000, pixels, width * height * 2);
      for(int i(0); i < (width * height); i++)
        ((uint16_t *)0x6840000)[i] = ((uint16_t *)pixels)[i] | 0x8000;

      // Restore texture memory
      REG_VRAM_C_CR = VRAM_ENABLE | VRAM_TYPE_TEXTURE;
    }
  }
}
