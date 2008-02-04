#include "debugScreen.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "gif.h"
#include "string.h"


SPS2VideoMode textmode[] =
{
  { 640,  256, 32, 0x03, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(652, 36, 3, 0, 2559,  255)}, // PAL
  { 640,  224, 32, 0x02, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(632, 25, 3, 0, 2559,  223)}, // NTSC
};

extern uint32_t courier_new[];
extern uint16_t fixed_tc[];


//---------------------------------------------------------------------------
CPS2DebugScreen::CPS2DebugScreen()
 : pCurrentPS2Mode_(NULL)
 , iCurrentX_(0)
 , iCurrentY_(0)
 , g2_fontbuf_addr(0)
 , g2_fontbuf_w(256)
 , g2_fontbuf_h(128)
 , g2_font_tc(0)
 , g2_font_spacing(1)
 , g2_font_mag(1)
{
  frameAddr_[0] = 0;
  frameAddr_[1] = 0;

  for(int y(0); y < TEXT_HEIGHT; y++)
    pBuffer_[y][0] = 0;
}

//---------------------------------------------------------------------------
CPS2DebugScreen::~CPS2DebugScreen()
{
  GS_RESET();
}

// -----------------------------------------------------------------------------
int
CPS2DebugScreen::init()
{
  // Set video mode
  this->setMode(&(textmode[0]));

  // Clear screen
  cls();

  return 0;
}

//---------------------------------------------------------------------------
ssize_t
CPS2DebugScreen::write(const void * data, size_t size, loff_t *)
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
  cls();

  // Draw all lines
  for(int y(0); y < TEXT_HEIGHT; y++)
    g2_out_text(15, 15 * y,  pBuffer_[y]);

  return 0;
}

//---------------------------------------------------------------------------
void
CPS2DebugScreen::cls()
{
  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(0, 0, 0, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(gs_origin_x<<4, gs_origin_y<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2((gs_origin_x+pCurrentPS2Mode_->width+1)<<4, (gs_origin_y+pCurrentPS2Mode_->height+1)<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2DebugScreen::setMode(SPS2VideoMode * mode)
{
  pCurrentPS2Mode_ = mode;

  gs_origin_x = 1024;
  gs_origin_y = 1024;

  // Create memory map
  uint32_t gs_mem_current = 0;

  // Allocate first frame
  frameAddr_[0] = gs_mem_current;
  gs_mem_current += pCurrentPS2Mode_->width * pCurrentPS2Mode_->height * (pCurrentPS2Mode_->bpp/8);

  // No double buffering (second frame points to first one)
  frameAddr_[1] = frameAddr_[0];

  // Allocate (font) texture
  g2_fontbuf_addr = gs_mem_current;
  gs_mem_current += g2_fontbuf_w * g2_fontbuf_h * (pCurrentPS2Mode_->bpp/8);

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
  setGsCrt(pCurrentPS2Mode_->interlace, pCurrentPS2Mode_->mode, pCurrentPS2Mode_->field);

  // - I havn't attempted to understand what the Alpha parameters can do. They
  //   have been blindly copied from the 3stars demo (although they don't seem
  //   do have any impact in this simple 2D code.
  GS_SET_PMODE(
      1,        // ReadCircuit1 ON
      0,        // ReadCircuit2 OFF
      1,        // Use ALP register for Alpha Blending
      1,        // Alpha Value of ReadCircuit2 for output selection
      0,        // Blend Alpha with the output of ReadCircuit2
      0xFF  // Alpha Value = 1.0
  );

  g2_set_visible_frame(0);
  g2_set_active_frame(0);

  REG_GS_DISPLAY1 = pCurrentPS2Mode_->display;
  //REG_GS_DISPLAY2 = pCurrentPS2Mode_->display;


  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);
  // Use drawing parameters from PRIM register
  GIF_DATA_AD(gs_dma_buf, prmodecont, 1);
  // Setup frame buffers. Point to 0 initially.
  GIF_DATA_AD(gs_dma_buf, frame_1, GS_FRAME(0, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0));
  // Displacement between Primitive and Window coordinate systems.
  GIF_DATA_AD(gs_dma_buf, xyoffset_1, GS_XYOFFSET(gs_origin_x<<4, gs_origin_y<<4));
  // Clip to frame buffer.
  GIF_DATA_AD(gs_dma_buf, scissor_1, GS_SCISSOR(0, pCurrentPS2Mode_->width, 0, pCurrentPS2Mode_->height));

/*
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
CPS2DebugScreen::g2_set_font(uint32_t *data, uint16_t w, uint16_t h, uint16_t *tc)
{
  g2_font_tc = tc;
  gs_load_texture(0, 0, w, h, (uint32_t)data, g2_fontbuf_addr, g2_fontbuf_w);
}

//---------------------------------------------------------------------------
void
CPS2DebugScreen::g2_out_text(uint16_t x, uint16_t y, char *str)
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
CPS2DebugScreen::g2_set_font_color(uint8_t red, uint8_t green, uint8_t blue)
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
      pCurrentPS2Mode_->width/64,
      pCurrentPS2Mode_->psm,
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
      frameAddr_[0]/8192,
      pCurrentPS2Mode_->width/64,
      pCurrentPS2Mode_->psm,
      0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2DebugScreen::g2_set_font_spacing(uint16_t s)
{
  g2_font_spacing = s;
}

//---------------------------------------------------------------------------
uint16_t
CPS2DebugScreen::g2_get_font_spacing(void)
{
  return(g2_font_spacing);
}

//---------------------------------------------------------------------------
void
CPS2DebugScreen::g2_set_font_mag(uint16_t m)
{
  g2_font_mag = m;
}

//---------------------------------------------------------------------------
uint16_t
CPS2DebugScreen::g2_get_font_mag(void)
{
  return(g2_font_mag);
}

//---------------------------------------------------------------------------
void
CPS2DebugScreen::g2_set_visible_frame(uint8_t frame)
{
  REG_GS_DISPFB1  = GS_SET_DISPFB(frameAddr_[frame] >> 13, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0, 0);
//  REG_GS_DISPFB2  = GS_SET_DISPFB(frameAddr_[frame] >> 13, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0, 0);
}

//---------------------------------------------------------------------------
void
CPS2DebugScreen::g2_set_active_frame(uint8_t frame)
{
  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, frame_1, GS_FRAME(frameAddr_[frame] >> 13, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0));

  SEND_GS_PACKET(gs_dma_buf);
}
