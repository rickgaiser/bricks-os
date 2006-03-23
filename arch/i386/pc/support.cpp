#include "i386Arch.h"
#include "kernel/arch.h"
#include "kernel/bricks.h"
#include "processor.h"


CI386Arch   cArch;
CProcessor  cProcessor;


void *__dso_handle;


// -----------------------------------------------------------------------------
extern "C" void
_init()
{
  extern void (*__CTOR_LIST__)();               //the ctor list is defined in the linker script
  void (**constructor)() = &__CTOR_LIST__;      //hold current constructor in list
  int total = *(int *)constructor;              //the first int is the number of constructors

  constructor++;                                //increment to first constructor

  while(total)
  {
    (*constructor)();
    total--;
    constructor++;
  }
}

// -----------------------------------------------------------------------------
extern "C" void
__cxa_atexit()                                  //Walk and call the deconstructors in the dtor_list
{
  extern void (*__DTOR_LIST__)();               //the dtor list is defined in the linker script
  void (**deconstructor)() = &__DTOR_LIST__;    //hold current deconstructor in list
  int total = *(int *)deconstructor;            //the first int is the number of deconstructors

  deconstructor++;                              //increment to first deconstructor

  while(total)
  {
    (*deconstructor)();
    total--;
    deconstructor++;
  }
}

// -----------------------------------------------------------------------------
// C++ support function
extern "C"
void
__cxa_pure_virtual()
{
}

// -----------------------------------------------------------------------------
// C++ support function
void
operator delete(void *)
{
}

// -----------------------------------------------------------------------------
extern "C"
void
_alloca()
{
}

// -----------------------------------------------------------------------------
// main: entry point of OS.
extern "C"
int
main(int, char *[])
{
  int iRetVal;

  _init();

  arch::pArch      = &cArch;
  arch::pProcessor = &cProcessor;
  iRetVal = CBricks::instance()->main();

  __cxa_atexit();

  return(iRetVal);
}

// -----------------------------------------------------------------------------
extern "C"
int
__main()
{
  return(main(0, 0));
}
