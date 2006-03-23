#include "ds9Video.h"
#include "nds.h"
#include "nds/arm9/console.h"
//#include "nds/system.h"


// -----------------------------------------------------------------------------
CDS9Video::CDS9Video()
 : CAVideo(32, 24)
 , pVideo_((unsigned short *)SCREEN_BASE_BLOCK(31))
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CDS9Video::~CDS9Video()
{
}

// -----------------------------------------------------------------------------
int
CDS9Video::init()
{
  // Power up screens
//  powerON(POWER_ALL);
  POWER_CR |= POWER_ALL_2D;

  // Use the main screen for output
//  videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
  DISPLAY_CR = MODE_0_2D | DISPLAY_BG0_ACTIVE;

//  vramSetBankA(VRAM_A_MAIN_BG);
  VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;

  BG0_CR = BG_MAP_BASE(31);

  // Set the colour of the font to White.
  BG_PALETTE[255] = RGB15(31,31,31);

  consoleInitDefault((u16*)SCREEN_BASE_BLOCK(31), (u16*)CHAR_BASE_BLOCK(0), 16);

//  this->cls();

  return(0);
}

// -----------------------------------------------------------------------------
void
CDS9Video::put(int iX, int iY, char c)
{
  pVideo_[iY * 32 + iX] = (unsigned short)(0xf000) | (unsigned short)(c);
}

// -----------------------------------------------------------------------------
char
CDS9Video::get(int iX, int iY)
{
  return(pVideo_[iY * 32 + iX] & 0xff);
}
