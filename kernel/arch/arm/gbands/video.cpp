#include "video.h"
#include "asm/arch/registers.h"
#include "font8x8.h"
#include "string.h"


#ifdef GBA
#define CHARS_PER_LINE 30
#define LINE_COUNT     20
#endif
#ifdef NDS9
#define CHARS_PER_LINE 32
#define LINE_COUNT     24
#endif


// -----------------------------------------------------------------------------
CGBAVideo::CGBAVideo()
 : CAVideo(CHARS_PER_LINE, LINE_COUNT)
 , pVideo_(reinterpret_cast<uint16_t *>(SCREEN_BASE_BLOCK(31)))
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CGBAVideo::~CGBAVideo()
{
}

// -----------------------------------------------------------------------------
int
CGBAVideo::init()
{
  REG_DISPCNT = MODE_0 | BG0_ENABLE | SPRITE_ENABLE;
  REG_BG0CNT  = BG_256_COLOR | BG_TILE_BASE(0) | BG_MAP_BASE(31);
#ifdef NDS9
  REG_POWCNT |= POWER_LCD | POWER_2D_TOP;
  REG_VRAM_A_CR = VRAM_ENABLE | VRAM_TYPE_MAIN_BG;
#endif

  // Set the background and object palette
  memcpy(BG_PALETTE, font8x8_pal, font8x8_pal_size);
  memcpy(SPRITE_PALETTE, font8x8_pal, font8x8_pal_size);
  // Set our text font
  memcpy(CHAR_BASE_BLOCK(0), font8x8, font8x8_size);

  this->cls();

  return 0;
}

// -----------------------------------------------------------------------------
void
CGBAVideo::put(int x, int y, char c)
{
  pVideo_[y * 32 + x] = c;
}

// -----------------------------------------------------------------------------
char
CGBAVideo::get(int x, int y)
{
  return pVideo_[y * 32 + x];
}
