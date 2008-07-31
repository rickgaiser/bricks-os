#include "keyboard.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/arch/registers.h"


// -----------------------------------------------------------------------------
CGBAKeyboard::CGBAKeyboard()
 : CAFileIOBufferedRead()
 , iKeys_(0x0000)
#ifdef NDS7
 , iKeysXY_(0x0000 | KEY_LID) // Lid is opened by default
#endif
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CGBAKeyboard::~CGBAKeyboard()
{
}

// -----------------------------------------------------------------------------
int
CGBAKeyboard::init()
{
/*
  // NOTE: Interrupt ONLY come when a button is PRESSED!
  // NOTE: Interrupt is intended for waking up from (the low power) stop mode
  // NOTE: Use VBlank instead
  // Attach to keypad IRQ
  CInterruptManager::attach(12, this);
  // Enable interrupt from all keys
  REG_KEYCNT = KEY_CNT_OR
             | KEY_CNT_IRQ
             | KEY_A
             | KEY_B
             | KEY_SELECT
             | KEY_START
             | KEY_RIGHT
             | KEY_LEFT
             | KEY_UP
             | KEY_DOWN
             | KEY_R
             | KEY_L;
*/

  // Attach to VBlank IRQ
  CInterruptManager::attach(0, this);
  // Enable VBlank interrupt
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;

  return 0;
}

// -----------------------------------------------------------------------------
int
CGBAKeyboard::isr(int irq)
{
  uint16_t iNewKeys       = ~REG_KEYS;
  uint16_t iChangedKeys   = iNewKeys   ^ iKeys_;
  uint16_t iChangedUp     = iNewKeys   & iChangedKeys;
#ifdef NDS7
  uint16_t iNewKeysXY     = ~REG_KEYSXY;
  uint16_t iChangedKeysXY = iNewKeysXY ^ iKeysXY_;
  uint16_t iChangedUpXY   = iNewKeysXY & iChangedKeysXY;
#endif

  if(iChangedUp & KEY_A)
    bufferRead_.put('A');
  if(iChangedUp & KEY_B)
    bufferRead_.put('B');
  if(iChangedUp & KEY_SELECT)
    bufferRead_.put('S');
  if(iChangedUp & KEY_START)
    bufferRead_.put('S');
  if(iChangedUp & KEY_RIGHT)
    bufferRead_.put('R');
  if(iChangedUp & KEY_LEFT)
    bufferRead_.put('L');
  if(iChangedUp & KEY_UP)
    bufferRead_.put('U');
  if(iChangedUp & KEY_DOWN)
    bufferRead_.put('D');
  if(iChangedUp & KEY_R)
    bufferRead_.put('>');
  if(iChangedUp & KEY_L)
    bufferRead_.put('<');
#ifdef NDS7
  if(iChangedUpXY & KEY_X)
    bufferRead_.put('X');
  if(iChangedUpXY & KEY_Y)
    bufferRead_.put('Y');
  if(iChangedUpXY & KEY_TOUCH)
    buffer_.put('T');
  if(iChangedUpXY & KEY_LID)
    buffer_.put('L');
#endif

  bufferRead_.notifyGetters();

  iKeys_   = iNewKeys;
#ifdef NDS7
  iKeysXY_ = iNewKeysXY;
#endif

  return 0;
}
