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
 , pVideo_(NULL)
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
    REG_BG0CNT  = BG_256_COLOR | BG_TILE_BASE(0) | BG_MAP_BASE(31);        // Tiles at 0x06000000, Map at 0x0600f800
#ifdef GBA
    REG_DISPCNT = MODE_0 | BG0_ENABLE | SPRITE_ENABLE;
#endif
#ifdef NDS9
    REG_DISPCNT = MODE_0 | BG0_ENABLE | SPRITE_ENABLE | DISP_SOURCE_ENGINE;
    REG_POWCNT |= POWER_LCD | POWER_2D_TOP;
    REG_VRAM_A_CR = VRAM_ENABLE | VRAM_OFFSET(0) | VRAM_TYPE_MAIN_BG;      // 128KiB @ 0x06000000 (EngineA)
#endif

    // Set the background and object palette
    // NOTE: We need to copy whole words per cycle, a byte gives strange results!
    for(unsigned int i(0); i < (font8x8_pal_size / 2); i++)
    {
      BG_PALETTE[i]     = ((uint16_t *)font8x8_pal)[i];                    // BG palette to   0x05000000
      SPRITE_PALETTE[i] = ((uint16_t *)font8x8_pal)[i];                    // Font palette to 0x05000200
    }
    // Set our text font
    // NOTE: We need to copy whole words per cycle, a byte gives strange results!
    for(unsigned int i(0); i < (font8x8_size / 2); i++)                    // Font to         0x06000000
      reinterpret_cast<uint16_t *>(CHAR_BASE_BLOCK(0))[i] = ((uint16_t *)font8x8)[i];

    pVideo_ = reinterpret_cast<uint16_t *>(SCREEN_BASE_BLOCK(31));         // Map at          0x0600f800
  }
#ifdef NDS9
  else if(screen == BOTTOM_SCREEN)
  {
    REG_SUB_BG0CNT  = BG_256_COLOR | BG_TILE_BASE(0) | BG_MAP_BASE(31);    // Tiles at 0x06200000, Map at 0x0620f800
    REG_SUB_DISPCNT = MODE_0 | BG0_ENABLE | SPRITE_ENABLE | DISP_SOURCE_ENGINE;
    REG_POWCNT |= POWER_LCD | POWER_2D_BOTTOM;
    REG_VRAM_C_CR = VRAM_ENABLE | 4;                                       // 128KiB @ 0x06200000 (EngineB)

    // Set the background and object palette
    // NOTE: We need to copy whole words per cycle, a byte gives strange results!
    for(unsigned int i(0); i < (font8x8_pal_size / 2); i++)
    {
      BG_PALETTE_SUB[i]     = ((uint16_t *)font8x8_pal)[i];                // BG palette to   0x05000400
      SPRITE_PALETTE_SUB[i] = ((uint16_t *)font8x8_pal)[i];                // Font palette to 0x05000600
    }
    // Set our text font
    // NOTE: We need to copy whole words per cycle, a byte gives strange results!
    for(unsigned int i(0); i < font8x8_size/2; i++)                        // Font to         0x06200000
      reinterpret_cast<uint16_t *>(CHAR_BASE_BLOCK_SUB(0))[i] = ((uint16_t *)font8x8)[i];

    pVideo_ = reinterpret_cast<uint16_t *>(SCREEN_BASE_BLOCK_SUB(31));     // Map at          0x0620f800
  }
#endif

  this->cls();

  return 0;
}

// -----------------------------------------------------------------------------
void
CGBADebugScreen::put(int x, int y, char c)
{
  if(pVideo_ != NULL)
    pVideo_[y * 32 + x] = c;
}

// -----------------------------------------------------------------------------
char
CGBADebugScreen::get(int x, int y)
{
  if(pVideo_ != NULL)
    return pVideo_[y * 32 + x];
  else
    return -1;
}
