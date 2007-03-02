#include "gbaKeyboard.h"
#include "kernel/interruptManager.h"
#include "asm/arch/registers.h"
#include "iostream"


// -----------------------------------------------------------------------------
CGBAKeyboard::CGBAKeyboard()
 : iBufferCount_(0)
 , iKey_('~')
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

  CInterruptManager::attach(12, this);

  return 0;
}

// -----------------------------------------------------------------------------
int
CGBAKeyboard::isr(int irq)
{
  if(~REG_KEYS & KEY_A)
    iKey_ = 'A';
  if(~REG_KEYS & KEY_B)
    iKey_ = 'B';
  if(~REG_KEYS & KEY_SELECT)
    iKey_ = 'S';
  if(~REG_KEYS & KEY_START)
    iKey_ = 'S';
  if(~REG_KEYS & KEY_RIGHT)
    iKey_ = 'R';
  if(~REG_KEYS & KEY_LEFT)
    iKey_ = 'L';
  if(~REG_KEYS & KEY_UP)
    iKey_ = 'U';
  if(~REG_KEYS & KEY_DOWN)
    iKey_ = 'D';
  if(~REG_KEYS & KEY_R)
    iKey_ = '>';
  if(~REG_KEYS & KEY_L)
    iKey_ = '<';

  iBufferCount_ = 1;

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CGBAKeyboard::read(void * buffer, size_t size, loff_t *)
{
  int    iRetVal(-1);
  char * string = static_cast<char *>(buffer);

  if(size >= 2)
  {
    // Wait for key
    while(iBufferCount_ == 0){}

    // Copy key
    string[0] = iKey_;
    string[1] = 0;
    iRetVal   = 2;

    // Allow next key
    iBufferCount_ = 0;
  }

  return iRetVal;
}
