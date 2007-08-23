#ifndef MEMORYMAP_H
#define MEMORYMAP_H


// Early declaration
class CMemoryManager;


/**
 * \brief Memory Map class holds all information about one memory map
 *
 * A memory map holds memory sections (code, data, stack, etc, etc...)
 */
class CMemoryMap
{
public:
  friend class CMemoryManager;

private:
  CMemoryMap();
  virtual ~CMemoryMap();
};


#endif
