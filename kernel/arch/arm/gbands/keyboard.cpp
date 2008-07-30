#include "keyboard.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/arch/registers.h"


// -----------------------------------------------------------------------------
CGBAKeyboard::CGBAKeyboard()
 : buffer_()
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
    buffer_.put('A');
  if(iChangedUp & KEY_B)
    buffer_.put('B');
  if(iChangedUp & KEY_SELECT)
    buffer_.put('S');
  if(iChangedUp & KEY_START)
    buffer_.put('S');
  if(iChangedUp & KEY_RIGHT)
    buffer_.put('R');
  if(iChangedUp & KEY_LEFT)
    buffer_.put('L');
  if(iChangedUp & KEY_UP)
    buffer_.put('U');
  if(iChangedUp & KEY_DOWN)
    buffer_.put('D');
  if(iChangedUp & KEY_R)
    buffer_.put('>');
  if(iChangedUp & KEY_L)
    buffer_.put('<');
#ifdef NDS7
  if(iChangedUpXY & KEY_X)
    buffer_.put('X');
  if(iChangedUpXY & KEY_Y)
    buffer_.put('Y');
  if(iChangedUpXY & KEY_TOUCH)
    buffer_.put('T');
  if(iChangedUpXY & KEY_LID)
    buffer_.put('L');
#endif

  buffer_.notifyGetters();

  iKeys_   = iNewKeys;
#ifdef NDS7
  iKeysXY_ = iNewKeysXY;
#endif

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CGBAKeyboard::read(void * buffer, size_t size, bool block)
{
  int iRetVal(0);
  uint8_t * data((uint8_t *)buffer);

  for(size_t i(0); i < size; i++)
  {
    if((i == 0) && (block == true))
      buffer_.get(data, true);
    else if(buffer_.get(data, false) == false)
      break;
    data++;
    iRetVal++;
  }

  return iRetVal;
}
