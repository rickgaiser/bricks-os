#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H


#include "memoryMap.h"


/**
 * \brief Memory Manager class owns all physical memory
 *
 *   The memory manager is the owner of all physical memory. It is responsible
 *   for:
 *     - creating memory maps (CMemoryMap objects)
 *     - deleting memory maps
 *     - adding sections to memory maps
 */
class CMemoryManager
{
public:
  CMemoryManager();
  virtual ~CMemoryManager();

  /**
   * \brief Create CMemoryMap object and release ownership
   */
  CMemoryMap * createMemoryMap();
  /**
   * \brief Gain ownership of CMemoryMap object and delete it
   */
  void deleteMemoryMap(CMemoryMap * pMemoryMAp);
  /**
   * \brief Add a section to CMemoryMap object
   */
  void addSection(CMemoryMap * pMemoryMap, unsigned char * pData, unsigned int iSize, unsigned char * pOffset);

private:
};


#endif
