#include "aspace.h"
#include "mmap.h"
#include "string.h"


extern bool bPAEEnabled;


// -----------------------------------------------------------------------------
CPCAddressSpace::CPCAddressSpace()
 : bInitialized_(false)
{
}

// -----------------------------------------------------------------------------
CPCAddressSpace::~CPCAddressSpace()
{
  if(bInitialized_ == true)
  {
    if(bPAEEnabled == true)
    {
      physFreePage(PDPT_.entry[0] & 0xfffff000);
      physFreePage(PDPT_.entry[1] & 0xfffff000);
      physFreePage(PDPT_.entry[2] & 0xfffff000);
      physFreePage(PDPT_.entry[3] & 0xfffff000);
    }
    else
    {
      physFreePage((uint64_t)pPD_);
    }
  }
}

// -----------------------------------------------------------------------------
void
CPCAddressSpace::init()
{
  if(bInitialized_ == false)
  {
    if(bPAEEnabled == true)
    {
      // Locate 16KiB of phisical memory for page directory entries
      PDPT_.entry[0] = physAllocPage() | 1;
      PDPT_.entry[1] = physAllocPage() | 1;
      PDPT_.entry[2] = physAllocPage() | 1;
      PDPT_.entry[3] = physAllocPage() | 1;

      // Setup PDEs
      pde64_t * pPDE;
      pPDE = (pde64_t *)(PDPT_.entry[0] & 0xfffff000);
      memset(pPDE, 0, 4096);
      pPDE = (pde64_t *)(PDPT_.entry[1] & 0xfffff000);
      memset(pPDE, 0, 4096);
      pPDE = (pde64_t *)(PDPT_.entry[2] & 0xfffff000);
      memset(pPDE, 0, 4096);
      pPDE = (pde64_t *)(PDPT_.entry[3] & 0xfffff000);
      memset(pPDE, 0, 4096);
      // Map PDE into itself in the last entries, making it accessible in the last 8MiB of virtual memory
      pPDE[508] = PDPT_.entry[0] | PG_USER | PG_WRITABLE | PG_PRESENT;
      pPDE[509] = PDPT_.entry[1] | PG_USER | PG_WRITABLE | PG_PRESENT;
      pPDE[510] = PDPT_.entry[2] | PG_USER | PG_WRITABLE | PG_PRESENT;
      pPDE[511] = PDPT_.entry[3] | PG_USER | PG_WRITABLE | PG_PRESENT;

      iCR3_ = (uint32_t)&PDPT_;
    }
    else
    {
      // Locate 4KiB of phisical memory for page directory entries
      pPD_ = (pde32_t *)physAllocPage();
      memset((void *)pPD_, 0, 4096);
      // Map PDE into itself in the last entries, making it accessible in the last 4MiB of virtual memory
      pPD_[1023] = (uint32_t)pPD_ | PG_USER | PG_WRITABLE | PG_PRESENT;

      iCR3_ = (uint32_t)pPD_;
    }

    bInitialized_ = true;
  }
}

// -----------------------------------------------------------------------------
void
CPCAddressSpace::identityMap(void * start, uint32_t length)
{
  uint32_t iStart((unsigned int)start >> 12);
  uint32_t iCount(length >> 12);
  
  if(bPAEEnabled == true)
  {
    pde64_t * pPD;
    pte64_t * pPT;
    
    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdppidx = (idx & 0x000c0000) >> 20;
      uint32_t pdidx   = (idx & 0x0003fe00) >>  9;
      uint32_t ptidx   = (idx & 0x000001ff);

      // Get PD
      pPD = (pde64_t *)(PDPT_.entry[pdppidx] & 0xfffff000);
      
      // Create PT if not present
      if((pPD[pdidx] & PG_PRESENT) == 0)
      {
        pPT = (pte64_t *)physAllocPage();
        memset(pPT, 0, 4096);      
        pPD[pdidx] = (uint64_t)pPT | PG_USER | PG_WRITABLE | PG_PRESENT;;
      }
      
      // Get PT
      pPT = (pte64_t *)(pPD[pdidx] & 0xfffff000);
      
      // Map Page
      pPT[ptidx] = (idx << 12) | PG_USER | PG_WRITABLE | PG_PRESENT;      
    }
  }
  else
  {
    pte32_t * pPT;

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdidx = (idx & 0x000ffc00) >> 10;
      uint32_t ptidx = (idx & 0x000003ff);      

      // Create PT if not present
      if((pPD_[pdidx] & PG_PRESENT) == 0)
      {
        pPT = (pte32_t *)physAllocPage();
        memset(pPT, 0, 4096);      
        pPD_[pdidx] = (uint32_t)pPT | PG_USER | PG_WRITABLE | PG_PRESENT;;
      }
      
      // Get PT
      pPT = (pte32_t *)(pPD_[pdidx] & 0xfffff000);
      
      // Map Page
      pPT[ptidx] = (idx << 12) | PG_USER | PG_WRITABLE | PG_PRESENT;      
    }
  }
}

// -----------------------------------------------------------------------------
void
CPCAddressSpace::addSection(void * to_addr, void * from_addr, uint32_t length)
{
  uint32_t iStart(PAGE_ALIGN_DOWN_4K((unsigned int)to_addr) >> 12);
  uint32_t iCount((PAGE_ALIGN_UP_4K((unsigned int)to_addr + length) >> 12) - iStart);
  
  if(bPAEEnabled == true)
  {
    pde64_t * pPD;
    pte64_t * pPT;
    
    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdppidx = (idx & 0x000c0000) >> 20;
      uint32_t pdidx   = (idx & 0x0003fe00) >>  9;
      uint32_t ptidx   = (idx & 0x000001ff);

      // Get PD
      pPD = (pde64_t *)(PDPT_.entry[pdppidx] & 0xfffff000);
      
      // Create PT if not present
      if((pPD[pdidx] & PG_PRESENT) == 0)
      {
        pPT = (pte64_t *)physAllocPage();
        memset(pPT, 0, 4096);      
        pPD[pdidx] = (uint64_t)pPT | PG_USER | PG_WRITABLE | PG_PRESENT;;
      }
      
      // Get PT
      pPT = (pte64_t *)(pPD[pdidx] & 0xfffff000);
      
      // Map new empty page if not present
      if((pPT[ptidx] & PG_PRESENT) == 0)
      {
        pPT[ptidx] = physAllocPage() | PG_USER | PG_WRITABLE | PG_PRESENT;      
      }
    }
  }
  else
  {
    pte32_t * pPT;

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdidx = (idx & 0x000ffc00) >> 10;
      uint32_t ptidx = (idx & 0x000003ff);      

      // Create PT if not present
      if((pPD_[pdidx] & PG_PRESENT) == 0)
      {
        pPT = (pte32_t *)physAllocPage();
        memset(pPT, 0, 4096);      
        pPD_[pdidx] = (uint32_t)pPT | PG_USER | PG_WRITABLE | PG_PRESENT;;
      }
      
      // Get PT
      pPT = (pte32_t *)(pPD_[pdidx] & 0xfffff000);
      
      // Map new empty page if not present
      if((pPT[ptidx] & PG_PRESENT) == 0)
      {
        pPT[ptidx] = physAllocPage() | PG_USER | PG_WRITABLE | PG_PRESENT;      
      }
    }
  }
  
  // FIXME: We're swapping address spaces here!!!
  setCR3(this->cr3());
  memcpy(to_addr, from_addr, length);
}

// -----------------------------------------------------------------------------
// Copy a range of PD entries from an address space to this one
void
CPCAddressSpace::addRange(const CPCAddressSpace & as, void * start, unsigned int length)
{
  uint32_t iStart;
  uint32_t iCount;
  
  if(bPAEEnabled == true)
  {
    pde64_t * pPD1;
    pde64_t * pPD2;
    
    iStart = (unsigned int)start >> (12 + 9);
    iCount = length >> (12 + 9);

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdppidx = (idx & 0x00000600) >> 11;
      uint32_t pdidx   = (idx & 0x000001ff);

      // Get PD
      pPD1 = (pde64_t *)(PDPT_.entry[pdppidx] & 0xfffff000);
      pPD2 = (pde64_t *)(as.PDPT_.entry[pdppidx] & 0xfffff000);
      
      // Create PT if not present
      pPD1[pdidx] = pPD2[pdidx];
    }
  }
  else
  {
    iStart = (unsigned int)start >> (12 + 10);
    iCount = length >> (12 + 10);

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
      pPD_[idx] = as.pPD_[idx];
  }
}

// -----------------------------------------------------------------------------
uint32_t
CPCAddressSpace::cr3()
{
  return iCR3_;
}

