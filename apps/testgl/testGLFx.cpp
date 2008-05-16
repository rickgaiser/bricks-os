#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "kernel/videoManager.h"
#include "../gl/fixedPoint.h"
#include "../gl/context.h"


extern void initPyramidFx();
extern void drawPyramidFx();
extern void initCubeFx();
extern void drawCubeFx();
const GLfixed material[]      = {gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f)};
const GLfixed lightAmbient[]  = {gl_fpfromf( 0.5f), gl_fpfromf( 0.5f), gl_fpfromf( 0.5f), gl_fpfromf( 1.0f)};
const GLfixed lightDiffuse[]  = {gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f)};
const GLfixed lightPosition[] = {gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 2.0f), gl_fpfromf( 1.0f)};
const GLfixed fogColor[]      = {gl_fpfromf( 0.4f), gl_fpfromf( 0.4f), gl_fpfromf( 0.4f), gl_fpfromf( 1.0f)};


extern void glMakeCurrent(I3DRenderer * ctx);
// -----------------------------------------------------------------------------
void
testGL(CAVideoDevice * device, CSurface * surface_a, CSurface * surface_b)
{
  bool bDisplayB(true);
  I3DRenderer * p3DRenderer_;
  device->get3DRenderer(&p3DRenderer_);
  p3DRenderer_->setSurface(surface_a);
  device->displaySurface(surface_b);
  glMakeCurrent(p3DRenderer_);

  // Automatically wait for VSync
  device->setVSync(true);

  // Background color
  glClearColorx(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

  // Depth test
  glClearDepthx(gl_fpfromi(1));
  glDepthFunc(GL_LEQUAL);
  //glEnable(GL_DEPTH_TEST);

  // Backface culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  // Material
  glMaterialxv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material);

  // Lighting
  glLightxv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  glLightxv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glLightxv(GL_LIGHT0, GL_POSITION, lightPosition);
  glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHTING);

  // Fog
  glFogxv(GL_FOG_COLOR, fogColor);
  glFogx(GL_FOG_DENSITY, gl_fpfromf(0.35f));
  glFogx(GL_FOG_START, gl_fpfromi(1));
  glFogx(GL_FOG_END, gl_fpfromi(10));
  //glEnable(GL_FOG);

  // Viewport & Perspective
  glViewport(0, 0, surface_a->width(), surface_a->height());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)surface_a->width() / (float)surface_a->height(), 0.1f, 100.0f);

  // Move up a little
  glTranslatex(gl_fpfromi(0), gl_fpfromi(-2), gl_fpfromi(0));
  // Look down a little
  glRotatex(gl_fpfromi(23), gl_fpfromi(1), gl_fpfromi(0), gl_fpfromi(0));

  initPyramidFx();
  initCubeFx();
  glMatrixMode(GL_MODELVIEW);

  while(true)
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    // Show 1 full rotation around y axis
    for(GLfixed yrot = gl_fpfromi(0); yrot < gl_fpfromi(360); yrot += gl_fpfromi(2))
    {
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClear(GL_COLOR_BUFFER_BIT);

      glLoadIdentity();
      glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-6));
      glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
      drawPyramidFx();

      // Flush everything to surface
      glFlush();

      // Swap display and render buffers
      if(bDisplayB == true)
      {
        p3DRenderer_->setSurface(surface_b);
        device->displaySurface(surface_a);
      }
      else
      {
        p3DRenderer_->setSurface(surface_a);
        device->displaySurface(surface_b);
      }
      bDisplayB = !bDisplayB;
    }

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    // Show 1 full rotation around y axis
    for(GLfixed yrot = gl_fpfromi(0); yrot < gl_fpfromi(360); yrot += gl_fpfromi(2))
    {
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClear(GL_COLOR_BUFFER_BIT);

      glLoadIdentity();
      glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-6));
      glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
      drawPyramidFx();

      // Flush everything to surface
      glFlush();

      // Swap display and render buffers
      if(bDisplayB == true)
      {
        p3DRenderer_->setSurface(surface_b);
        device->displaySurface(surface_a);
      }
      else
      {
        p3DRenderer_->setSurface(surface_a);
        device->displaySurface(surface_b);
      }
      bDisplayB = !bDisplayB;
    }

    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    // Show 1 full rotation around y axis
    for(GLfixed yrot = gl_fpfromi(0); yrot < gl_fpfromi(360); yrot += gl_fpfromi(2))
    {
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClear(GL_COLOR_BUFFER_BIT);

      glLoadIdentity();
      glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-6));
      glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
      drawCubeFx();

      // Flush everything to surface
      glFlush();

      // Swap display and render buffers
      if(bDisplayB == true)
      {
        p3DRenderer_->setSurface(surface_b);
        device->displaySurface(surface_a);
      }
      else
      {
        p3DRenderer_->setSurface(surface_a);
        device->displaySurface(surface_b);
      }
      bDisplayB = !bDisplayB;
    }
  }

  delete p3DRenderer_;
}
