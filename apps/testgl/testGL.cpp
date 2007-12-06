#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "stdlib.h"
#include "EGL/egl.h"


#ifdef CONFIG_FPU
extern void testGLF(CSurface * surface);
#else
extern void testGLFx(CSurface * surface);
#endif // CONFIG_FPU


EGLDisplay   egldisplay;
EGLConfig    eglconfig = 0;
EGLSurface   eglsurface;
EGLContext   eglcontext;


// -----------------------------------------------------------------------------
void
eglInit(CSurface * surface)
{
  // Get default display
  egldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if(egldisplay == NULL)
  {
    printk("ERROR: eglInit failed\n");
    return;
  }

  // Initialize display
  if(eglInitialize(egldisplay, NULL, NULL) == EGL_FALSE)
  {
    printk("ERROR: eglInitialize failed\n");
    return;
  }

  // Bind to OpenGL-ES API
  //eglBindAPI(EGL_OPENGL_ES_API);

  // Choose configuration
  //eglChooseConfig(egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);

  // Create surface
  eglsurface = eglCreateWindowSurface(egldisplay, eglconfig, (EGLNativeDisplayType)surface, NULL);
  if(eglsurface == NULL)
  {
    printk("ERROR: eglCreateWindowSurface failed\n");
    return;
  }

  // Create EGL context
  eglcontext = eglCreateContext(egldisplay, eglconfig, NULL, NULL);
  if(eglcontext == NULL)
  {
    printk("ERROR: eglCreateContext failed\n");
    return;
  }

  // Set current
  if(eglMakeCurrent(egldisplay, eglsurface, eglsurface, eglcontext) == EGL_FALSE)
  {
    printk("ERROR: eglMakeCurrent failed\n");
    return;
  }
}

// -----------------------------------------------------------------------------
void
eglDestroy()
{
  // Shutdown EGL
  eglMakeCurrent(egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(egldisplay, eglsurface);
  eglDestroyContext(egldisplay, eglcontext);
  eglTerminate(egldisplay);
}

// -----------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
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
          devices[iDev]->setMode(&modes[iMode]);
          CSurface * pVideoSurface;

          // Test 3d with a double buffered surface
          devices[iDev]->getSurface(&pVideoSurface, stSCREEN);
          devices[iDev]->displaySurface(pVideoSurface);
          eglInit(pVideoSurface);
#ifdef CONFIG_FPU
          // Test floating point interface
          testGLF(pVideoSurface);
#else
          // Test fixed point interface
          testGLFx(pVideoSurface);
#endif // CONFIG_FPU
          eglDestroy();
          delete pVideoSurface;
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
