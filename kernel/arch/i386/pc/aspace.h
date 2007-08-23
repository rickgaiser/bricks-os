#ifndef I386_ASPACE_H
#define I386_ASPACE_H


#include "hal.h"


/****************************************************************************
 *                      Bricks-OS Physical Memory Map                       *
 ****************************************************************************
 *
 *                         +----------------------+   4 GiB
 *                         |                      |
 *                         |                      |
 *                         |                      |
 *          Free           |       free ram       |
 *                         |                      |
 *                         |                      |
 *                         |                      |
 *   ----------------------+----------------------+   4 MiB
 *                         |      kernel heap     |
 *                         +----------------------+
 *                         |     kernel modules   |
 *         Kernel          +----------------------+
 *                         |        kernel        |
 *                         +----------------------+
 *                         |     kernel stack     |
 *   ----------------------+----------------------+   1 MiB
 *                         |        legacy        |
 *                         +----------------------+ 640 KiB
 *                         |                      |
 *         Legacy          |       free ram       |
 *                         |                      |
 *                         +----------------------+   4 KiB
 *                         |        legacy        |
 *                         +----------------------+   0
 */

/****************************************************************************
 *                       Bricks-OS Virtual Memory Map                       *
 ****************************************************************************
 *
 * TODO: Kernel is currently NOT remapped to 3GiB. It still resides in the
 *       lower 4MiB.
 *
 *       A separate kernel loader would be nice:
 *        - Grub sets protected mode and loads kernel loader + kernel.
 *        - Kernel loader enables paging and loads kernel to 3GiB.
 *        - Kernel takes over control.
 *
 *                         +----------------------+   4 GiB
 *                         |      PDEs/PTEs       |
 *                         +----------------------+
 *                         |     kernel stack     |
 *         Kernel          +----------------------+
 *                         |      kernel heap     |
 *                         +----------------------+
 *                         |        kernel        |
 *   ----------------------+----------------------+   3 GiB ?
 *                         |                      |
 *                         |                      |
 *                         |                      |
 *          User           |      user space      |
 *                         |                      |
 *                         |                      |
 *                         |                      |
 *   ----------------------+----------------------+   4 MiB
 *         Legacy          |        legacy        |
 *                         +----------------------+   0
 */


class CPCAddressSpace
{
public:
  CPCAddressSpace();
  virtual ~CPCAddressSpace();
  
  void init();
  void identityMap(void * start, uint32_t length);
  void addSection(void * to_addr, void * from_addr, uint32_t length);
  void addRange(const CPCAddressSpace & as, void * start, unsigned int length);
  
  uint32_t cr3();
  
private:
  bool bInitialized_;
  
  // Page Directory Pointer (Table)
  union {
    pde32_t * pPD_;  // Without PAE (pointer to physical addr)
    pdpt64_t  PDPT_; // With PAE
  };
  uint32_t iCR3_;
};


#endif

