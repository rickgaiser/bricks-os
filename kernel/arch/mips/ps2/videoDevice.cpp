#include "videoDevice.h"
#include <dma.h>
#include <graph.h>


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2Surface::CPS2Surface()
 : CSurface()
{
}

//---------------------------------------------------------------------------
CPS2Surface::~CPS2Surface()
{
}

//---------------------------------------------------------------------------
void
CPS2Surface::swap(bool bForceCopy)
{
  // Wait for vsync
  graph_wait_vsync();

  // Write frame
  graph_vram_write(0,             640, 256, GRAPH_PSM_16, &((uint16_t *)p)[0],         640 * 256 * 2);
  graph_vram_write(640 * 256 * 2, 640, 256, GRAPH_PSM_16, &((uint16_t *)p)[256 * 640], 640 * 256 * 2);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2VideoDevice::CPS2VideoDevice()
 : CAVideoDevice()
{
}

//---------------------------------------------------------------------------
CPS2VideoDevice::~CPS2VideoDevice()
{
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  static const SVideoMode videoModes[] =
  {
      {640, 512, 16, 2, cfX1R5G5B5}
  };
  static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));

  *modes = videoModes;
  *modeCount = videoModeCount;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getMode(SVideoMode ** mode)
{
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::setMode(const SVideoMode * mode)
{
  pCurrentMode_ = mode;

  dma_initialize();
  graph_initialize();
  graph_set_mode(GRAPH_MODE_AUTO, GRAPH_PSM_16, GRAPH_PSM_16); // 640x512
  // Set the display buffer.
  graph_set_displaybuffer(0);
  // Set the draw buffer.
  graph_set_drawbuffer(0);
  // Set the zbuffer.
  graph_set_zbuffer(graph_get_width() * graph_get_height() * (graph_get_bpp() >> 3));
  // Clear the screen.
  graph_set_clearbuffer(0, 64, 0);
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getSurface(CSurface ** surface, ESurfaceType type)
{
  CSurface * pSurface = new CPS2Surface;

  switch(type)
  {
    case stSCREEN:
    {
      pSurface->pBack  = new pixel_t[640*512];
      pSurface->width  = pCurrentMode_->xres;
      pSurface->height = pCurrentMode_->yres;
      pSurface->format = cfX1R5G5B5;
      pSurface->pFront = pSurface->pBack;  // Fail safe?
      pSurface->p      = pSurface->pBack;
      pSurface->key    = false;
      pSurface->clKey  = 0;
      break;
    }
    case stOFFSCREEN:
    {
      pSurface->width  = 0;
      pSurface->height = 0;
      pSurface->format = cfX1R5G5B5;
      pSurface->p      = 0;
      pSurface->key    = false;
      pSurface->clKey  = 0;
      break;
    }
    default:
    {
      delete pSurface;
      pSurface = 0;
    }
  };

  *surface = pSurface;
}
