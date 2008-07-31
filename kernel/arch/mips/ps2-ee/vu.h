#ifndef ARCH_VU_H
#define ARCH_VU_H


#include "inttypes.h"


#define VU0_CODE_START ((void *)0x11000000)
#define VU0_DATA_START ((void *)0x11004000)
#define VU1_CODE_START ((void *)0x11008000)
#define VU1_DATA_START ((void *)0x1100c000)


void vu_upload(void * dst, const void * src, unsigned int size);
void vu1_wait();
void vu1_run();


// -----------------------------------------------------------------------------
class CVUData
{
public:
  CVUData(const void * data, uint32_t size) : pData_(data), iSize_(size) {}

  inline void upload(void * addr){vu1_wait();vu_upload(addr, pData_, iSize_);}

private:
  const void * pData_;
  uint32_t iSize_;
};

// -----------------------------------------------------------------------------
class CVUCode
{
public:
  CVUCode(const void * data, uint32_t size) : pData_(data), iSize_(size) {}

  inline void upload(void * addr){vu1_wait();vu_upload(addr, pData_, iSize_);}
  inline void run(){vu1_run();}
  inline void wait(){vu1_wait();}

private:
  const void * pData_;
  uint32_t iSize_;
};


#endif
