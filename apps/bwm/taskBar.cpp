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
  if(rect().height() == 14)
    startButton_.rect(1, 1, 27, rectClient().height() - 2); // Tiny mode
  else
    startButton_.rect(2, 2, 54, rectClient().height() - 4); // Normal mode

  startButton_.visible(true);

  return true;
}


};
