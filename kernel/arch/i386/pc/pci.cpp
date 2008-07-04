#include "pci.h"
#include "asm/hal.h"
#include "kernel/debug.h"


const char * sPCIClasses[] =
{
  "Unknown device class", // Devices without class code (before PCI 2.0)
  "Mass storage controller",
  "Network controller",
  "Display controller",
  "Multimedia device",
  "Memory controller",
  "Bridge device",
  "Simple communications controller",
  "Base system peripheral",
  "Input device",
  "Docking station",
  "Processor",
  "Serial bus controller",
//  {0x0d-0xfe, "Reserved
//  {0xff, "Misc
};


// -----------------------------------------------------------------------------
uint16_t
pciConfigReadWord(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
  uint32_t  address;
  uint32_t  lbus  = (uint32_t)bus;
  uint32_t  lslot = (uint32_t)slot;
  uint32_t  lfunc = (uint32_t)func;
  uint16_t tmp   = 0;

  // create configuration address
  address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

  // write out the address
  outd(address, 0xcf8);
  // read in the data
  tmp = (uint16_t)((ind(0xcfc) >> ((offset & 2) * 8)) & 0xffff);

  return tmp;
}

// -----------------------------------------------------------------------------
uint16_t
pciGetVendor(uint16_t bus, uint16_t slot)
{
  return pciConfigReadWord(bus, slot, 0, 0);
}

// -----------------------------------------------------------------------------
uint16_t
pciGetDevice(uint16_t bus, uint16_t slot)
{
  return pciConfigReadWord(bus, slot, 0, 2);
}

// -----------------------------------------------------------------------------
void
init_pci()
{
  printk("PCI bus scan:\n");

  for(uint16_t bus(0); bus < 256; bus++)
  {
    printk(" - Scanning bus %d\r");

    for(uint16_t slot(0); slot < 32; slot++)
    {
      uint16_t vendor = pciGetVendor(bus, slot);

      if(vendor != 0xffff)
      {
        uint16_t device = pciGetDevice(bus, slot);
        uint16_t dclass = pciConfigReadWord(bus, slot, 0, 10);
        int      devclass = dclass >> 8;
//        int      subclass = dclass & 0xff;
        const char * sClass;

        if((devclass >= 0) && (devclass <= 0x0c))
          sClass = sPCIClasses[devclass];
        else
          sClass = sPCIClasses[0];

        printk(" - %d:%d - 0x%x-0x%x - %s(%d)\n", bus, slot, vendor, device, sClass, devclass);
      }
    }
  }

  printk(" - Done            \n");
}
