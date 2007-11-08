#include "videoDevice.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "gif.h"
#include "string.h"


//---------------------------------------------------------------------------
ps2_vmode_t vmodes[] =
{
  // PAL
  {0x03,  640,  256, GRAPH_PSM_16, NON_INTERLACED, 16, GS_SET_DISPLAY(652, 36, 3, 0, 2559,  255)},
  {0x03,  640,  256, GRAPH_PSM_24, NON_INTERLACED, 24, GS_SET_DISPLAY(652, 36, 3, 0, 2559,  255)},
  {0x03,  640,  256, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(652, 36, 3, 0, 2559,  255)},
  {0x03,  640,  512, GRAPH_PSM_16, INTERLACED,     16, GS_SET_DISPLAY(652, 72, 3, 0, 2559,  511)},
  {0x03,  640,  512, GRAPH_PSM_24, INTERLACED,     24, GS_SET_DISPLAY(652, 72, 3, 0, 2559,  511)},
  {0x03,  640,  512, GRAPH_PSM_32, INTERLACED,     32, GS_SET_DISPLAY(652, 72, 3, 0, 2559,  511)},
  // NTSC
  {0x02,  640,  224, GRAPH_PSM_16, NON_INTERLACED, 16, GS_SET_DISPLAY(632, 25, 3, 0, 2559,  223)},
  {0x02,  640,  224, GRAPH_PSM_24, NON_INTERLACED, 24, GS_SET_DISPLAY(632, 25, 3, 0, 2559,  223)},
  {0x02,  640,  224, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(632, 25, 3, 0, 2559,  223)},
  {0x02,  640,  448, GRAPH_PSM_16, INTERLACED,     16, GS_SET_DISPLAY(632, 50, 3, 0, 2559,  447)},
  {0x02,  640,  448, GRAPH_PSM_24, INTERLACED,     24, GS_SET_DISPLAY(632, 50, 3, 0, 2559,  447)},
  {0x02,  640,  448, GRAPH_PSM_32, INTERLACED,     32, GS_SET_DISPLAY(632, 50, 3, 0, 2559,  447)},

  // EDTV
  {0x50,  720,  480, GRAPH_PSM_16, NON_INTERLACED, 16, GS_SET_DISPLAY(232, 35, 1, 0, 1439,  479)},
  {0x50,  720,  480, GRAPH_PSM_24, NON_INTERLACED, 24, GS_SET_DISPLAY(232, 35, 1, 0, 1439,  479)},
  {0x50,  720,  480, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(232, 35, 1, 0, 1439,  479)},
  // HDTV
  {0x52, 1280,  720, GRAPH_PSM_16, NON_INTERLACED, 16, GS_SET_DISPLAY(302, 24, 0, 0, 1279,  719)},
  {0x52, 1280,  720, GRAPH_PSM_24, NON_INTERLACED, 24, GS_SET_DISPLAY(302, 24, 0, 0, 1279,  719)},
  {0x52, 1280,  720, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(302, 24, 0, 0, 1279,  719)},
  {0x51, 1920, 1080, GRAPH_PSM_16, INTERLACED,     16, GS_SET_DISPLAY(238, 40, 0, 0, 1919, 1079)},

  // VGA
  {0x1A,  640,  480, GRAPH_PSM_16, NON_INTERLACED, 16, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 60Hz
  {0x1A,  640,  480, GRAPH_PSM_24, NON_INTERLACED, 24, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)},
  {0x1A,  640,  480, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)},
//  {0x1B,  640,  480, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 72Hz
//  {0x1C,  640,  480, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 75Hz
//  {0x1D,  640,  480, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 85Hz
  // SVGA
//  {0x2A,  800,  600, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 56Hz
  {0x2B,  800,  600, GRAPH_PSM_16, NON_INTERLACED, 16, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 60Hz
  {0x2B,  800,  600, GRAPH_PSM_24, NON_INTERLACED, 24, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)},
  {0x2B,  800,  600, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)},
//  {0x2C,  800,  600, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 72Hz
//  {0x2D,  800,  600, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 75Hz
//  {0x2E,  800,  600, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 85Hz
  // XGA
  {0x3B, 1024,  768, GRAPH_PSM_16, NON_INTERLACED, 16, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 60Hz
  {0x3B, 1024,  768, GRAPH_PSM_24, NON_INTERLACED, 24, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)},
  {0x3B, 1024,  768, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)},
//  {0x3C, 1024,  768, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 70Hz
//  {0x3D, 1024,  768, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 75Hz
//  {0x3E, 1024,  768, GRAPH_PSM_32, NON_INTERLACED, 32, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 85Hz
  // SXGA
  {0x4A, 1280, 1024, GRAPH_PSM_16, NON_INTERLACED, 16, GS_SET_DISPLAY(348, 40, 0, 0, 1279, 1023)}, // 60Hz
//  {0x4B, 1280, 1024, GRAPH_PSM_16, NON_INTERLACED, 16, GS_SET_DISPLAY(348, 40, 0, 0, 1279, 1023)}, // 75Hz
};
const uint32_t vmode_count(sizeof(vmodes) / sizeof(ps2_vmode_t));

//---------------------------------------------------------------------------
static const SVideoMode videoModes[] =
{
  // PAL
//  { 640,  256, 16, cfA1R5G5B5}, //  320Kb
//  { 640,  256, 24, cfB8G8R8},   //  480Kb
//  { 640,  256, 32, cfA8B8G8R8}, //  640Kb
//  { 640,  512, 16, cfA1R5G5B5}, //  640Kb
//  { 640,  512, 24, cfB8G8R8},   //  960Kb
  { 640,  512, 32, cfA8B8G8R8}, // 1280Kb
  // NTSC
//  { 640,  224, 16, cfA1R5G5B5}, //  280Kb
//  { 640,  224, 24, cfB8G8R8},   //  420Kb
//  { 640,  224, 32, cfA8B8G8R8}, //  560Kb
//  { 640,  448, 16, cfA1R5G5B5}, //  560Kb
//  { 640,  448, 24, cfB8G8R8},   //  840Kb
  { 640,  448, 32, cfA8B8G8R8}, // 1120Kb

  // EDTV
//  { 720,  480, 16, cfA1R5G5B5}, //  675Kb
//  { 720,  480, 24, cfB8G8R8},   // 1012Kb
//  { 720,  480, 32, cfA8B8G8R8}, // 1350Kb
  // HDTV
//  {1280,  720, 16, cfA1R5G5B5}, // 1800Kb
//  {1280,  720, 24, cfB8G8R8},   // 2700Kb
//  {1280,  720, 32, cfA8B8G8R8}, // 3600Kb
//  {1920, 1080, 16, cfA1R5G5B5}, // 4050Kb

  // VGA
//  { 640,  480, 16, cfA1R5G5B5},
//  { 640,  480, 24, cfB8G8R8},
//  { 640,  480, 32, cfA8B8G8R8},
  // SVGA
//  { 800,  600, 16, cfA1R5G5B5},
//  { 800,  600, 24, cfB8G8R8},
//  { 800,  600, 32, cfA8B8G8R8},
  // XGA
//  {1024,  768, 16, cfA1R5G5B5},
//  {1024,  768, 24, cfB8G8R8},
//  {1024,  768, 32, cfA8B8G8R8},
  // SXGA
//  {1280, 1024, 16, cfA1R5G5B5},
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));

//---------------------------------------------------------------------------
static ps2_vmode_t * cur_mode;

static uint16_t   g2_view_x0=0;                 // current viewport coordinates
static uint16_t   g2_view_x1=1;
static uint16_t   g2_view_y0=0;
static uint16_t   g2_view_y1=1;

static uint32_t   g2_frame_addr[2] = {0,0};     // address of both frame buffers in GS memory
static uint32_t   g2_texbuf_addr   = 0;         // address of texture buffer in GS memory

// Font
static uint32_t   g2_fontbuf_addr  = 0;         // address of font buffer
static uint32_t   g2_fontbuf_w     = 256;       // font buffer width
static uint32_t   g2_fontbuf_h     = 128;       // font buffer height
static uint16_t * g2_font_tc       = 0;         // pointer to texture coordinate buffer
static uint16_t   g2_font_spacing  = 1;         // pixels between each drawn character
static uint16_t   g2_font_mag      = 1;         // magnification factor

static uint8_t    g2_visible_frame;             // Identifies the frame buffer to display
static uint8_t    g2_active_frame;              // Identifies the frame buffer to direct drawing to

 uint32_t   gs_mem_current;               // points to current GS memory allocation point
 uint16_t   gs_origin_x;                  // used for mapping Primitive to Window coordinate systems
 uint16_t   gs_origin_y;

//---------------------------------------------------------------------------
DECLARE_GS_PACKET(gs_dma_buf,50);


extern uint32_t courier_new[];
extern uint16_t fixed_tc[];


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2Surface::CPS2Surface()
 : CSurface()
 , iCurrentX_(0)
 , iCurrentY_(0)
{
  for(int y(0); y < TEXT_HEIGHT; y++)
    pBuffer_[y][0] = 0;
}

//---------------------------------------------------------------------------
CPS2Surface::~CPS2Surface()
{
  if(pBack != NULL)
    delete (uint32_t *)pBack;

  GS_RESET();
}

//---------------------------------------------------------------------------
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
          pBuffer_[iCurrentY_][0] = 0;
        }
        break;
      }
      default:
      {
        if(iCurrentX_ < (TEXT_WIDTH-1))
        {
          pBuffer_[iCurrentY_][iCurrentX_] = *((const char *)data);
          iCurrentX_++;
          pBuffer_[iCurrentY_][iCurrentX_] = 0;
        }
      }
    };
    ((const char *)data)++;
    size--;
  }

  // Clear screen
  setFillColor(0, 0, 0);
  fillRect(0, 0, width_, height_);

  // Draw all lines
  for(int y(0); y < TEXT_HEIGHT; y++)
    g2_out_text(15, 15 * y,  pBuffer_[y]);

  // Swap buffers
//  swap();

  return 0;
}

//---------------------------------------------------------------------------
void
CPS2Surface::setPixel(int x, int y)
{
  x += gs_origin_x;
  y += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2Surface::fill()
{
  fillRect(0, 0, width_, height_);
}

//---------------------------------------------------------------------------
void
CPS2Surface::fillRect(int x, int y, unsigned int width, unsigned int height)
{
  x += gs_origin_x;
  y += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(fillColor_.r, fillColor_.g, fillColor_.b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2((x+width+1)<<4, (y+height+1)<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2Surface::drawLine(int x1, int y1, int x2, int y2)
{
  x1 += gs_origin_x;
  y1 += gs_origin_y;
  x2 += gs_origin_x;
  y2 += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x1<<4, y1<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x2<<4, y2<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2Surface::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  x += gs_origin_x;
  y += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_LINE_STRIP, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x+width<<4, y<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x+width<<4, y+height<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y+height<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2Surface::swap(bool sync)
{
//  if(pBack != 0)
  {
    if(sync == true)
      waitVSync();

//    g2_put_image(0, 0, width_, height_, (uint32_t *)pBack);

//    if(cur_mode->interlace == NON_INTERLACED)
//    {
      // Swap frames
      //g2_set_visible_frame(1 - g2_get_visible_frame());
      //g2_set_active_frame(1 - g2_get_active_frame());
//    }
//    else
//    {
      // Swap fields
      //g2_set_visible_field(1 - g2_get_visible_field());
      //g2_set_active_field(1 - g2_get_active_field());
//    }
  }
}

//---------------------------------------------------------------------------
void
CPS2Surface::waitHSync()
{
  REG_GS_CSR = REG_GS_CSR & 4;
  while(!(REG_GS_CSR & 4));
}

//---------------------------------------------------------------------------
void
CPS2Surface::waitVSync()
{
  REG_GS_CSR = REG_GS_CSR & 8;
  while(!(REG_GS_CSR & 8));
}

//---------------------------------------------------------------------------
void
CPS2Surface::setMode(ps2_vmode_t * mode)
{
  cur_mode = mode;

  width_  = cur_mode->width;
  height_ = cur_mode->height;

  g2_view_x0 = 0;
  g2_view_y0 = 0;
  g2_view_x1 = width_;
  g2_view_y1 = height_;

  gs_origin_x = 1024;
  gs_origin_y = 1024;

  g2_visible_frame = 0; // display frame 0
  g2_active_frame  = 0; // draw to frame 0

  // Create memory map
  gs_mem_current = 0;
  // Allocate first frame
  g2_frame_addr[0] = gs_mem_current;
  gs_mem_current += cur_mode->width * cur_mode->height * (cur_mode->bpp/8);
  // Allocate second frame is progressive
  if(cur_mode->interlace == NON_INTERLACED)
  {
    g2_frame_addr[1] = gs_mem_current;
    gs_mem_current += cur_mode->width * cur_mode->height * (cur_mode->bpp/8);
  }

  // - Initialize the DMA.
  // - Writes a 0 to most of the DMA registers.
  dma_reset();

  // - Sets the RESET bit if the GS CSR register.
  GS_RESET();

  // - Can someone please tell me what the sync.p
  // instruction does. Synchronizes something :-)
  __asm__("sync.p\nnop");

  // - Sets up the GS IMR register (i guess).
  // - The IMR register is used to mask and unmask certain interrupts,
  //   for example VSync and HSync. We'll use this properly in Tutorial 2.
  // - Does anyone have code to do this without using the 0x71 syscall?
  // - I havn't gotten around to looking at any PS2 bios code yet.
  setGsIMR(0);

  // - Use syscall 0x02 to setup some video mode stuff.
  // - Pretty self explanatory I think.
  // - Does anyone have code to do this without using the syscall? It looks
  //   like it should only set the SMODE2 register, but if I remove this syscall
  //   and set the SMODE2 register myself, it donesn't work. What else does
  //   syscall 0x02 do?
  setGsCrt(cur_mode->interlace, cur_mode->mode, FIELD);

  // - I havn't attempted to understand what the Alpha parameters can do. They
  //   have been blindly copied from the 3stars demo (although they don't seem
  //   do have any impact in this simple 2D code.
  GS_SET_PMODE(
      1,        // ReadCircuit1 OFF
      1,        // ReadCircuit2 ON
      1,        // Use ALP register for Alpha Blending
      1,        // Alpha Value of ReadCircuit2 for output selection
      0,        // Blend Alpha with the output of ReadCircuit2
      0xFF  // Alpha Value = 1.0
  );

  g2_set_visible_frame(0);

  REG_GS_DISPLAY1 = cur_mode->display;
  REG_GS_DISPLAY2 = cur_mode->display;


  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);
  // Use drawing parameters from PRIM register
  GIF_DATA_AD(gs_dma_buf, prmodecont, 1);
  // Setup frame buffers. Point to 0 initially.
  GIF_DATA_AD(gs_dma_buf, frame_1, GS_FRAME(0, cur_mode->width >> 6, cur_mode->psm, 0));
  // Displacement between Primitive and Window coordinate systems.
  GIF_DATA_AD(gs_dma_buf, xyoffset_1, GS_XYOFFSET(gs_origin_x<<4, gs_origin_y<<4));
  // Clip to frame buffer.
  GIF_DATA_AD(gs_dma_buf, scissor_1, GS_SCISSOR(0, width_, 0, height_));

/*
  // Create a single 256x128 font buffer
  g2_fontbuf_addr = gs_mem_current;
  gs_mem_current += g2_fontbuf_w * g2_fontbuf_h * (v->bpp/8);

  // Create a texture buffer as big as the screen.
  // Just save the address advance GS memory pointer by buffer size (in bytes)
  // The TEX registers are set later, when drawing.
  g2_texbuf_addr = gs_mem_current;
  gs_mem_current += v->width * v->height * (v->bpp/8);

  // Setup test_1 register to allow transparent texture regions where A=0
  GIF_DATA_AD(gs_dma_buf, test_1,
      GS_TEST(
          1,                        // Alpha Test ON
          ATST_NOTEQUAL, 0x00,  // Reject pixels with A=0
          AFAIL_KEEP,               // Don't update frame or Z buffers
          0, 0, 0, 0));         // No Destination Alpha or Z-Buffer Tests

  // Setup the ALPHA_1 register to correctly blend edges of
  // pre-antialiased fonts using Alpha Blending stage.
  // The blending formula is
  //   PIXEL=(SRC-FRAME)*SRC_ALPHA>>7+FRAME
  GIF_DATA_AD(gs_dma_buf, alpha_1,
      GS_ALPHA(
          0,            // A - source
          1,            // B - frame buffer
          0,            // C - alpha from source
          1,            // D - frame buffer
          0));      // FIX - not needed
*/

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_put_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t *data)
{
  x += gs_origin_x;
  y += gs_origin_y;

  // - Call this to copy the texture data from EE memory to GS memory.
  // - The g2_texbuf_addr variable holds the byte address of the
  //   'texture buffer' and is setup in g2_init() to be just after
  //   the frame buffer(s). When only the standard resolutions are
  //   used this buffer is guaranteed to be correctly aligned on 256
  //   bytes.
  gs_load_texture(0, 0, w, h, (uint32_t)data, g2_texbuf_addr, width_+1);

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  // Setup the Texture Buffer register. Note the width and height! They
  // must both be a power of 2.
  GIF_DATA_AD(gs_dma_buf, tex0_1,
    GS_TEX0(
      g2_texbuf_addr/256,  // base pointer
      (width_+1)/64,       // width
      0,                   // 32bit RGBA
      gs_texture_wh(w),    // width
      gs_texture_wh(h),    // height
      1,                   // RGBA
      TEX_DECAL,           // just overwrite existing pixels
      0,0,0,0,0));

  GIF_DATA_AD(gs_dma_buf, prim,
    GS_PRIM(PRIM_SPRITE,
      0,                   // flat shading
      1,                   // texture mapping ON
      0, 0, 0,             // no fog, alpha, or antialiasing
      1,                   // use UV register for coordinates.
      0,
      0));

  // Texture and vertex coordinates are specified consistently, with
  // the last four bits being the decimal part (always X.0 here).

  // Top/Left, texture coordinates (0, 0).
  GIF_DATA_AD(gs_dma_buf, uv,    GS_UV(0, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2,  GS_XYZ2(x<<4, y<<4, 0));

  // Bottom/Right, texture coordinates (w, h).
  GIF_DATA_AD(gs_dma_buf, uv,    GS_UV(w<<4, h<<4));
  GIF_DATA_AD(gs_dma_buf, xyz2,  GS_XYZ2((x+w)<<4, (y+h)<<4, 0));

  // Finally send the command buffer to the GIF.
  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_font(uint32_t *data, uint16_t w, uint16_t h, uint16_t *tc)
{
  g2_font_tc = tc;
  gs_load_texture(0, 0, w, h, (uint32_t)data, g2_fontbuf_addr, g2_fontbuf_w);
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_out_text(uint16_t x, uint16_t y, char *str)
{
  char c;                   // current character
  uint16_t * tc;            // current texture coordinates [4]
  uint16_t x0, y0, x1, y1;  // rectangle for current character
  uint16_t w, h;            // width and height of above rectangle

  x += gs_origin_x;
  y += gs_origin_y;

  c = *str;
  while(c)
  {
    // Read the texture coordinates for current character
    tc = &g2_font_tc[c*4];
    x0 = *tc++;
    y0 = *tc++;
    x1 = *tc++;
    y1 = *tc++;
    w  = x1-x0+1;
    h  = y1-y0+1;

    // Draw a sprite with current character mapped onto it
    BEGIN_GS_PACKET(gs_dma_buf);
    GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

    GIF_DATA_AD(gs_dma_buf, tex0_1,
      GS_TEX0(
        g2_fontbuf_addr/256,            // base pointer
        (g2_fontbuf_w)/64,              // width
        0,                              // 32bit RGBA
        gs_texture_wh(g2_fontbuf_w),    // width
        gs_texture_wh(g2_fontbuf_w),    // height
        1,                              // RGBA
        TEX_DECAL,                      // just overwrite existing pixels
        0,0,0,0,0));
/*
    GIF_DATA_AD(gs_dma_buf, tex1_1,
      GS_TEX1(
        0, 0,
        FILTER_LINEAR,
        FILTER_LINEAR,
        0, 0, 0));

    GIF_DATA_AD(gs_dma_buf, clamp_1, 0x05);
*/
    GIF_DATA_AD(gs_dma_buf, prim,
      GS_PRIM(PRIM_SPRITE,
        0,                              // flat shading
        1,                              // texture mapping ON
        0, 1, 0,                        // no fog or antialiasing, but use alpha
        1,                              // use UV register for coordinates.
        0,
        0));

    GIF_DATA_AD(gs_dma_buf, uv,    GS_UV(x0<<4, y0<<4));
    GIF_DATA_AD(gs_dma_buf, xyz2,  GS_XYZ2(x<<4, y<<4, 0));
    GIF_DATA_AD(gs_dma_buf, uv,    GS_UV((x1+1)<<4, (y1+1)<<4));
    GIF_DATA_AD(gs_dma_buf, xyz2,  GS_XYZ2((x+w*g2_font_mag)<<4, (y+h*g2_font_mag)<<4, 0));

    SEND_GS_PACKET(gs_dma_buf);

    // Advance drawing position
    x += (w + g2_font_spacing) * g2_font_mag;

    // Get next character
    str++;
    c = *str;
  }
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_font_color(uint8_t red, uint8_t green, uint8_t blue)
{
  uint64_t x0;
  uint64_t y0;
  uint64_t x1;
  uint64_t y1;

  x0 = gs_origin_x-10;
  y0 = gs_origin_y-10;
  x1 = g2_fontbuf_w + gs_origin_x;
  y1 = g2_fontbuf_h + gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  // Restore the frame buffer to the current active frame buffer.
  GIF_DATA_AD(gs_dma_buf, frame_1,
    GS_FRAME(
      g2_fontbuf_addr/8192,
      cur_mode->width/64,
      cur_mode->psm,
      0));

  // Draw the colored rectangle over the entire Font Bitmap.
  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  //GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));

  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(red, green, blue, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x0<<4, y0<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x1<<4, y1<<4, 0));

  // Restore the frame buffer to the current active frame buffer.
  GIF_DATA_AD(gs_dma_buf, frame_1,
    GS_FRAME(
      g2_frame_addr[g2_active_frame]/8192,
      cur_mode->width/64,
      cur_mode->psm,
      0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_font_spacing(uint16_t s)
{
  g2_font_spacing = s;
}

//---------------------------------------------------------------------------
uint16_t
CPS2Surface::g2_get_font_spacing(void)
{
  return(g2_font_spacing);
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_font_mag(uint16_t m)
{
  g2_font_mag = m;
}

//---------------------------------------------------------------------------
uint16_t
CPS2Surface::g2_get_font_mag(void)
{
  return(g2_font_mag);
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_visible_frame(uint8_t frame)
{
  REG_GS_DISPFB1  = GS_SET_DISPFB(g2_frame_addr[frame] >> 13, width_ >> 6, cur_mode->psm, 0, 0);
  REG_GS_DISPFB2  = GS_SET_DISPFB(g2_frame_addr[frame] >> 13, width_ >> 6, cur_mode->psm, 0, 0);
  g2_visible_frame = frame;
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_active_frame(uint8_t frame)
{
  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, frame_1, GS_FRAME(g2_frame_addr[frame] >> 13, width_ >> 6, cur_mode->psm, 0));

  SEND_GS_PACKET(gs_dma_buf);

  g2_active_frame = frame;
}

//---------------------------------------------------------------------------
uint8_t
CPS2Surface::g2_get_visible_frame(void)
{
  return g2_visible_frame;
}

//---------------------------------------------------------------------------
uint8_t
CPS2Surface::g2_get_active_frame(void)
{
  return g2_active_frame;
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_visible_field(uint8_t field)
{
//  REG_GS_DISPFB1  = GS_SET_DISPFB(g2_frame_addr[frame] >> 13, width_ >> 6, cur_mode->psm, 0, 0);
//  REG_GS_DISPFB2  = GS_SET_DISPFB(g2_frame_addr[frame] >> 13, width_ >> 6, cur_mode->psm, 0, 0);
//  g2_visible_field = field;
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_active_field(uint8_t field)
{
  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  // If the field is odd...
  if(field == 1)
  {
    GIF_DATA_AD(gs_dma_buf, scanmsk, 2);
  }
  else if(field == 0)
  {
    GIF_DATA_AD(gs_dma_buf, scanmsk, 3);
  }
  else
  {
    GIF_DATA_AD(gs_dma_buf, scanmsk, 0);
  }

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
uint8_t
CPS2Surface::g2_get_visible_field(void)
{
  if(REG_GS_CSR & (1 << 13))
    return 1; // Odd
  else
    return 0; // Even
}

//---------------------------------------------------------------------------
uint8_t
CPS2Surface::g2_get_active_field(void)
{
  if(REG_GS_CSR & (1 << 13))
    return 0; // Even
  else
    return 1; // Odd
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_viewport(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  g2_view_x0 = x0;
  g2_view_x1 = x1;
  g2_view_y0 = y0;
  g2_view_y1 = y1;

  BEGIN_GS_PACKET(gs_dma_buf);

  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, scissor_1, GS_SCISSOR(x0, x1, y0, y1));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_get_viewport(uint16_t *x0, uint16_t *y0, uint16_t *x1, uint16_t *y1)
{
  *x0 = g2_view_x0;
  *x1 = g2_view_x1;
  *y0 = g2_view_y0;
  *y1 = g2_view_y1;
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
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getSurface(CSurface ** surface, ESurfaceType type, bool bDouble)
{
  CPS2Surface * pSurface = 0;

  switch(type)
  {
    case stSCREEN:
    {
      for(uint32_t i(0); i < vmode_count; i++)
      {
        if((pCurrentMode_->width  == vmodes[i].width) &&
           (pCurrentMode_->height == vmodes[i].height) &&
           (pCurrentMode_->bpp    == vmodes[i].bpp))
        {
          pSurface = new CPS2Surface;
          pSurface->setMode(&vmodes[i]);
          pSurface->width_ = pCurrentMode_->width;
          pSurface->height_= pCurrentMode_->height;
          pSurface->bpp_   = pCurrentMode_->bpp;
          pSurface->format_= pCurrentMode_->format;
          pSurface->pBack  = 0;//new uint32_t[pCurrentMode_->xres * pCurrentMode_->yres];
          pSurface->pFront = pSurface->pBack;  // Fail safe?
          pSurface->p      = pSurface->pBack;

          // Set active/Visible frame
          pSurface->g2_set_active_frame(0);
          pSurface->g2_set_visible_frame(0);

          // Clear screen
          pSurface->setFillColor(0, 0, 0);
          pSurface->fill();

          // Set font
          pSurface->g2_set_font(courier_new, 256, 128, fixed_tc);
          pSurface->g2_set_font_spacing(0);
          pSurface->g2_set_font_mag(1);

          break;
        }
      }
    }
    default:
      ;
  };

  *surface = pSurface;
}
