#include "dsKeyboard.h"
#include "kernel/interruptManager.h"
#include "nds/arm9/input.h"
#include "nds/registers_alt.h"
  

// These shouldn't be here
#ifdef ARM7
#define REG_KEYXY    (*(vu16*)0x04000136)
#endif
#define KEY_CNT_IRQ  (1<<14)
#define KEY_CNT_OR   (0<<15)
#define KEY_CNT_AND  (1<<15)
  

// -----------------------------------------------------------------------------
CDSKeyboard::CDSKeyboard()
 : iBufferCount_(0)
 , iKey_('~')
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CDSKeyboard::~CDSKeyboard()
{
}

// -----------------------------------------------------------------------------
int
CDSKeyboard::init()
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

  CInterruptManager::instance()->attach(12, this);

  return(0);
}

// -----------------------------------------------------------------------------
int
CDSKeyboard::isr(int irq)
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
#ifdef ARM7
  if(~REG_KEYXY & (1<<0))
    iKey_ = 'X';
  if(~REG_KEYXY & (1<<1))
    iKey_ = 'Y';
#endif

  iBufferCount_ = 1;

  return(0);
}

// -----------------------------------------------------------------------------
int
CDSKeyboard::read(void * data, size_t size)
{
  int    iRetVal(0);
  char * string((char *)data);

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
  else
  {
    iRetVal = -1;
  }
  
  return(iRetVal);
}
