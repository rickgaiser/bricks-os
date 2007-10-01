#include "videoDevice.h"
#include "g2.h"
#include "string.h"


extern uint32_t courier_new[];
extern uint16_t fixed_tc[];


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2Surface::CPS2Surface()
 : CSurface()
 , iCurrentX_(0)
 , iCurrentY_(0)
{
  g2_init(PAL_512_256_32);

  width_  = g2_get_max_x();
  height_ = g2_get_max_y();

  // Clear frame 0
  g2_set_active_frame(0);
  g2_set_fill_color(0, 0, 0);
  g2_fill_rect(0, 0, width_, height_);

  // Show frame 0
  g2_set_visible_frame(0);

  // Clear frame 1
//  g2_set_active_frame(1);
//  g2_set_fill_color(0, 0, 0);
//  g2_fill_rect(0, 0, width_, height_);

  // Set font
  g2_set_font(courier_new, 256, 128, fixed_tc);
  g2_set_font_spacing(0);
  g2_set_font_mag(1);

  // Fill background
  //g2_set_fill_color(0, 0, 0);
  //g2_fill_rect(0, 0, iWidth_, iHeight_);

  for(int y(0); y < TEXT_HEIGHT; y++)
    pBuffer_[y][0] = 0;
}

//---------------------------------------------------------------------------
CPS2Surface::~CPS2Surface()
{
}

// -----------------------------------------------------------------------------
ssize_t
CPS2Surface::write(const void * data, size_t size, loff_t *)
{
  while((*((const char *)data) != 0) && (size > 0))
  {
    switch(*((const char *)data))
    {
      case '\n':
      {
        // Advance to next line
        iCurrentY_++;
        iCurrentX_ = 0;
        if(iCurrentY_ >= TEXT_HEIGHT)
        {
          // Scroll buffer up 1 line
          iCurrentY_ = TEXT_HEIGHT - 1;
          for(int y(0); y < TEXT_HEIGHT-1; y++)
            strcpy(pBuffer_[y], pBuffer_[y+1]);
        }
      }
      default:
      {
        if(iCurrentX_ < (TEXT_WIDTH-1))
        {
          pBuffer_[iCurrentY_][iCurrentX_] = *((const char *)data);
          iCurrentX_++;
          pBuffer_[iCurrentY_][iCurrentX_] = 0;
          ((const char *)data)++;
          size--;
        }
      }
    };
  }

  // Clear screen
  g2_set_fill_color(0, 0, 0);
  g2_fill_rect(0, 0, width_, height_);

  // Draw all lines
  for(int y(0); y < TEXT_HEIGHT; y++)
    g2_out_text(15, 15 * y,  pBuffer_[y]);

  // Swap buffers
//  swap();

  return 0;
}

//---------------------------------------------------------------------------
void
CPS2Surface::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  CSurface::setColor(r, g, b);
  g2_set_color(r, g, b);
}

//---------------------------------------------------------------------------
void
CPS2Surface::setFillColor(uint8_t r, uint8_t g, uint8_t b)
{
  CSurface::setFillColor(r, g, b);
  g2_set_fill_color(r, g, b);
}

//---------------------------------------------------------------------------
void
CPS2Surface::setPixel(int x, int y)
{
  g2_put_pixel(x, y);
}

//---------------------------------------------------------------------------
void
CPS2Surface::fill()
{
  fillRect(0, 0, width_, height_);
}

//---------------------------------------------------------------------------
void
CPS2Surface::fillRect(int x, int y, int width, int height)
{
  g2_fill_rect(x, y, width, height);
}

//---------------------------------------------------------------------------
void
CPS2Surface::drawLine(int x1, int y1, int x2, int y2)
{
  g2_line(x1, y1, x2, y2);
}

//---------------------------------------------------------------------------
void
CPS2Surface::drawRect(int x, int y, int width, int height)
{
  g2_rect(x, y, width, height);
}

//---------------------------------------------------------------------------
void
CPS2Surface::swap(bool bForceCopy)
{
//  g2_wait_vsync();
//  g2_set_visible_frame(1 - g2_get_visible_frame());
//  g2_set_active_frame(1 - g2_get_active_frame());
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
    {512, 256, 32, 4, cfA8B8G8R8}
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
/*
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
*/
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getSurface(CSurface ** surface, ESurfaceType type)
{
  CSurface * pSurface = new CPS2Surface;
/*
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
*/
  *surface = pSurface;
}
