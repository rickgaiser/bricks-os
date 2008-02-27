#include "debugScreen.h"
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
CGBADebugScreen::CGBADebugScreen()
 : CAVideo(CHARS_PER_LINE, LINE_COUNT)
 , pVideo_(reinterpret_cast<uint16_t *>(SCREEN_BASE_BLOCK(31)))
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CGBADebugScreen::~CGBADebugScreen()
{
}

// -----------------------------------------------------------------------------
int
CGBADebugScreen::init(int screen)
{
  if(screen == DEFAULT_SCREEN)
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
    // Somehow need to copy whole words per cycle, a byte gives strange results!
    for(unsigned int i(0); i < font8x8_size/2; i++)
      reinterpret_cast<uint16_t *>(CHAR_BASE_BLOCK(0))[i] = ((uint16_t *)font8x8)[i];

    pVideo_ = reinterpret_cast<uint16_t *>(SCREEN_BASE_BLOCK(31));
  }
#ifdef NDS9
  else if(screen == BOTTOM_SCREEN)
  {
    REG_SUB_DISPCNT = MODE_0 | BG0_ENABLE | SPRITE_ENABLE;
    REG_SUB_BG0CNT  = BG_256_COLOR | BG_TILE_BASE(0) | BG_MAP_BASE(31);
    REG_POWCNT |= POWER_LCD | POWER_2D_BOTTOM;
    REG_VRAM_C_CR = VRAM_ENABLE | 4;

    // Set the background and object palette
    memcpy(BG_PALETTE_SUB, font8x8_pal, font8x8_pal_size);
    memcpy(SPRITE_PALETTE_SUB, font8x8_pal, font8x8_pal_size);
    // Set our text font
    // Somehow need to copy whole words per cycle, a byte gives strange results!
    for(unsigned int i(0); i < font8x8_size/2; i++)
      reinterpret_cast<uint16_t *>(CHAR_BASE_BLOCK_SUB(0))[i] = ((uint16_t *)font8x8)[i];

    pVideo_ = reinterpret_cast<uint16_t *>(SCREEN_BASE_BLOCK_SUB(31));
  }
#endif

  this->cls();

  return 0;
}

// -----------------------------------------------------------------------------
void
CGBADebugScreen::put(int x, int y, char c)
{
  pVideo_[y * 32 + x] = c;
}

// -----------------------------------------------------------------------------
char
CGBADebugScreen::get(int x, int y)
{
  return pVideo_[y * 32 + x];
}
