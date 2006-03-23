#include "gdt.h"
#include "hal.h"
#include "string.h"


uint16_t selNull(0x00), selCodeSys(0x08), selDataSys(0x10);


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

  //selNull    = newGD(0,       0,    0,    0);  // 0x00
  selCodeSys = newGD(0, 0xFFFFF, CODE, 0xC0);  // 0x08
  selDataSys = newGD(0, 0xFFFFF, DATA, 0xC0);  // 0x10

  // setting up the GDTR register
  gdtr_.base  = (uint32_t)&gd_[0];
  gdtr_.limit = (sizeof(SGlobalDescriptor) * MAX_GLOBAL_DESCRIPTORS) - 1;
  load_gdt(&gdtr_);
  
  write_ds(selDataSys);
  write_ss(selDataSys);
  write_es(selDataSys);
  write_fs(selDataSys);
  write_gs(selDataSys);

  return(0);
}

// -----------------------------------------------------------------------------
uint16_t
CGDT::newGD(uint32_t base,
            uint32_t limit,  
            uint8_t  access,
            uint8_t  attribs)
{
  int i;
  
  for(i = 1; i < MAX_GLOBAL_DESCRIPTORS; i++)
    if(gd_[i].access == 0)
      break;
  if(gd_[i].access != 0)
    return 0;
  
  gd_[i].limit   = limit;
  gd_[i].base_l  = base;
  gd_[i].base_m  = base >> 16;
  gd_[i].access  = access;
  gd_[i].attribs = (attribs & 0xF0) | ((limit >> 16) & 0x0F);
  gd_[i].base_h  = base >> 24;
  
  return(i * 8);
}
