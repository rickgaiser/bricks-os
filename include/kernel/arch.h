#ifndef ARCH_H
#define ARCH_H


namespace arch
{


//----------------------------------------------------------------------------
class IArch
{
public:
  virtual int init() = 0;
};

//----------------------------------------------------------------------------
class IProcessor
{
public:
  virtual void cli() = 0;
  virtual void sti() = 0;
  virtual void halt() = 0;
};


extern IArch      * pArch;
extern IProcessor * pProcessor;


};


#endif
