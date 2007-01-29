// SYS include files
#include "kernel/bricks.h"
#include "kernel/timerManager.h"
#include "kernel/taskManager.h"
#include "asm/arch.h"
#include "asm/cpu.h"

// STL include files
#include "iostream"
#include "string"

// Posix include files
#include "unistd.h"


// -----------------------------------------------------------------------------
// Static data members
CBricks   CBricks::singleton_;


// -----------------------------------------------------------------------------
CBricks *
CBricks::instance()
{
  return &singleton_;
}

// -----------------------------------------------------------------------------
CBricks::CBricks()
 : pInterruptManager_(CInterruptManager::instance())
{
}

// -----------------------------------------------------------------------------
CBricks::~CBricks()
{
}

// -----------------------------------------------------------------------------
int
CBricks::main()
{
  // Initialize architecture first
  if(arch::init() == -1)
    std::cout<<"Arch...ERROR"<<std::endl;
  else
    std::cout<<"Arch...OK"<<std::endl;

  // Initialize timer manager
  std::cout<<"Timer manager...";
  if(CTimerManager::instance()->init() == -1)
    std::cout<<"ERROR"<<std::endl;
  else
    std::cout<<"OK"<<std::endl;
/*  
  // Initialize task manager
  std::cout<<"Task manager...";
  if(CTaskManager::instance()->init() == -1)
    std::cout<<"ERROR"<<std::endl;
  else
    std::cout<<"OK"<<std::endl;
*/
  // Welcome user :-)
  std::cout<<"Bricks OS"<<std::endl;
  std::cout<<"========="<<std::endl;

  // Enable interrupts
  CCPU::sti();

  // Forever do nothing
//  std::cout<<"Entering key loop"<<std::endl;
  while(true)
  {
    char buffer[10];

    ::read(STDIN, buffer, 10);

    std::cout<<"key pressed: "<<buffer<<std::endl;
  }

  return(0);
}
