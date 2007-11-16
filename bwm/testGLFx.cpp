#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "kernel/videoManager.h"
#include "../gl/fixedPoint.h"


extern void initPyramidFx();
extern void drawPyramidFx();
extern void initCubeFx();
extern void drawCubeFx();
const GLfixed lightAmbient[]  = {gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(1.0f)};
const GLfixed lightDiffuse[]  = {gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f)};
const GLfixed lightPosition[] = {gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(2.0f), gl_fpfromf(1.0f)};
const GLfixed fogColor[]      = {gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(1.0f)};


// -----------------------------------------------------------------------------
void
testGLFx(CSurface * surface)
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

  // Viewport & Perspective
  glViewport(0, 0, surface->width(), surface->height());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)surface->width() / (float)surface->height(), 0.1f, 100.0f);

  // Move up a little
  glTranslatex(gl_fpfromi(0), gl_fpfromi(-2), gl_fpfromi(0));
  // Look down a little
  glRotatex(gl_fpfromi(23), gl_fpfromi(1), gl_fpfromi(0), gl_fpfromi(0));

  initPyramidFx();
  initCubeFx();
  glMatrixMode(GL_MODELVIEW);
  // Show Pyramid for 1 full rotation around y axis
  for(GLfixed yrot = gl_fpfromi(0); yrot < gl_fpfromi(360); yrot += gl_fpfromi(2))
  {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
/*
    glLoadIdentity();
    glTranslatex(gl_fpfromi(-2), gl_fpfromi(0), gl_fpfromi(-8));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    drawPyramidFx();

    glLoadIdentity();
    glTranslatex(gl_fpfromi(2), gl_fpfromi(0), gl_fpfromi(-8));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    drawPyramidFx();
*/
    glLoadIdentity();
    glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-6));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    drawCubeFx();

    glFlush();

    // Display progress bar
    //surface->setFillColor(255, 255, 255);
    //surface->fillRect(1, surface->height() - 12, surface->width() - 2, 10);
    //surface->setFillColor(0, 0, 0);
    //surface->fillRect(3, surface->height() - 10, ((surface->width() - 6) * gl_fptoi(yrot)) / 360, 6);

    surface->swap(true);
    //surface->swap(false);
  }
}
