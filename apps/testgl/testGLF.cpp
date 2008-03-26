#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "kernel/videoManager.h"
#include "../gl/context.h"


extern void initPyramidF();
extern void drawPyramidF();
extern void initCubeF();
extern void drawCubeF();
const GLfloat lightAmbient[]  = {0.5f, 0.5f, 0.5f, 1.0f};
const GLfloat lightDiffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat lightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};
const GLfloat fogColor[]      = {0.5f, 0.5f, 0.5f, 1.0f};


extern IGLESRenderer * context;
// -----------------------------------------------------------------------------
void
testGL(CAVideoDevice * device, CSurface * surface_a, CSurface * surface_b)
{
  // Background color
  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

  // FIXME: We can only call context after the first gl call, becouse the first
  //        call creates the context.
  bool bDisplayB(true);
  context->setSurface(surface_a);
  device->displaySurface(surface_b);
  // Automatically wait for VSync
  device->setVSync(true);

  // Depth test
  glClearDepthf(1.0f);
  glDepthFunc(GL_LEQUAL);
  //glEnable(GL_DEPTH_TEST);

  // Backface culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  // Shade model
  glShadeModel(/*GL_FLAT*/GL_SMOOTH);

  // Lighting
  //glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  //glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHTING);

  // Fog
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.35f);
  glFogf(GL_FOG_START, 1.0f);
  glFogf(GL_FOG_END, 10.0f);
  //glEnable(GL_FOG);

  // Viewport & Perspective
  glViewport(0, 0, surface_a->width(), surface_a->height());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)surface_a->width() / (float)surface_a->height(), 0.1f, 100.0f);

  // Move up a little
  glTranslatef(0.0f, -2.0f, 0.0f);
  // Look down a little
  glRotatef(23.0f, 1.0f, 0.0f, 0.0f);

  initPyramidF();
  initCubeF();
  glMatrixMode(GL_MODELVIEW);
  // Show 1 full rotation around y axis
  for(GLfloat yrot(0.0f); yrot < 360.0f; yrot += 2.0f)
  {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
/*
    glLoadIdentity();
    glTranslatef(-2.0f, 0.0f, -8.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    drawPyramidF();

    glLoadIdentity();
    glTranslatef(2.0f, 0.0f, -8.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    drawPyramidF();
*/
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -6.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    drawCubeF();

    // Flush everything to surface
    glFlush();

    // Swap display and render buffers
    if(bDisplayB == true)
    {
      context->setSurface(surface_b);
      device->displaySurface(surface_a);
    }
    else
    {
      context->setSurface(surface_a);
      device->displaySurface(surface_b);
    }
    bDisplayB = !bDisplayB;
  }
}
