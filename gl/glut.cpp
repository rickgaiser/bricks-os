#include "GL/glut.h"
#include "GLES/gl.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "asm/arch/registers.h"
#include "stdlib.h"

#ifdef PSP
#include <pspctrl.h>
#endif

// NDS Renders to screen directly
#if defined(NDS)
#else
  #define BUFFER_A
#endif

// NGC Uses copy from RGB to YPbPr framebuffer
// NDS Renders to screen directly
#if defined(NGC) || defined(NDS)
#else
  #define BUFFER_B // Double buffering
#endif


extern void glMakeCurrent(I3DRenderer * ctx);


// Selected device
CAVideoDevice * pGlutDevice = NULL;

// Selected mode
const SVideoMode * pGlutMode = NULL;

// 2 surfaces for double buffering
#ifdef BUFFER_A
CSurface * pGlutSurfaceA = NULL;
#endif
#ifdef BUFFER_B
CSurface * pGlutSurfaceB = NULL;
bool bGlutActiveDisplay = false;
#endif

// 3D rendering device
I3DRenderer * pGlutRenderer;


typedef void (* glutDisplayFunction)(void);
typedef void (* glutReshapeFunction)(int, int);
typedef void (* glutKeyboardFunction)(unsigned char, int, int);
typedef void (* glutSpecialFunction)(int, int, int);
glutDisplayFunction  fpGlutDisplay  = NULL;
glutReshapeFunction  fpGlutReshape  = NULL;
glutKeyboardFunction fpGlutKeyboard = NULL;
glutSpecialFunction  fpGlutSpecial  = NULL;


// -----------------------------------------------------------------------------
void
glutInit(int * pargc, char ** argv)
{
  CAVideoDevice ** devices;
  int iDeviceCount;

  videoManager.listDevices(&devices, &iDeviceCount);
  if(iDeviceCount > 0)
  {
    pGlutDevice = devices[0];

    pGlutDevice->getDefaultMode(&pGlutMode);
    if(pGlutMode != NULL)
    {
      // Set default video mode
      pGlutDevice->setMode(pGlutMode);

      // Allocate two surfaces for double buffering
#ifdef BUFFER_A
      pGlutDevice->getSurface(&pGlutSurfaceA, pGlutMode->width, pGlutMode->height);
#endif
#ifdef BUFFER_B
      pGlutDevice->getSurface(&pGlutSurfaceB, pGlutMode->width, pGlutMode->height);
#endif

      // Allocate renderer and activate it
      pGlutDevice->get3DRenderer(&pGlutRenderer);
      glMakeCurrent(pGlutRenderer);

#ifdef BUFFER_A
      pGlutRenderer->setSurface(pGlutSurfaceA);
      pGlutDevice->displaySurface(pGlutSurfaceA);
#endif
    }
    else
      printk("ERROR: Device has no modes!\n");
  }
  else
    printk("ERROR: No video devices!\n");
}

// -----------------------------------------------------------------------------
void
glutDisplayFunc(void (* callback)(void))
{
  fpGlutDisplay = callback;
}

// -----------------------------------------------------------------------------
void
glutReshapeFunc(void (* callback)(int, int))
{
  fpGlutReshape = callback;
}

// -----------------------------------------------------------------------------
void
glutKeyboardFunc(void (* callback)(unsigned char, int, int))
{
  fpGlutKeyboard = callback;
}

// -----------------------------------------------------------------------------
void
glutSpecialFunc(void (* callback)(int, int, int))
{
  fpGlutSpecial = callback;
}

// -----------------------------------------------------------------------------
void
glutProcessKeys()
{
  bool bUp(false), bDown(false), bLeft(false), bRight(true);

#if defined(GBA) || defined(NDS)
  uint16_t data;
  data   = ~REG_KEYS;
  bUp    = (data & KEY_UP);
  bDown  = (data & KEY_DOWN);
  bLeft  = (data & KEY_LEFT);
  bRight = (data & KEY_RIGHT);
#endif
#ifdef NGC
  uint32_t datah, datal;
  datah  = REG_SI_CHANNEL0_INBUFH;
  datal  = REG_SI_CHANNEL0_INBUFL;
  bUp    = (datah & KEY_UP);
  bDown  = (datah & KEY_DOWN);
  bLeft  = (datah & KEY_LEFT);
  bRight = (datah & KEY_RIGHT);
#endif
#ifdef PSP
  SceCtrlData pad;
  sceCtrlReadBufferPositive(&pad, 1);
  bUp    = (pad.Buttons & PSP_CTRL_UP);
  bDown  = (pad.Buttons & PSP_CTRL_DOWN);
  bLeft  = (pad.Buttons & PSP_CTRL_LEFT);
  bRight = (pad.Buttons & PSP_CTRL_RIGHT);
#endif

  if(bUp)
    fpGlutSpecial(GLUT_KEY_UP, 0, 0);
  if(bDown)
    fpGlutSpecial(GLUT_KEY_DOWN, 0, 0);
  if(bLeft)
    fpGlutSpecial(GLUT_KEY_LEFT, 0, 0);
  if(bRight)
    fpGlutSpecial(GLUT_KEY_RIGHT, 0, 0);
}

// -----------------------------------------------------------------------------
void
glutMainLoop(void)
{
  if((fpGlutDisplay == NULL) || (fpGlutReshape == NULL))
    return;

  fpGlutReshape(pGlutMode->width, pGlutMode->height);

  while(true)
  {
    if(fpGlutSpecial != NULL)
      glutProcessKeys();

    fpGlutDisplay();
  }
}

// -----------------------------------------------------------------------------
void
glutSwapBuffers(void)
{
  pGlutDevice->waitVSync();
#ifdef BUFFER_B
  pGlutRenderer->setSurface(bGlutActiveDisplay ? pGlutSurfaceA : pGlutSurfaceB);
  pGlutDevice->displaySurface(bGlutActiveDisplay ? pGlutSurfaceB : pGlutSurfaceA);
  bGlutActiveDisplay = !bGlutActiveDisplay;
#endif
}
