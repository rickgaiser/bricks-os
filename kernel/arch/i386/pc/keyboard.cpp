#include "keyboard.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "hal.h"


char normal_keys[128] =
{
   0 , 0 ,'1','2','3','4','5','6',  //   0-  7
  '7','8','9','0','-','=', 8 , 0 ,  //   8- 15
  'q','w','e','r','t','y','u','i',  //  16- 23
  'o','p','[',']',0x0a,0 ,'a','s',  //  24- 31
  'd','f','g','h','j','k','l',';',  //  32- 39
  '\'','`',0,'\\','z','x','c','v',  //  40- 47
  'b','n','m',',','.','/', 0 ,'*',  //  48- 55
   0 ,' ', 0 , 0 , 0 , 0 , 0 , 0 ,  //  56- 63
   0 , 0 , 0 , 0 , 0 , 0 , 0 ,'7',  //  64- 71
  '8','9','-','4','5','6','+','1',  //  72- 79
  '2','3','0','.', 0 , 0 , 0 , 0 ,  //  80- 87
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  //  88- 95
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  //  96-103
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  // 104-111
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  // 112-119
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0    // 120-127
};

char shift_keys[128] =
{
   0 , 0 ,'!','@','#','$','%','^',  //   0-  7
  '&','*','(',')','_','+', 0 , 0 ,  //   8- 15
  'Q','W','E','R','T','Y','U','I',  //  16- 23
  'O','P','{','}',0x0a,0 ,'A','S',  //  24- 31
  'D','F','G','H','J','K','L',':',  //  32- 39
  '"','~', 0 ,'|','Z','X','C','V',  //  40- 47
  'B','N','M','<','>','?', 0 ,'*',  //  48- 55
   0 ,' ', 0 , 0 , 0 , 0 , 0 , 0 ,  //  56- 63
   0 , 0 , 0 , 0 , 0 , 0 , 0 ,'7',  //  64- 71
  '8','9','-','4','5','6','+','1',  //  72- 79
  '2','3','0','.', 0 , 0 , 0 , 0 ,  //  80- 87
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  //  88- 95
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  //  96-103
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  // 104-111
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  // 112-119
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0    // 120-127
};

char E0_keys[128] =
{
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 ,0x0a,0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 ,'/', 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
};


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CRingBuffer::CRingBuffer()
 : size_(128)
 , inPtr_(0)
 , outPtr_(0)
{
}

// -----------------------------------------------------------------------------
CRingBuffer::~CRingBuffer()
{
}

// -----------------------------------------------------------------------------
bool
CRingBuffer::put(uint8_t data)
{
  //printk("%c", (char)data);

  // Try to increment in ptr
  uint32_t in = inPtr_ + 1;
  if(in >= size_)
    in = 0;

  // Buffer full
  if(in == outPtr_)
    return false;

  // Place data and increment pointer
  buffer_[inPtr_] = data;
  inPtr_ = in;

  return true;
}

// -----------------------------------------------------------------------------
bool
CRingBuffer::get(uint8_t * data)
{
  // Buffer empty
  if(inPtr_ == outPtr_)
    return false;

  // Get data and increment pointer
  *data = buffer_[outPtr_++];
  if(outPtr_ >= size_)
    outPtr_ = 0;

  return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CI8042Keyboard::CI8042Keyboard(C8042 & driver)
 : driver_     (driver)
 , buffer_     ()
 , bE0_        (false)
 , bShift_     (false)
 , bCtrl_      (false)
 , bAlt_       (false)
 , bCapsLock_  (false)
 , bNumLock_   (false)
 , bScrollLock_(false)
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CI8042Keyboard::~CI8042Keyboard()
{
}

// -----------------------------------------------------------------------------
int
CI8042Keyboard::init()
{
  driver_.registerHandler(0, this);

  return 0;
}

// -----------------------------------------------------------------------------
void
CI8042Keyboard::i8042_callBack(uint8_t scancode)
{
  switch(scancode)
  {
    case 0x00:
    case 0xaa:
    case 0xab:
    case 0xee:
    case 0xfa:
    case 0xfc:
    case 0xfe:
    case 0xff:
      break;
    case 0xe0:
      bE0_ = true;
      break;
    default:
    {
      // Up or down?
      bool bDown = !(scancode & 0x80);
      scancode &= ~0x80;

      // State keys: shift/ctrl/alt/caps-/num-/scroll-lock
      switch(scancode)
      {
      case 0x2a: // Shift
      case 0x36: // Shift
        bShift_ = bDown;
        break;
      case 0x1d: // Ctrl
        bCtrl_ = bDown;
        break;
      case 0x38: // Alt
        bAlt_ = bDown;
        break;
      case 0x3a: // Caps Lock
        if(bDown == true)
        {
          bCapsLock_ = !bCapsLock_;
          updateLeds();
        }
        break;
      case 0x45: // Num Lock
        if(bDown == true)
        {
          bNumLock_ = !bNumLock_;
          updateLeds();
        }
        break;
      case 0x46: // Scroll Lock
        if(bDown == true)
        {
          bScrollLock_ = !bScrollLock_;
          updateLeds();
        }
        break;
      default:
        if(bE0_ == true)
        {
          bE0_ = false;
          if(E0_keys[scancode] != 0)
            buffer_.put(E0_keys[scancode]);
        }
        else if(bCapsLock_ != bShift_)
        {
          if(shift_keys[scancode] != 0)
            buffer_.put(shift_keys[scancode]);
        }
        else
        {
          if(normal_keys[scancode] != 0)
            buffer_.put(normal_keys[scancode]);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
int
CI8042Keyboard::read(void * data, size_t size)
{
  int iRetVal(0);
  uint8_t * pData = (uint8_t *)data;

  while(size--)
  {
    uint8_t value;

    if(buffer_.get(&value) == false)
      break;

    *pData++ = value;
    iRetVal++;
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
void
CI8042Keyboard::updateLeds()
{
  uint8_t iLeds;

  iLeds =  (bCapsLock_ << 2) | (bNumLock_ << 1) | (bScrollLock_);

  driver_.writeData(0, KBD_LEDS); // "set LEDs" command
  driver_.writeData(0, iLeds);    // bottom 3 bits set LEDs
}
