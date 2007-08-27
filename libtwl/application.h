#ifndef TWL_APPLICATION_H
#define TWL_APPLICATION_H


#include "widget.h"


namespace twl
{


class CApplication
{
public:
  CApplication(int argc = 0, char * argv[] = 0);
  virtual ~CApplication();

  void setMainWidget(CWidget * pMainWidget);
  int exec();

private:
};


};


#endif
