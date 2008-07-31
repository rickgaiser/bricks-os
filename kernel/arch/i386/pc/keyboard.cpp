#include "keyboard.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/hal.h"


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
CI8042Keyboard::CI8042Keyboard(C8042 & driver)
 : CAFileIOBufferedRead()
 , driver_     (driver)
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
  int iRetVal(0);

  if(driver_.registerHandler(0, this) == false)
    iRetVal = -1;

  return iRetVal;
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
          {
            if(bDown == true)
            {
              bufferRead_.put(E0_keys[scancode]);
              bufferRead_.notifyGetters();
            }
          }
        }
        else if(bCapsLock_ != bShift_)
        {
          if(shift_keys[scancode] != 0)
          {
            if(bDown == true)
            {
              bufferRead_.put(shift_keys[scancode]);
              bufferRead_.notifyGetters();
            }
          }
        }
        else
        {
          if(normal_keys[scancode] != 0)
          {
            if(bDown == true)
            {
              bufferRead_.put(normal_keys[scancode]);
              bufferRead_.notifyGetters();
            }
          }
        }
      }
    }
  }
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
