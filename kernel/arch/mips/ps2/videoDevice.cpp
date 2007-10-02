#include "videoDevice.h"
#include "ps2.h"
#include "dma.h"
#include "gs.h"
#include "gif.h"
#include "string.h"


// int_mode
#define NON_INTERLACED	0
#define INTERLACED		1
// ntsc_pal
#define NTSC			2
#define PAL				3
// field_mode
#define FRAME			1
#define FIELD			2

//---------------------------------------------------------------------------
typedef struct
{
  uint16_t ntsc_pal;
  uint16_t width;
  uint16_t height;
  uint16_t psm;
  uint16_t bpp;
  uint16_t magh;
} vmode_t __attribute__((aligned(16)));

vmode_t vmodes[] =
{
   {PAL, 256, 256, 0, 32, 10}  // PAL_256_512_32
  ,{PAL, 320, 256, 0, 32, 8}   // PAL_320_256_32
  ,{PAL, 384, 256, 0, 32, 7}   // PAL_384_256_32
  ,{PAL, 512, 256, 0, 32, 5}   // PAL_512_256_32
  ,{PAL, 640, 256, 0, 32, 4}   // PAL_640_256_32

  ,{NTSC, 256, 224, 0, 32, 10} // PAL_256_512_32
  ,{NTSC, 320, 224, 0, 32, 8}  // PAL_320_256_32
  ,{NTSC, 384, 224, 0, 32, 7}  // PAL_384_256_32
  ,{NTSC, 512, 224, 0, 32, 5}  // PAL_512_256_32
  ,{NTSC, 640, 224, 0, 32, 4}  // PAL_640_256_32
};

static vmode_t *cur_mode;

//---------------------------------------------------------------------------
static uint8_t	g2_col_r=0;		// current draw color
static uint8_t	g2_col_g=0;
static uint8_t	g2_col_b=0;

static uint8_t	g2_fill_r=0;	// current fill color
static uint8_t	g2_fill_g=0;
static uint8_t	g2_fill_b=0;

static uint16_t	g2_view_x0=0;	// current viewport coordinates
static uint16_t	g2_view_x1=1;
static uint16_t	g2_view_y0=0;
static uint16_t	g2_view_y1=1;

static uint32_t	g2_frame_addr[2] = {0,0};	// address of both frame buffers in GS memory
static uint32_t	g2_texbuf_addr   = 0;		// address of texture buffer in GS memory

static uint32_t	g2_fontbuf_addr  = 0;		// address of font buffer
static uint32_t	g2_fontbuf_w     = 256;		// font buffer width
static uint32_t	g2_fontbuf_h     = 128;		// font buffer height
static uint16_t	*g2_font_tc      = 0;		// pointer to texture coordinate buffer
static uint16_t	g2_font_spacing  = 1;		// pixels between each drawn character
static uint16_t	g2_font_mag      = 1;		// magnification factor

static uint8_t	g2_visible_frame;			// Identifies the frame buffer to display
static uint8_t	g2_active_frame;			// Identifies the frame buffer to direct drawing to

static uint32_t	gs_mem_current;	// points to current GS memory allocation point
static uint16_t	gs_origin_x;	// used for mapping Primitive to Window coordinate systems
static uint16_t	gs_origin_y;

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
//  setMode(PAL_256_256_32);
//  setMode(PAL_320_256_32);
//  setMode(PAL_384_256_32);
  setMode(PAL_512_256_32);
//  setMode(PAL_640_256_32);

  // Clear frame 0
  g2_set_active_frame(0);
  setFillColor(0, 0, 0);
  fillRect(0, 0, width_, height_);

  // Show frame 0
  g2_set_visible_frame(0);

  // Clear frame 1
//  g2_set_active_frame(1);
//  setFillColor(0, 0, 0);
//  fillRect(0, 0, width_, height_);

  // Set font
  g2_set_font(courier_new, 256, 128, fixed_tc);
  g2_set_font_spacing(0);
  g2_set_font_mag(1);

  for(int y(0); y < TEXT_HEIGHT; y++)
    pBuffer_[y][0] = 0;
}

//---------------------------------------------------------------------------
CPS2Surface::~CPS2Surface()
{
  GS_RESET();
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
	GIF_TAG_AD(gs_dma_buf, 6, 1, 0, 0, 0);

	// Setup the Texture Buffer register. Note the width and height! They
	// must both be a power of 2.
	GIF_DATA_AD(gs_dma_buf, tex0_1,
		GS_TEX0(
			g2_texbuf_addr/256,	// base pointer
			(width_+1)/64,	// width
			0,					// 32bit RGBA
			gs_texture_wh(w),	// width
			gs_texture_wh(h),	// height
			1,					// RGBA
			TEX_DECAL,			// just overwrite existing pixels
			0,0,0,0,0));

	GIF_DATA_AD(gs_dma_buf, prim,
		GS_PRIM(PRIM_SPRITE,
			0,			// flat shading
			1, 			// texture mapping ON
			0, 0, 0,	// no fog, alpha, or antialiasing
			1, 			// use UV register for coordinates.
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
char	c;				// current character
uint16_t	*tc;			// current texture coordinates [4]
uint16_t	x0, y0, x1, y1;	// rectangle for current character
uint16_t	w, h;			// width and height of above rectangle

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
		GIF_TAG_AD(gs_dma_buf, 6, 1, 0, 0, 0);

		GIF_DATA_AD(gs_dma_buf, tex0_1,
			GS_TEX0(
				g2_fontbuf_addr/256,		// base pointer
				(g2_fontbuf_w)/64,			// width
				0,							// 32bit RGBA
				gs_texture_wh(g2_fontbuf_w),// width
				gs_texture_wh(g2_fontbuf_w),// height
				1,							// RGBA
				TEX_DECAL,					// just overwrite existing pixels
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
				0,			// flat shading
				1, 			// texture mapping ON
				0, 1, 0,	// no fog or antialiasing, but use alpha
				1, 			// use UV register for coordinates.
				0,
				0));

		GIF_DATA_AD(gs_dma_buf, uv,    GS_UV(x0<<4, y0<<4));
		GIF_DATA_AD(gs_dma_buf, xyz2,  GS_XYZ2(x<<4, y<<4, 0));
		GIF_DATA_AD(gs_dma_buf, uv,    GS_UV((x1+1)<<4, (y1+1)<<4));
		GIF_DATA_AD(gs_dma_buf, xyz2,  GS_XYZ2(
				(x+w*g2_font_mag)<<4, (y+h*g2_font_mag)<<4, 0));

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
	GIF_TAG_AD(gs_dma_buf, 6, 1, 0, 0, 0);

	// Restore the frame buffer to the current active frame buffer.
	GIF_DATA_AD(gs_dma_buf, frame_1,
		GS_FRAME(
			g2_fontbuf_addr/8192,
			cur_mode->width/64,
			cur_mode->psm,
			0));

	// Draw the colored rectangle over the entire Font Bitmap.
	GIF_DATA_AD(gs_dma_buf, prim,
		GS_PRIM(PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
//		GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));

	GIF_DATA_AD(gs_dma_buf, rgbaq,
		GS_RGBAQ(red, green, blue, 0x80, 0));

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
	GS_SET_DISPFB2(
		g2_frame_addr[frame]/8192,	// Frame Buffer base pointer = Address/8192
		cur_mode->width/64,			// Buffer Width (Pixels/64)
		cur_mode->psm,				// Pixel Storage Format
		0,							// Upper Left X in Buffer = 0
		0							// Upper Left Y in Buffer = 0
	);

	g2_visible_frame = frame;
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_set_active_frame(uint8_t frame)
{
	BEGIN_GS_PACKET(gs_dma_buf);
	GIF_TAG_AD(gs_dma_buf, 1, 1, 0, 0, 0);

	GIF_DATA_AD(gs_dma_buf, frame_1,
		GS_FRAME(
			g2_frame_addr[frame]/8192,	// FrameBuffer base pointer = Address/8192
			cur_mode->width/64,			// Frame buffer width (Pixels/64)
			cur_mode->psm,				// Pixel Storage Format
			0));

	SEND_GS_PACKET(gs_dma_buf);

	g2_active_frame = frame;
}

//---------------------------------------------------------------------------
uint8_t
CPS2Surface::g2_get_visible_frame(void)
{
	return(g2_visible_frame);
}

//---------------------------------------------------------------------------
uint8_t
CPS2Surface::g2_get_active_frame(void)
{
	return(g2_active_frame);
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_wait_vsync(void)
{
	*CSR = *CSR & 8;
	while(!(*CSR & 8));
}

//---------------------------------------------------------------------------
void
CPS2Surface::g2_wait_hsync(void)
{
	*CSR = *CSR & 4;
	while(!(*CSR & 4));
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

	GIF_TAG_AD(gs_dma_buf, 1, 1, 0, 0, 0);

	GIF_DATA_AD(gs_dma_buf, scissor_1,
		GS_SCISSOR(x0, x1, y0, y1));

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
void
CPS2Surface::setMode(g2_video_mode mode)
{
  vmode_t * v;

  v = &(vmodes[mode]);
  cur_mode = v;

  width_  = v->width - 1;
  height_ = v->height - 1;

  g2_view_x0 = 0;
  g2_view_y0 = 0;
  g2_view_x1 = width_;
  g2_view_y1 = height_;

  gs_origin_x = 1024;
  gs_origin_y = 1024;

  gs_mem_current = 0;		// nothing allocated yet

  g2_visible_frame = 0;	// display frame 0
  g2_active_frame  = 0;	// draw to frame 0

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
  gs_set_imr();

  // - Use syscall 0x02 to setup some video mode stuff.
  // - Pretty self explanatory I think.
  // - Does anyone have code to do this without using the syscall? It looks
  //   like it should only set the SMODE2 register, but if I remove this syscall
  //   and set the SMODE2 register myself, it donesn't work. What else does
  //   syscall 0x02 do?
  gs_set_crtc(NON_INTERLACED, v->ntsc_pal, FRAME);

  // - I havn't attempted to understand what the Alpha parameters can do. They
  //   have been blindly copied from the 3stars demo (although they don't seem
  //   do have any impact in this simple 2D code.
  GS_SET_PMODE(
      0,		// ReadCircuit1 OFF
      1,		// ReadCircuit2 ON
      1,		// Use ALP register for Alpha Blending
      1,		// Alpha Value of ReadCircuit2 for output selection
      0,		// Blend Alpha with the output of ReadCircuit2
      0xFF	// Alpha Value = 1.0
  );
/*
  // - Non needed if we use gs_set_crt()
  GS_SET_SMODE2(
      0,		// Non-Interlaced mode
      1,		// FRAME mode (read every line)
      0		// VESA DPMS Mode = ON		??? please explain ???
  );
*/
  GS_SET_DISPFB2(
      0,				// Frame Buffer base pointer = 0 (Address/8192)
      v->width/64,	// Buffer Width (Pixels/64)
      v->psm,			// Pixel Storage Format
      0,				// Upper Left X in Buffer = 0
      0				// Upper Left Y in Buffer = 0
  );

  // Why doesn't (0, 0) equal the very top-left of the TV?
  GS_SET_DISPLAY2(
      656,		// X position in the display area (in VCK units)
      36,			// Y position in the display area (in Raster units)
      v->magh-1,	// Horizontal Magnification - 1
      0,						// Vertical Magnification = 1x
      v->width*v->magh-1,		// Display area width  - 1 (in VCK units) (Width*HMag-1)
      v->height-1				// Display area height - 1 (in pixels)	  (Height-1)
  );

  GS_SET_BGCOLOR(
      0,	// RED
      0,	// GREEN
      0	// BLUE
  );


  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 6, 1, 0, 0, 0);

  // Use drawing parameters from PRIM register
  GIF_DATA_AD(gs_dma_buf, prmodecont, 1);

  // Setup frame buffers. Point to 0 initially.
  GIF_DATA_AD(gs_dma_buf, frame_1,
      GS_FRAME(
          0,					// FrameBuffer base pointer = 0 (Address/8192)
          v->width/64,		// Frame buffer width (Pixels/64)
          v->psm,				// Pixel Storage Format
          0));

  // Save address and advance GS memory pointer by buffer size (in bytes)
  // Do this for both frame buffers.
  g2_frame_addr[0] = gs_mem_current;
  gs_mem_current += v->width * v->height * (v->bpp/8);

  g2_frame_addr[1] = gs_mem_current;
  gs_mem_current += v->width * v->height * (v->bpp/8);

  // Displacement between Primitive and Window coordinate systems.
  GIF_DATA_AD(gs_dma_buf, xyoffset_1,
      GS_XYOFFSET(
          gs_origin_x<<4,
          gs_origin_y<<4));

  // Clip to frame buffer.
  GIF_DATA_AD(gs_dma_buf, scissor_1,
      GS_SCISSOR(
          0,
          width_,
          0,
          height_));

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
          1,						// Alpha Test ON
          ATST_NOTEQUAL, 0x00,	// Reject pixels with A=0
          AFAIL_KEEP,				// Don't update frame or Z buffers
          0, 0, 0, 0));			// No Destination Alpha or Z-Buffer Tests

  // Setup the ALPHA_1 register to correctly blend edges of
  // pre-antialiased fonts using Alpha Blending stage.
  // The blending formula is
  //   PIXEL=(SRC-FRAME)*SRC_ALPHA>>7+FRAME
  GIF_DATA_AD(gs_dma_buf, alpha_1,
      GS_ALPHA(
          0,			// A - source
          1, 			// B - frame buffer
          0,			// C - alpha from source
          1, 			// D - frame buffer
          0));		// FIX - not needed

  SEND_GS_PACKET(gs_dma_buf);
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
CPS2Surface::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  CSurface::setColor(r, g, b);
  g2_col_r = r;
  g2_col_g = g;
  g2_col_b = b;
}

//---------------------------------------------------------------------------
void
CPS2Surface::setFillColor(uint8_t r, uint8_t g, uint8_t b)
{
  CSurface::setFillColor(r, g, b);
  g2_fill_r = r;
  g2_fill_g = g;
  g2_fill_b = b;
}

//---------------------------------------------------------------------------
void
CPS2Surface::setPixel(int x, int y)
{
  x += gs_origin_x;
  y += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);

  GIF_TAG_AD(gs_dma_buf, 4, 1, 0, 0, 0);
  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(g2_col_r, g2_col_g, g2_col_b, 0x80, 0));
  // The XYZ coordinates are actually floating point numbers between
  // 0 and 4096 represented as unsigned integers where the lowest order
  // four bits are the fractional point. That's why all coordinates are
  // shifted left 4 bits.
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
CPS2Surface::fillRect(int x, int y, int width, int height)
{
  x += gs_origin_x;
  y += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);

  GIF_TAG_AD(gs_dma_buf, 4, 1, 0, 0, 0);
  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(g2_fill_r, g2_fill_g, g2_fill_b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  // It looks like the default operation for the SPRITE primitive is to
  // not draw the right and bottom 'lines' of the rectangle refined by
  // the parameters. Add +1 to change this.
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

  GIF_TAG_AD(gs_dma_buf, 4, 1, 0, 0, 0);
  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(g2_col_r, g2_col_g, g2_col_b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x1<<4, y1<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x2<<4, y2<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS2Surface::drawRect(int x, int y, int width, int height)
{
  x += gs_origin_x;
  y += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);

  GIF_TAG_AD(gs_dma_buf, 7, 1, 0, 0, 0);
  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_LINE_STRIP, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(g2_col_r, g2_col_g, g2_col_b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x+width<<4, y<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x+width<<4, y+height<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y+height<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
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
