#include "debugScreen.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "gif.h"
#include "string.h"


extern SPS2VideoMode vmodes[];
extern uint32_t courier_new[];
extern uint16_t fixed_tc[];

uint64_t pdata[50 * 2] __attribute__((aligned(64)));


//---------------------------------------------------------------------------
CPS2DebugScreen::CPS2DebugScreen()
 : pCurrentPS2Mode_(NULL)
 , iCurrentX_(0)
 , iCurrentY_(0)
 , g2_fontbuf_addr(0)
 , g2_fontbuf_w(256)
 , g2_fontbuf_h(128)
 , g2_font_tc(0)
 , packet_(pdata, 50, DMAC::Channel::GIF)
{
  frameAddr_[0] = 0;
  frameAddr_[1] = 0;

  for(int y(0); y < TEXT_HEIGHT; y++)
    pBuffer_[y][0] = 0;
}

//---------------------------------------------------------------------------
CPS2DebugScreen::~CPS2DebugScreen()
{
  REG_GS_CSR = GS_CSR_RESET();
}

// -----------------------------------------------------------------------------
int
CPS2DebugScreen::init()
{
  // Set video mode
  this->setMode(&(vmodes[2]));

  // Clear screen
  cls();

  // Set font
  g2_font_tc = fixed_tc;
  ee_to_gsBitBlt(g2_fontbuf_addr, g2_fontbuf_w, 0, 0, 0, 256, 128, (uint32_t)courier_new);

  return 0;
}

//---------------------------------------------------------------------------
ssize_t
CPS2DebugScreen::write(const void * data, size_t size, bool block)
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
    printLine(15, 15 * y,  pBuffer_[y]);

  return 0;
}

//---------------------------------------------------------------------------
void
CPS2DebugScreen::cls()
{
  packet_.scTagOpenEnd();
    packet_.gifTagOpen();
      packet_.addSetGSReg(prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
      packet_.addSetGSReg(rgbaq, GS_RGBAQ(0, 0, 0, 0x80, 0));
      packet_.addSetGSReg(xyz2, GS_XYZ2(gs_origin_x<<4, gs_origin_y<<4, 0));
      packet_.addSetGSReg(xyz2, GS_XYZ2((gs_origin_x+pCurrentPS2Mode_->width+1)<<4, (gs_origin_y+pCurrentPS2Mode_->height+1)<<4, 0));
    packet_.gifTagClose();
  packet_.scTagClose();
  packet_.send();
  packet_.reset();
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

  // Reset the GS
  REG_GS_CSR = GS_CSR_RESET();

  // Sync
  __asm__("sync.p\nnop");

  // Clear GS interrupt mask register
  bios::GsPutIMR(0);

  // Setup CRTC for video mode
  bios::SetGsCrt(pCurrentPS2Mode_->interlace, pCurrentPS2Mode_->crtcMode->biosMode, pCurrentPS2Mode_->field);

  // Enable read circuit 1
  REG_GS_PMODE = GS_PMODE(
    1,        // ReadCircuit1 ON
    0,        // ReadCircuit2 OFF
    1,        // Use ALP register for Alpha Blending
    1,        // Alpha Value of ReadCircuit2 for output selection
    0,        // Blend Alpha with the output of ReadCircuit2
    0xff      // Alpha Value = 1.0
  );

  // Setup read circuit 1
  REG_GS_DISPLAY1 = GS_DISPLAY_CREATE(pCurrentPS2Mode_->crtcMode->vck,
                                      pCurrentPS2Mode_->xoffset,
                                      pCurrentPS2Mode_->yoffset,
                                      pCurrentPS2Mode_->width,
                                      pCurrentPS2Mode_->height);

  // Setup read circuit 2
  //REG_GS_DISPLAY2 = ...;

  // Display buffer
  REG_GS_DISPFB1  = GS_DISPFB(frameAddr_[0] >> 13, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0, 0);

  packet_.scTagOpenEnd();
    packet_.gifTagOpen();
      // Render buffer
      packet_.addSetGSReg(frame_1, GS_FRAME(frameAddr_[0] >> 13, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0));
      // Use drawing parameters from PRIM register
      packet_.addSetGSReg(prmodecont, 1);
      // Setup frame buffers. Point to 0 initially.
      packet_.addSetGSReg(frame_1, GS_FRAME(0, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0));
      // Displacement between Primitive and Window coordinate systems.
      packet_.addSetGSReg(xyoffset_1, GS_XYOFFSET(gs_origin_x<<4, gs_origin_y<<4));
      // Clip to frame buffer.
      packet_.addSetGSReg(scissor_1, GS_SCISSOR(0, pCurrentPS2Mode_->width, 0, pCurrentPS2Mode_->height));
    packet_.gifTagClose();
  packet_.scTagClose();
  packet_.send();
  packet_.reset();
}

//---------------------------------------------------------------------------
void
CPS2DebugScreen::printLine(uint16_t x, uint16_t y, char * str)
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

    packet_.scTagOpenEnd();
      packet_.gifTagOpen();

        // Draw a sprite with current character mapped onto it
        packet_.addSetGSReg(tex0_1,
          GS_TEX0(
            g2_fontbuf_addr/256,            // base pointer
            (g2_fontbuf_w)/64,              // width
            0,                              // 32bit RGBA
            getBitNr(g2_fontbuf_w),         // width
            getBitNr(g2_fontbuf_w),         // height
            1,                              // RGBA
            TEX_DECAL,                      // just overwrite existing pixels
            0,0,0,0,0));
/*
        packet_.addSetGSReg(tex1_1,
          GS_TEX1(
            0, 0,
            FILTER_LINEAR,
            FILTER_LINEAR,
            0, 0, 0));

        packet_.addSetGSReg(clamp_1, 0x05);
*/
        packet_.addSetGSReg(prim,
          GS_PRIM(PRIM_SPRITE,
            0,                              // flat shading
            1,                              // texture mapping ON
            0, 1, 0,                        // no fog or antialiasing, but use alpha
            1,                              // use UV register for coordinates.
            0,
            0));

        packet_.addSetGSReg(uv,    GS_UV(x0<<4, y0<<4));
        packet_.addSetGSReg(xyz2,  GS_XYZ2(x<<4, y<<4, 0));
        packet_.addSetGSReg(uv,    GS_UV((x1+1)<<4, (y1+1)<<4));
        packet_.addSetGSReg(xyz2,  GS_XYZ2((x+w)<<4, (y+h)<<4, 0));
      packet_.gifTagClose();
    packet_.scTagClose();
    packet_.send();
    packet_.reset();

    // Advance drawing position
    x += w;

    // Get next character
    str++;
    c = *str;
  }
}
