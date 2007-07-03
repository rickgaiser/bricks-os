#include "descriptor.h"
#include "isrWrapper.h"
#include "string.h"


CDescriptorTable cGDT;
CDescriptorTable cIDT;

selector_t selNull       (0x00);
selector_t selCodeKernel (0x08 | 0);
selector_t selDataKernel (0x10 | 0);
selector_t selCodeUser   (0x18 | 3);
selector_t selDataUser   (0x20 | 3);
selector_t selCodeUserTmp(0x28 | 3);  // Temporary full range selector for testing user space
selector_t selDataUserTmp(0x30 | 3);  // Temporary full range selector for testing user space


// -----------------------------------------------------------------------------
void
init_gdt(SDescriptor * desc, unsigned int count)
{
  cGDT.init(desc, count, dttGlobal);

  selNull        = cGDT.createSegment(dtDataR,  0, 0x00000000, 0x00000000);
  selCodeKernel  = cGDT.createSegment(dtCodeR,  0, 0x00000000, 0xffffffff);  // Start: 0GiB, Size: 4GiB
  selDataKernel  = cGDT.createSegment(dtDataRW, 0, 0x00000000, 0xffffffff);  // Start: 0GiB, Size: 4GiB
  selCodeUser    = cGDT.createSegment(dtCodeR,  3, 0x00000000, 0xbfffffff);  // Start: 0GiB, Size: 3GiB
  selDataUser    = cGDT.createSegment(dtDataRW, 3, 0x00000000, 0xbfffffff);  // Start: 0GiB, Size: 3GiB
  selCodeUserTmp = cGDT.createSegment(dtCodeR,  3, 0x00000000, 0xffffffff);  // Start: 0GiB, Size: 4GiB
  selDataUserTmp = cGDT.createSegment(dtDataRW, 3, 0x00000000, 0xffffffff);  // Start: 0GiB, Size: 4GiB

  // Set the GDTR
  cGDT.dtr_.base  = (uint32_t)&cGDT.desc_[0];
  cGDT.dtr_.limit = (sizeof(SDescriptor) * cGDT.iCount_) - 1;
  setGDTR(&cGDT.dtr_);
  
  // Reload the segment registers with newly created segments
  setDS(selDataKernel);
  setSS(selDataKernel);
  setES(selDataKernel);
  setFS(selDataKernel);
  setGS(selDataKernel);
}

// -----------------------------------------------------------------------------
void
init_idt(SDescriptor * desc, unsigned int count)
{
  cIDT.init(desc, count, dttInterrupt);

  // Processor generated interrupts
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x00], 0, selCodeKernel, (unsigned long)&isr_0x00_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x01], 0, selCodeKernel, (unsigned long)&isr_0x01_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x02], 0, selCodeKernel, (unsigned long)&isr_0x02_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x03], 0, selCodeKernel, (unsigned long)&isr_0x03_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x04], 0, selCodeKernel, (unsigned long)&isr_0x04_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x05], 0, selCodeKernel, (unsigned long)&isr_0x05_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x06], 0, selCodeKernel, (unsigned long)&isr_0x06_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x07], 0, selCodeKernel, (unsigned long)&isr_0x07_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x08], 0, selCodeKernel, (unsigned long)&isr_0x08_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x09], 0, selCodeKernel, (unsigned long)&isr_0x09_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x0A], 0, selCodeKernel, (unsigned long)&isr_0x0a_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x0B], 0, selCodeKernel, (unsigned long)&isr_0x0b_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x0C], 0, selCodeKernel, (unsigned long)&isr_0x0c_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x0D], 0, selCodeKernel, (unsigned long)&isr_0x0d_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x0E], 0, selCodeKernel, (unsigned long)&isr_0x0e_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x0F], 0, selCodeKernel, (unsigned long)&isr_0x0f_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x10], 0, selCodeKernel, (unsigned long)&isr_0x10_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x11], 0, selCodeKernel, (unsigned long)&isr_0x11_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x12], 0, selCodeKernel, (unsigned long)&isr_0x12_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x13], 0, selCodeKernel, (unsigned long)&isr_0x13_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x14], 0, selCodeKernel, (unsigned long)&isr_0x14_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x15], 0, selCodeKernel, (unsigned long)&isr_0x15_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x16], 0, selCodeKernel, (unsigned long)&isr_0x16_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x17], 0, selCodeKernel, (unsigned long)&isr_0x17_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x18], 0, selCodeKernel, (unsigned long)&isr_0x18_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x19], 0, selCodeKernel, (unsigned long)&isr_0x19_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x1A], 0, selCodeKernel, (unsigned long)&isr_0x1a_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x1B], 0, selCodeKernel, (unsigned long)&isr_0x1b_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x1C], 0, selCodeKernel, (unsigned long)&isr_0x1c_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x1D], 0, selCodeKernel, (unsigned long)&isr_0x1d_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x1E], 0, selCodeKernel, (unsigned long)&isr_0x1e_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x1F], 0, selCodeKernel, (unsigned long)&isr_0x1f_wrapper);
  // PIC generated interrupts
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x20], 0, selCodeKernel, (unsigned long)&isr_0x20_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x21], 0, selCodeKernel, (unsigned long)&isr_0x21_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x22], 0, selCodeKernel, (unsigned long)&isr_0x22_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x23], 0, selCodeKernel, (unsigned long)&isr_0x23_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x24], 0, selCodeKernel, (unsigned long)&isr_0x24_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x25], 0, selCodeKernel, (unsigned long)&isr_0x25_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x26], 0, selCodeKernel, (unsigned long)&isr_0x26_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x27], 0, selCodeKernel, (unsigned long)&isr_0x27_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x28], 0, selCodeKernel, (unsigned long)&isr_0x28_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x29], 0, selCodeKernel, (unsigned long)&isr_0x29_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x2A], 0, selCodeKernel, (unsigned long)&isr_0x2a_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x2B], 0, selCodeKernel, (unsigned long)&isr_0x2b_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x2C], 0, selCodeKernel, (unsigned long)&isr_0x2c_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x2D], 0, selCodeKernel, (unsigned long)&isr_0x2d_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x2E], 0, selCodeKernel, (unsigned long)&isr_0x2e_wrapper);
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x2F], 0, selCodeKernel, (unsigned long)&isr_0x2f_wrapper);
  // Software generated interrupts
  CDescriptorTable::createInterruptGate(cIDT.desc_[0x30], 3, selCodeKernel, (unsigned long)&isr_0x30_wrapper);
  for(unsigned int i(0x31); i < cIDT.iCount_; i++)
    CDescriptorTable::createInterruptGate(cIDT.desc_[i],  3, selCodeKernel, (unsigned long)&isr_0x80_wrapper);

  // Set the IDTR
  cIDT.dtr_.base  = (unsigned long)&cIDT.desc_[0];
  cIDT.dtr_.limit = (sizeof(SDescriptor) * cIDT.iCount_) - 1;
  setIDTR(&cIDT.dtr_);
}

// -----------------------------------------------------------------------------
// Find unused entry
#define NEW_ENTRY_RETURN(retval)                              \
  unsigned int iIndex;                                        \
  for(iIndex = 0; iIndex < iCount_; iIndex++)                 \
    if(descriptorPresent(desc_[iIndex]) == false)             \
      break;                                                  \
  if(descriptorPresent(desc_[iIndex]) == true)                \
    return retval;                                            \

// -----------------------------------------------------------------------------
CDescriptorTable::CDescriptorTable()
 : desc_(0)
 , iCount_(0)
{
}

// -----------------------------------------------------------------------------
CDescriptorTable::~CDescriptorTable()
{
}

// -----------------------------------------------------------------------------
void
CDescriptorTable::init(SDescriptor * desc, unsigned int count, EDescriptorTableType type)
{
  desc_   = desc;
  eType_  = type;
  iCount_ = count;

  // Check size of table
  switch(eType_)
  {
    case dttInterrupt: if(iCount_ > 256) iCount_ = 256; break;
    case dttGlobal:    // Fall through, gdt and ldt have the same max. size
    case dttLocal:     if(iCount_ > 8192) iCount_ = 8192; break;
  };
  
  // Clear entire table (also clears the present flag)
  memset(desc_, 0, sizeof(SDescriptor) * iCount_);
}

// -----------------------------------------------------------------------------
selector_t
CDescriptorTable::createSegment(EDescriptorType type, unsigned int privilege, uint32_t base, uint32_t limit)
{
  uint8_t access(DESC_PRESENT | (privilege << 5) | type);
  uint8_t attribs(0x40);
  
  // Validate parameters
  if(privilege > 3)
    return 0;

  // Get index for new descriptor entry
  NEW_ENTRY_RETURN(0);
  
  // Check for large segments
  if(limit > 0xfffff)
  {
    attribs |= 0x80;
    limit >>= 12;
  }
  
  desc_[iIndex].limit   = limit;
  desc_[iIndex].base_l  = base;
  desc_[iIndex].base_m  = base >> 16;
  desc_[iIndex].access  = access;
  desc_[iIndex].attribs = (attribs & 0xF0) | ((limit >> 16) & 0x0F);
  desc_[iIndex].base_h  = base >> 24;
  
  return ((iIndex * 8) | privilege);
}

// -----------------------------------------------------------------------------
selector_t
CDescriptorTable::createCallGate(unsigned int privilege, selector_t selector, unsigned int params)
{
  // Validate parameters
  if(privilege > 3)
    return 0;

  // Get index for new descriptor entry
  NEW_ENTRY_RETURN(0);
  
  CDescriptorTable::createCallGate(desc_[iIndex], privilege, selector, params);
  
  return ((iIndex * 8) | privilege);
}

// -----------------------------------------------------------------------------
selector_t
CDescriptorTable::createTaskGate(unsigned int privilege, selector_t selector)
{
  // Validate parameters
  if(privilege > 3)
    return 0;

  // Get index for new descriptor entry
  NEW_ENTRY_RETURN(0);

  CDescriptorTable::createTaskGate(desc_[iIndex], privilege, selector);
  
  return ((iIndex * 8) | privilege);
}

// -----------------------------------------------------------------------------
selector_t
CDescriptorTable::createInterruptGate(unsigned int privilege, selector_t selector, uint32_t offset)
{
  // Validate parameters
  if(privilege > 3)
    return 0;

  // Get index for new descriptor entry
  NEW_ENTRY_RETURN(0);

  CDescriptorTable::createInterruptGate(desc_[iIndex], privilege, selector, offset);
  
  return ((iIndex * 8) | privilege);
}

// -----------------------------------------------------------------------------
selector_t
CDescriptorTable::createTrapGate(unsigned int privilege, selector_t selector, uint32_t offset)
{
  // Validate parameters
  if(privilege > 3)
    return 0;

  // Get index for new descriptor entry
  NEW_ENTRY_RETURN(0);

  CDescriptorTable::createTrapGate(desc_[iIndex], privilege, selector, offset);
  
  return ((iIndex * 8) | privilege);
}

// -----------------------------------------------------------------------------
void
CDescriptorTable::createCallGate(SDescriptor & desc, unsigned int privilege, selector_t selector, unsigned int params)
{
  desc.offset_l  = 0;
  desc.selector  = selector;
  desc.param_cnt = params;  // Copy 0..31 parameters to called stack (32 bit parameters)
  desc.access    = DESC_PRESENT | (privilege << 5) | dtCallGate;
  desc.offset_h  = 0;
}

// -----------------------------------------------------------------------------
void
CDescriptorTable::createTaskGate(SDescriptor & desc, unsigned int privilege, selector_t selector)
{
  desc.offset_l  = 0;
  desc.selector  = selector;
  desc.param_cnt = 0;
  desc.access    = DESC_PRESENT | (privilege << 5) | dtTaskGate;
  desc.offset_h  = 0;
}

// -----------------------------------------------------------------------------
void
CDescriptorTable::createInterruptGate(SDescriptor & desc, unsigned int privilege, selector_t selector, uint32_t offset)
{
  desc.offset_l  = offset;
  desc.selector  = selector;
  desc.param_cnt = 0;
  desc.access    = DESC_PRESENT | (privilege << 5) | dtInterruptGate;
  desc.offset_h  = offset >> 16;
}

// -----------------------------------------------------------------------------
void
CDescriptorTable::createTrapGate(SDescriptor & desc, unsigned int privilege, selector_t selector, uint32_t offset)
{
  desc.offset_l  = offset;
  desc.selector  = selector;
  desc.param_cnt = 0;
  desc.access    = DESC_PRESENT | (privilege << 5) | dtTrapGate;
  desc.offset_h  = offset >> 16;
}
