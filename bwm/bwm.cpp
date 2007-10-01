#include "bwm/bwm.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#ifdef CONFIG_GL
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#endif // CONFIG_GL


// Default desktop color
color_t clDesktop(BxRGB(58, 110, 165));
// Window title-bar colors
color_t clActiveWindowL(BxRGB(10, 36, 106));
color_t clActiveWindowR(BxRGB(166, 202, 240));
color_t clActiveWindow(clActiveWindowL);
color_t clInActiveWindowL(BxRGB(128, 128, 128));
color_t clInActiveWindowR(BxRGB(192, 192, 192));
color_t clInActiveWindow(clInActiveWindowL);
// Panel colors
color_t clPanelLight(BxRGB(255, 255, 255));
color_t clPanelShade1(BxRGB(64, 64, 64));
color_t clPanelShade2(BxRGB(128, 128, 128));
color_t clPanelFill(BxRGB(212, 208, 200));


#ifdef CONFIG_GL
const GLfloat lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
const GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat fogColor[]     = {0.5f, 0.5f, 0.5f, 0.5f};

const GLfloat triangle[] =
{
  // Front
   0.0f,  1.0f,  0.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
  // Right
   0.0f,  1.0f,  0.0f,
   1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,
  // Back
   0.0f,  1.0f,  0.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
  // Left
   0.0f,  1.0f,  0.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f
};
GLint triangle_count(4);

const GLfloat colors[] =
{
  // Front
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  // Right
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  // Back
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  // Left
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f
};

const GLfloat normals[] =
{
  // Front
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f,
  // Right
  1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  // Back
  0.0f, 0.0f,-1.0f,
  0.0f, 0.0f,-1.0f,
  0.0f, 0.0f,-1.0f,
  // Left
  -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f
};
#endif // CONFIG_GL


#define RAND_A 9301
#define RAND_C 49297
#define RAND_M 233280
static uint32_t seed(0x12345678);

// -----------------------------------------------------------------------------
float
crap_rand()
{
  seed = (seed*RAND_A+RAND_C) % RAND_M;
  return((float)seed / (float)RAND_M);
}

// -----------------------------------------------------------------------------
void
testFill(CSurface * surface)
{
/*
  for(int i(0); i < 0x001f; i++)
  {
    // Fill entire screen with one color
    surface->setFillColor(i);
    surface->fill();
    // Display progress bar
    surface->setFillColor(clWhite);
    surface->fillRect(2, surface->height() - 12, surface->width() - 4, 10);
    surface->setFillColor(clBlack);
    surface->fillRect(3, surface->height() - 10, ((surface->width() - 6) * i) / 0x001f, 6);
    // Swap back and front buffer, placing the rendered image on screen
    surface->swap();
  }
*/

  // Pixel test
  surface->setFillColor(clBlack);
  surface->fill();
  for(int i(0); i < 0x00FF; i++)
  {
    surface->setColor(BxRGB((uint8_t)(crap_rand()*255), (uint8_t)(crap_rand()*255), (uint8_t)(crap_rand()*255)));
    surface->setPixel(
      (int)(crap_rand() * surface->width()),
      (int)(crap_rand() * surface->height()));

    surface->swap(true);
  }

  // Line test
  surface->setFillColor(clBlack);
  surface->fill();
  for(int i(0); i < 0x00FF; i++)
  {
    surface->setColor(BxRGB((uint8_t)(crap_rand()*255), (uint8_t)(crap_rand()*255), (uint8_t)(crap_rand()*255)));
    surface->drawLine(
      (int)(crap_rand() * surface->width()),
      (int)(crap_rand() * surface->height()),
      (int)(crap_rand() * surface->width()),
      (int)(crap_rand() * surface->height()));

    surface->swap(true);
  }

  // Rect test
  surface->setFillColor(clBlack);
  surface->fill();
  for(int i(0); i < 0x00FF; i++)
  {
    int x1 = (int)(crap_rand() * surface->width());
    int y1 = (int)(crap_rand() * surface->height());
    int x2 = (int)(crap_rand() * surface->width());
    int y2 = (int)(crap_rand() * surface->height());
    int x  = x1 < x2 ? x1 : x2;
    int y  = y1 < y2 ? y1 : y2;
    int w  = x1 < x2 ? (x2-x1) : (x1-x2);
    int h  = y1 < y2 ? (y2-y1) : (y1-y2);

    surface->setColor(BxRGB((uint8_t)(crap_rand()*255), (uint8_t)(crap_rand()*255), (uint8_t)(crap_rand()*255)));
    surface->drawRect(x, y, w, h);

    surface->swap(true);
  }

  // Filled Rect test
  surface->setFillColor(clBlack);
  surface->fill();
  for(int i(0); i < 0x00FF; i++)
  {
    int x1 = (int)(crap_rand() * surface->width());
    int y1 = (int)(crap_rand() * surface->height());
    int x2 = (int)(crap_rand() * surface->width());
    int y2 = (int)(crap_rand() * surface->height());
    int x  = x1 < x2 ? x1 : x2;
    int y  = y1 < y2 ? y1 : y2;
    int w  = x1 < x2 ? (x2-x1) : (x1-x2);
    int h  = y1 < y2 ? (y2-y1) : (y1-y2);

    surface->setFillColor(BxRGB((uint8_t)(crap_rand()*255), (uint8_t)(crap_rand()*255), (uint8_t)(crap_rand()*255)));
    surface->fillRect(x, y, w, h);

    surface->swap(true);
  }
}

#ifdef CONFIG_GL
// -----------------------------------------------------------------------------
void
testGL(CSurface * surface)
{
  EGLDisplay   egldisplay;
  EGLConfig    eglconfig = 0;
  EGLSurface   eglsurface;
  EGLContext   eglcontext;

  // Initialize EGL
  egldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(egldisplay, 0, 0);
  eglBindAPI(EGL_OPENGL_ES_API);
  //eglChooseConfig(egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
  eglsurface = eglCreateWindowSurface(egldisplay, eglconfig, (EGLNativeDisplayType)surface, 0);
  eglcontext = eglCreateContext(egldisplay, eglconfig, 0, 0);
  eglMakeCurrent(egldisplay, eglsurface, eglsurface, eglcontext);

  // Initialize GL
  // Background color
//  glClearColor(0.23f, 0.43f, 0.65f, 0.0f);
  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
  // Depth test
  glClearDepthf(1.0f);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  // Backface culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  // Shade model
  glShadeModel(/*GL_FLAT*/GL_SMOOTH);
  // Viewport & Perspective
  glViewport(0, 0, surface->width, surface->height);
  gluPerspective(45.0f, (float)surface->width / (float)surface->height, 0.1f, 100.0f);
  // Lighting
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  // Fog
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.35f);
  glFogf(GL_FOG_START, 1.0f);
  glFogf(GL_FOG_END, 10.0f);
  glEnable(GL_FOG);
  // Pointers to data
  glVertexPointer(3, GL_FLOAT, 0, triangle);
  glColorPointer(4, GL_FLOAT, 0, colors);
  glNormalPointer(GL_FLOAT, 0, normals);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Move up a little
  //glTranslatef(0.0f, -2.0f, 0.0f);
  // Look down a little
  //glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Show Pyramid for 1 full rotation around y axis
  for(float fYRotTriangle(0.0f); fYRotTriangle < 360.0f; fYRotTriangle += 2.0f)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -4.0f);
    glRotatef(fYRotTriangle, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    glLoadIdentity();
    glTranslatef(-2.0f, 0.0f, -7.0f);
    glRotatef(fYRotTriangle, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    glLoadIdentity();
    glTranslatef(2.0f, 0.0f, -7.0f);
    glRotatef(fYRotTriangle, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    glFlush();

    // Display progress bar
    surface->fillRect(1, surface->height - 12, surface->width - 2, 10, clWhite);
    surface->fillRect(3, surface->height - 10, ((surface->width - 6) * static_cast<int>(fYRotTriangle)) / 360, 6, clBlack);

    surface->swap();
  }

  // Shutdown EGL
  eglMakeCurrent(egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglTerminate(egldisplay);
}
#endif // CONFIG_GL

// -----------------------------------------------------------------------------
int
bwm(int argc, char * argv[])
{
  CAVideoDevice ** devices;
  int iDeviceCount;
  videoManager.listDevices(&devices, &iDeviceCount);
  if(iDeviceCount > 0)
  {
    for(int iDev(0); iDev < iDeviceCount; iDev++)
    {
      const SVideoMode * modes;
      int iModeCount;
      devices[iDev]->listModes(&modes, &iModeCount);
      if(iModeCount > 0)
      {
        for(int iMode(0); iMode < iModeCount; iMode++)
        {
          if((modes[iMode].format == cfX1R5G5B5) || (modes[iMode].format == cfA1R5G5B5))
          {
            devices[iDev]->setMode(&modes[iMode]);
            CSurface * pVideoSurface;
            devices[iDev]->getSurface(&pVideoSurface, stSCREEN);

            while(true)
            {
#ifndef CONFIG_GL
              testFill(pVideoSurface);
#else
              testGL(pVideoSurface);
#endif // CONFIG_GL
            }
            delete pVideoSurface;
          }
        }
      }
      else
      {
        printk("ERROR: Device has no modes!\n");
      }
    }
  }
  else
  {
    printk("ERROR: No video devices!\n");
  }

  return 0;
}
