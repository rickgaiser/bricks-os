#include "i386Keyboard.h"
#include "kernel/interruptManager.h"
#include "hal.h"
#include "iostream"
  

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
CI386Keyboard::CI386Keyboard()
 : bE0_   (false)
 , bShift_(false)
 , bCtrl_ (false)
 , bAlt_  (false)
 , bCaps_ (false)
 , iLeds_ (0x00)
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CI386Keyboard::~CI386Keyboard()
{
}

// -----------------------------------------------------------------------------
int
CI386Keyboard::init()
{
  CInterruptManager::attach(0x21, this);

  updateLeds();

  return(0);
}

// -----------------------------------------------------------------------------
int
CI386Keyboard::isr(int irq)
{
  unsigned char iScanCode;
  unsigned char iAck;
  char          cChar(-1);

  // Read in the Scan Code
  iScanCode = inb(KBD_DATA_REG);
  // ACK the scancode
  iAck = inb(KBD_ACK_REG);
  ::outb(iAck | 0x80, KBD_ACK_REG);
  ::outb(iAck , KBD_ACK_REG);

  switch(iScanCode)
  {
    case 0x00:
    case 0xAA:
    case 0xAB:
    case 0xEE:
    case 0xFA:
    case 0xFC:
    case 0xFE:
    case 0xFF:
      break;
    case 0xE0:
      bE0_ = true;
      break;
    default:
    {
      // Up or down?
      if(iScanCode & 0x80)
      {
        bDown_ = false;
        iScanCode -= 0x80;
      }
      else
      {
        bDown_ = true;
      }

      // State keys: shift/ctrl/alt/caps-/num-/scroll-lock
      switch(iScanCode)
      {
      case 0x2A: // Shift
      case 0x36: // Shift
        bShift_ = bDown_;
        break;
      case 0x1D: // Ctrl
        bCtrl_ = bDown_;
        break;
      case 0x38: // Alt
        bAlt_ = bDown_;
        break;
      case 0x3A: // Caps Lock
        if(bDown_ == true)
        {
          iLeds_ ^= 0x04;
          bCaps_  = (iLeds_ & 0x04) != 0;
          updateLeds();
        }
        break;
      case 0x45: // Num Lock
        if(bDown_ == true)
        {
          iLeds_ ^= 0x02;
          updateLeds();
        }
        break;
      case 0x46: // Scroll Lock
        if(bDown_ == true)
        {
          iLeds_ ^= 0x01;
          updateLeds();
        }
        break;
      default:
        if(bE0_ == true)
        {
          bE0_ = false;
          cChar = E0_keys[iScanCode];
        }
        else if(bCaps_ != bShift_)
        {
          cChar = shift_keys[iScanCode];
        }
        else
        {
          cChar = normal_keys[iScanCode];
        }
      }
    }
  }
    
  if((bDown_ == true) && (cChar != -1))
    std::cout<<cChar;

  return 0;
}

// -----------------------------------------------------------------------------
int
CI386Keyboard::read(void * data, size_t size)
{
  while(true){}

  return(0);
}

// -----------------------------------------------------------------------------
void
CI386Keyboard::outb(unsigned char data, unsigned short addr)
{
  // loop until 8042 input buffer empty
  for(int iTimeout(0); iTimeout < 500000; iTimeout++)
  {
    unsigned char iStatus = inb(KBD_STATUS_REG);

    if((iStatus & 0x02) == 0)
    {
      ::outb(data, addr);
      break;
    }
  }
}

// -----------------------------------------------------------------------------
void
CI386Keyboard::updateLeds()
{
  this->outb(KBD_LEDS, KBD_DATA_REG); // "set LEDs" command
  this->outb(iLeds_,   KBD_DATA_REG); // bottom 3 bits set LEDs
}
