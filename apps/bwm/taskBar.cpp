#include "taskBar.h"


namespace bwm
{


using namespace twl;


//---------------------------------------------------------------------------
CTaskBar::CTaskBar(CWidget * parent)
 : CWidget(parent)
 , startButton_(this, "Start")
{
}

//---------------------------------------------------------------------------
CTaskBar::~CTaskBar()
{
}

//---------------------------------------------------------------------------
bool
CTaskBar::eventRedraw(const CEventRedraw & event)
{
  const CRect & rect = this->rectAbsolute();

  pWindow_->drawHLine(rect.x(), rect.y(), rect.width(), clPanelLight);
  pWindow_->fillRect(CRect(rect.x(), rect.y() + 1, rect.width(), rect.height() - 1), clPanelFill);

  return true;
}

//---------------------------------------------------------------------------
bool
CTaskBar::eventResize(const CEventResize & event)
{
  startButton_.rect(2, 2, 24, height() - 3);
  startButton_.visible(true);

  return true;
}


};
