#include "glDemo1.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "../gl/context.h"


const GLfixed lightAmbient[]  = {gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(1.0f)};
const GLfixed lightDiffuse[]  = {gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f)};
const GLfixed lightPosition[] = {gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(2.0f), gl_fpfromf(1.0f)};
const GLfixed fogColor[]      = {gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(1.0f)};

const GLfixed pyramidVertFx[] =
{
  // Square (strip)
  gl_fpfromf(-1.0f), gl_fpfromf(-1.5f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.5f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.5f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.5f), gl_fpfromf(-1.0f),

  // Pyramid (fan)
  gl_fpfromf( 0.0f), gl_fpfromf( 1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f)
};

const GLfixed pyramidColFx[] =
{
  // Square
  gl_fpfromf(0.4f), gl_fpfromf(0.4f), gl_fpfromf(0.4f), gl_fpfromf(1.0f),
  gl_fpfromf(0.4f), gl_fpfromf(0.4f), gl_fpfromf(0.4f), gl_fpfromf(1.0f),
  gl_fpfromf(0.4f), gl_fpfromf(0.4f), gl_fpfromf(0.4f), gl_fpfromf(1.0f),
  gl_fpfromf(0.4f), gl_fpfromf(0.4f), gl_fpfromf(0.4f), gl_fpfromf(1.0f),

  // Pyramid
  gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f)
};


//---------------------------------------------------------------------------
CGLDemo1::CGLDemo1(CWidget * parent)
 : CGLWidget(parent)
 , yrot_(gl_fpfromi(0))
{
}

//---------------------------------------------------------------------------
CGLDemo1::~CGLDemo1()
{
}

//---------------------------------------------------------------------------
void
CGLDemo1::initializeGL()
{
  // Background color
  glClearColorx(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

  // Depth test
  glClearDepthx(gl_fpfromi(1));
  glDepthFunc(GL_LEQUAL);
  //glEnable(GL_DEPTH_TEST);

  // Backface culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  // Shade model
  glShadeModel(/*GL_FLAT*/GL_SMOOTH);

  // Lighting
  //glLightxv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  //glLightxv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  //glLightxv(GL_LIGHT0, GL_POSITION, lightPosition);
  //glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHTING);

  // Fog
  glFogxv(GL_FOG_COLOR, fogColor);
  glFogx(GL_FOG_DENSITY, gl_fpfromf(0.35f));
  glFogx(GL_FOG_START, gl_fpfromi(1));
  glFogx(GL_FOG_END, gl_fpfromi(10));
  //glEnable(GL_FOG);

  glMatrixMode(GL_MODELVIEW);

  glVertexPointer(3, GL_FIXED, 0, pyramidVertFx);
  glColorPointer(4, GL_FIXED, 0, pyramidColFx);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

//---------------------------------------------------------------------------
void
CGLDemo1::resizeGL(int w, int h)
{
  // Viewport & Perspective
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);

  // Move up a little
  glTranslatex(gl_fpfromi(0), gl_fpfromi(-2), gl_fpfromi(0));
  // Look down a little
  glRotatex(gl_fpfromi(23), gl_fpfromi(1), gl_fpfromi(0), gl_fpfromi(0));

  glMatrixMode(GL_MODELVIEW);
}

//---------------------------------------------------------------------------
void
CGLDemo1::drawGL()
{
  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();
  glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-6));
  glRotatex(yrot_, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));

  //glDrawArrays(GL_TRIANGLE_STRIP,  0, 4); // 'Shade'
  glDrawArrays(GL_TRIANGLE_FAN,    4, 6); // Pyramid

  // Flush everything to surface
  glFlush();

  yrot_ += gl_fpfromi(3);
}
