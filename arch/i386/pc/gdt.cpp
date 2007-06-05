#include "gdt.h"
#include "hal.h"
#include "string.h"


selector_t selNull    (0x00);
selector_t selCodeSys (0x08 | 0);
selector_t selDataSys (0x10 | 0);
selector_t selCodeUser(0x18 | 3);
selector_t selDataUser(0x20 | 3);


// -----------------------------------------------------------------------------
CGDT::CGDT()
{
}

// -----------------------------------------------------------------------------
CGDT::~CGDT()
{
}

// -----------------------------------------------------------------------------
int
CGDT::init()
{
  memset(gd_, 0, sizeof(SGlobalDescriptor) * MAX_GLOBAL_DESCRIPTORS);

  //selNull     = createSegment(stNull, 0, 0,          0);
  selCodeSys  = createSegment(stCode, 0, 0, 0xffffffff);
  selDataSys  = createSegment(stData, 0, 0, 0xffffffff);
  selCodeUser = createSegment(stCode, 3, 0, 0xffffffff);
  selDataUser = createSegment(stData, 3, 0, 0xffffffff);

  // setting up the GDTR register
  gdtr_.base  = (uint32_t)&gd_[0];
  gdtr_.limit = (sizeof(SGlobalDescriptor) * MAX_GLOBAL_DESCRIPTORS) - 1;
  setGDTR(&gdtr_);
  
  write_ds(selDataSys);
  write_ss(selDataSys);
  write_es(selDataSys);
  write_fs(selDataSys);
  write_gs(selDataSys);

  return 0;
}

// -----------------------------------------------------------------------------
selector_t
CGDT::createSegment(ESegmentType type, unsigned int privilege, uint32_t base, uint32_t limit)
{
  int iIndex;
  uint8_t access;
  uint8_t attribs;
  
  // Validate parameters
  if(privilege > 3)
    return 0;

  // Find unused entry
  for(iIndex = 1; iIndex < MAX_GLOBAL_DESCRIPTORS; iIndex++)
    if(gd_[iIndex].access == 0)
      break;
  if(gd_[iIndex].access != 0)
    return 0;
  
  switch(type)
  {
    case stCode:
      access  = SD_PRESENT | (privilege << 5) | SD_CODE_DATA | CODE;
      attribs = 0x40; // 32 bit code
      break;
    case stData:
      access  = SD_PRESENT | (privilege << 5) | SD_CODE_DATA | DATA;
      attribs = 0x40;
      break;
    case stStack:
      access  = SD_PRESENT | (privilege << 5) | SD_CODE_DATA | STACK;
      attribs = 0x00;
      break;
    case stTSS:
      access  = SD_PRESENT | (privilege << 5) | SD_SYSTEM    | TYPE_TSS;
      attribs = 0x00;
      break;
    default:
      return 0;
  };
  
  // Check for large segments
  if(limit > 0xfffff)
  {
    attribs |= 0x80;
    limit >>= 12;
  }
  
  gd_[iIndex].limit   = limit;
  gd_[iIndex].base_l  = base;
  gd_[iIndex].base_m  = base >> 16;
  gd_[iIndex].access  = access;
  gd_[iIndex].attribs = (attribs & 0xF0) | ((limit >> 16) & 0x0F);
  gd_[iIndex].base_h  = base >> 24;
  
  return ((iIndex * 8) | privilege);
}

// -----------------------------------------------------------------------------
//selector_t
//createGate(ESegmentType type, unsigned int privilege, selector_t selector, uint32_t offset, unsigned int wordCount)
//{
//}
