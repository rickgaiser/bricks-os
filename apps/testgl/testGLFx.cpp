#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "glconfig.h"
#include "vhl/fixedPoint.h"
#include "../gl/context.h"


extern void glMakeCurrent(I3DRenderer * ctx);
extern void initPyramidFx();
extern void drawPyramidFx();
#ifdef ENABLE_TEXTURES
extern void initCubeFx();
extern void drawCubeFx();
#endif


#ifdef ENABLE_LIGHTING
// Material
const GLfixed matAmbient []   = {gl_fpfromf( 0.2f), gl_fpfromf( 0.2f), gl_fpfromf( 0.2f), gl_fpfromf( 1.0f)};
const GLfixed matDiffuse []   = {gl_fpfromf( 0.6f), gl_fpfromf( 0.6f), gl_fpfromf( 0.6f), gl_fpfromf( 1.0f)};
const GLfixed matSpecular[]   = {gl_fpfromf( 0.2f), gl_fpfromf( 0.2f), gl_fpfromf( 0.2f), gl_fpfromf( 1.0f)};
const GLfixed matShininess    = gl_fpfromf( 8.0f);
// Light
const GLfixed lightAmbient[]  = {gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f)};
const GLfixed lightDiffuse[]  = {gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f)};
const GLfixed lightSpecular[] = {gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f)};
const GLfixed lightPosition[] = {gl_fpfromf(-5.0f), gl_fpfromf(10.0f), gl_fpfromf(10.0f), gl_fpfromf( 1.0f)};
#endif
// Fog/Clear color
const GLfixed fogColor[]      = {gl_fpfromf( 0.4f), gl_fpfromf( 0.4f), gl_fpfromf( 0.4f), gl_fpfromf( 1.0f)};


// -----------------------------------------------------------------------------
int
renderPyramidFx(CAVideoDevice * device, I3DRenderer * renderer, CSurface * surface_a, CSurface * surface_b)
{
  int iFrameCount(0);
  int iStartFrame;
  int iCurrentFrame;

#ifdef ENABLE_DOUBLE_BUFFERED
  bool bDisplayB(true);

  // Set buffers
  renderer->setSurface(bDisplayB ? surface_b : surface_a);
  device->displaySurface(bDisplayB ? surface_a : surface_b);
  bDisplayB = !bDisplayB;
#endif

  iStartFrame = device->getFrameNr();

  // Show 1 full rotation around y axis
  for(GLfixed yrot = gl_fpfromi(0); yrot < gl_fpfromi(360);)
  {
#ifdef ENABLE_DEPTH_TEST
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
    glClear(GL_COLOR_BUFFER_BIT);
#endif

    glLoadIdentity();
    glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-6));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    drawPyramidFx();

    // Flush everything to surface
    glFlush();

    // Swap display and render buffers
#ifdef ENABLE_DOUBLE_BUFFERED
    renderer->setSurface(bDisplayB ? surface_b : surface_a);
    device->displaySurface(bDisplayB ? surface_a : surface_b);
    bDisplayB = !bDisplayB;
#endif
#ifdef ENABLE_VSYNC
    device->waitVSync();
#endif

    iFrameCount++;
    iCurrentFrame = device->getFrameNr();
    yrot = gl_fpfromi(1) * (iCurrentFrame - iStartFrame);
  }

  return iFrameCount;
}

#ifdef ENABLE_TEXTURES
// -----------------------------------------------------------------------------
int
renderCubeFx(CAVideoDevice * device, I3DRenderer * renderer, CSurface * surface_a, CSurface * surface_b)
{
  int iFrameCount(0);
  int iStartFrame;
  int iCurrentFrame;

#ifdef ENABLE_DOUBLE_BUFFERED
  bool bDisplayB(true);

  // Set buffers
  renderer->setSurface(bDisplayB ? surface_b : surface_a);
  device->displaySurface(bDisplayB ? surface_a : surface_b);
  bDisplayB = !bDisplayB;
#endif

  iStartFrame = device->getFrameNr();

  // Show 1 full rotation around y axis
  for(GLfixed yrot = gl_fpfromi(0); yrot < gl_fpfromi(360);)
  {
#ifdef ENABLE_DEPTH_TEST
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
    glClear(GL_COLOR_BUFFER_BIT);
#endif

    glLoadIdentity();
    glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-6));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    drawCubeFx();

    // Flush everything to surface
    glFlush();

    // Swap display and render buffers
#ifdef ENABLE_DOUBLE_BUFFERED
    renderer->setSurface(bDisplayB ? surface_b : surface_a);
    device->displaySurface(bDisplayB ? surface_a : surface_b);
    bDisplayB = !bDisplayB;
#endif
#ifdef ENABLE_VSYNC
    device->waitVSync();
#endif

    iFrameCount++;
    iCurrentFrame = device->getFrameNr();
    yrot = gl_fpfromi(1) * (iCurrentFrame - iStartFrame);
  }

  return iFrameCount;
}
#endif

// -----------------------------------------------------------------------------
void
testGLFx(CAVideoDevice * device, CSurface * surface_a, CSurface * surface_b)
{
  I3DRenderer * renderer;
  device->get3DRenderer(&renderer);
  glMakeCurrent(renderer);

  // Set buffers
  renderer->setSurface(surface_a);
  device->displaySurface(surface_a);

  // Background color
  glClearColorx(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

#ifdef ENABLE_DEPTH_TEST
  // Depth test
  glClearDepthx(gl_fpfromi(1));
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
#endif

  // Backface culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

#ifdef ENABLE_LIGHTING
  // Material
  glMaterialxv(GL_FRONT, GL_AMBIENT,   matAmbient);
  glMaterialxv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
  glMaterialxv(GL_FRONT, GL_SPECULAR,  matSpecular);
  glMaterialx (GL_FRONT, GL_SHININESS, matShininess);

  // Lighting
  glLightxv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  glLightxv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glLightxv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
  glLightxv(GL_LIGHT0, GL_POSITION, lightPosition);
  glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHTING);
#endif

#ifdef ENABLE_FOG
  // Fog
  glFogxv(GL_FOG_COLOR, fogColor);
  glFogx(GL_FOG_DENSITY, gl_fpfromf(0.35f));
  glFogx(GL_FOG_START, gl_fpfromi(1));
  glFogx(GL_FOG_END, gl_fpfromi(10));
  glEnable(GL_FOG);
#endif

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
#ifdef ENABLE_TEXTURES
  initCubeFx();
#endif

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Notify user of error
  if(glGetError() != GL_NO_ERROR)
  {
    glClearColorx(gl_fpfromi(1), gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(0));
  }

  while(true)
  {
    // Flat
//    glShadeModel(GL_FLAT);
//    renderPyramidFx(device, renderer, surface_a, surface_b);
    // Smooth
    glShadeModel(GL_SMOOTH);
    renderPyramidFx(device, renderer, surface_a, surface_b);

#ifdef ENABLE_LIGHTING
    glEnable(GL_LIGHTING);

    // Flat
//    glShadeModel(GL_FLAT);
//    renderPyramidFx(device, renderer, surface_a, surface_b);
    // Smooth
    glShadeModel(GL_SMOOTH);
    renderPyramidFx(device, renderer, surface_a, surface_b);

    glDisable(GL_LIGHTING);
#endif

#ifdef ENABLE_TEXTURES
    glEnable(GL_TEXTURE_2D);

    // Flat
    glShadeModel(GL_FLAT);
    renderCubeFx(device, renderer, surface_a, surface_b);

    glDisable(GL_TEXTURE_2D);
#endif
  }

  glMakeCurrent(NULL);
  delete renderer;
}
