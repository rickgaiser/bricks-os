#include "application.h"
#include "windowManager.h"


namespace twl
{


//---------------------------------------------------------------------------
CApplication::CApplication(int argc, char * argv[])
{
}

//---------------------------------------------------------------------------
CApplication::~CApplication()
{
}

//---------------------------------------------------------------------------
void
CApplication::setMainWidget(CWidget * pMainWidget)
{
}

//---------------------------------------------------------------------------
int
CApplication::exec()
{
  return bwm::windowManager.exec();
}


};
