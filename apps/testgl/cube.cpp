#include "GL/gl.h"
#include "GLES/gl.h"
#include "GL/glu.h"
#include "vhl/fixedPoint.h"
#include "vhl/vector.h"
#include "glconfig.h"


extern const unsigned short crate_Width;
extern const unsigned short crate_Height;
extern const unsigned short crate_Bitmap[];
GLuint textures[1];


// -----------------------------------------------------------------------------
const GLfloat cubeVertF[] =
{
  // Cube (6 triangle strips)
  // Top
  -1.0f, 1.0f, 1.0f,
   1.0f, 1.0f, 1.0f,
  -1.0f, 1.0f,-1.0f,
   1.0f, 1.0f,-1.0f,
  // Bottom
  -1.0f,-1.0f,-1.0f,
   1.0f,-1.0f,-1.0f,
  -1.0f,-1.0f, 1.0f,
   1.0f,-1.0f, 1.0f,
  // Front
  -1.0f,-1.0f, 1.0f,
   1.0f,-1.0f, 1.0f,
  -1.0f, 1.0f, 1.0f,
   1.0f, 1.0f, 1.0f,
  // Back
  -1.0f, 1.0f,-1.0f,
   1.0f, 1.0f,-1.0f,
  -1.0f,-1.0f,-1.0f,
   1.0f,-1.0f,-1.0f,
  // Left
  -1.0f, 1.0f, 1.0f,
  -1.0f, 1.0f,-1.0f,
  -1.0f,-1.0f, 1.0f,
  -1.0f,-1.0f,-1.0f,
  // Right
   1.0f, 1.0f,-1.0f,
   1.0f, 1.0f, 1.0f,
   1.0f,-1.0f,-1.0f,
   1.0f,-1.0f, 1.0f,
};

// -----------------------------------------------------------------------------
const GLfloat cubeTexF[] =
{
  // Cube
  // Top
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Bottom
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Front
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Back
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Left
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Right
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
};

// -----------------------------------------------------------------------------
const GLfixed cubeVertFx[] =
{
  // Cube (6 triangle strips)
  // Top
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  // Bottom
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  // Front
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  // Back
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  // Left
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  // Right
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
};

// -----------------------------------------------------------------------------
const GLfixed cubeTexFx[] =
{
  // Cube
  // Top
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Bottom
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Front
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Back
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Left
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Right
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
};

#ifdef ENABLE_LIGHTING
GLfloat cubeNormalF[24*3];
#endif
// -----------------------------------------------------------------------------
void
initCubeF()
{
#ifdef ENABLE_LIGHTING
  // Precalculated normals
  TVector3<GLfloat> V[3];
  TVector3<GLfloat> normal;
  for(int i(0); i < 6; i++)
  {
    // Load vetices V0, V1 and V2
    for(int v(0); v < 3; v++)
    {
      V[v].x = cubeVertF[i*4*3+(v*3)+0];
      V[v].y = cubeVertF[i*4*3+(v*3)+1];
      V[v].z = cubeVertF[i*4*3+(v*3)+2];
    }

    // Calculate normal
    normal = (V[0] - V[1]).getCrossProduct(V[2] - V[1]);
    normal.normalize();

    // Store normal for V0, V1, V2 and V3
    for(int v(0); v < 4; v++)
    {
      cubeNormalF[i*4*3+(v*3)+0] = normal.x;
      cubeNormalF[i*4*3+(v*3)+1] = normal.y;
      cubeNormalF[i*4*3+(v*3)+2] = normal.z;
    }
  }
#endif

  glGenTextures(1, &textures[0]);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, crate_Width, crate_Height, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, crate_Bitmap);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, crate_Width, crate_Height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, crate_Bitmap);
}

// -----------------------------------------------------------------------------
void
drawCubeF()
{
  glVertexPointer  (3, GL_FLOAT, 0, cubeVertF);
  glTexCoordPointer(2, GL_FLOAT, 0, cubeTexF);
#ifdef ENABLE_LIGHTING
  glNormalPointer  (   GL_FLOAT, 0, cubeNormalF);
#endif

  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#ifdef ENABLE_LIGHTING
  glEnableClientState(GL_NORMAL_ARRAY);
#else
  glDisableClientState(GL_NORMAL_ARRAY);
#endif

  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glDrawArrays(GL_TRIANGLE_STRIP,  0, 4); // Top
  glDrawArrays(GL_TRIANGLE_STRIP,  4, 4); // Bottom
  glDrawArrays(GL_TRIANGLE_STRIP,  8, 4); // Front
  glDrawArrays(GL_TRIANGLE_STRIP, 12, 4); // Back
  glDrawArrays(GL_TRIANGLE_STRIP, 16, 4); // Left
  glDrawArrays(GL_TRIANGLE_STRIP, 20, 4); // Right
}

// -----------------------------------------------------------------------------
#define TEX_WH 64
#define TEX_W  TEX_WH
#define TEX_H  TEX_WH
uint16_t * pTex = 0;
void
initCubeFx()
{
  // Scale down texture
  pTex = new uint16_t[TEX_W * TEX_H];
  gluScaleImage(GL_RGBA, crate_Width, crate_Height, GL_UNSIGNED_SHORT_1_5_5_5_REV, crate_Bitmap, TEX_W, TEX_H, GL_UNSIGNED_SHORT_1_5_5_5_REV, pTex);

  glGenTextures(1, &textures[0]);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, crate_Width   , crate_Height   , 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, crate_Bitmap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_W, TEX_H, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, pTex);
}

// -----------------------------------------------------------------------------
void
drawCubeFx()
{
  glVertexPointer(3, GL_FIXED, 0, cubeVertFx);
  glTexCoordPointer(2, GL_FIXED, 0, cubeTexFx);

  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glDrawArrays(GL_TRIANGLE_STRIP,  0, 4); // Top
  glDrawArrays(GL_TRIANGLE_STRIP,  4, 4); // Bottom
  glDrawArrays(GL_TRIANGLE_STRIP,  8, 4); // Front
  glDrawArrays(GL_TRIANGLE_STRIP, 12, 4); // Back
  glDrawArrays(GL_TRIANGLE_STRIP, 16, 4); // Left
  glDrawArrays(GL_TRIANGLE_STRIP, 20, 4); // Right
}
