#ifndef BRICKS_H
#define BRICKS_H


// SYS include files
#include "interruptManager.h"


class CBricks
{
public:
  static CBricks * instance();

  int main();

private:
  CBricks();   // Constructor initializes the entire OS
  virtual ~CBricks();

  CInterruptManager * pInterruptManager_;

  static CBricks      singleton_;
};


#endif
