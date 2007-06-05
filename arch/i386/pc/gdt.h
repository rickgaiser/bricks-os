#ifndef GDT_H
#define GDT_H


#include "hal.h"


#define MAX_GLOBAL_DESCRIPTORS 0x20


typedef uint16_t selector_t;


enum ESegmentType
{
    stCode
  , stData
  , stStack
  , stTSS
};

enum EGateType
{
    gtCall
  , gtTask
  , gtInterrupt
  , gtTrap
};


class CGDT
{
public:
  CGDT();
  virtual ~CGDT();

  virtual int init();

  selector_t createSegment(ESegmentType type, unsigned int privilege, uint32_t base, uint32_t limit);
//  selector_t createGate   (ESegmentType type, unsigned int privilege, selector_t selector, uint32_t offset, unsigned int wordCount = 0);

private:
  SGlobalDescriptor    gd_[MAX_GLOBAL_DESCRIPTORS]; // Global Descriptor Table
  SDescriptorTableReg  gdtr_;                       // Global Descriptor Table Register
};


extern selector_t selNull;
extern selector_t selCodeSys;
extern selector_t selDataSys;
extern selector_t selCodeUser;
extern selector_t selDataUser;


#endif
