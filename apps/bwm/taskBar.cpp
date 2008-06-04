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

  pWindow_->drawHLine(rect.x(), rect.y() + 0, rect.width(), clPanelFill);
  pWindow_->drawHLine(rect.x(), rect.y() + 1, rect.width(), clPanelLight);
  pWindow_->fillRect(CRect(rect.x(), rect.y() + 2, rect.width(), rect.height() - 1), clPanelFill);

  return true;
}

//---------------------------------------------------------------------------
bool
CTaskBar::eventResize(const CEventResize & event)
{
  // Update client rect area first
  rectRelativeClient_.rect(rect().x(), rect().y() + 2, rect().width(), rect().height() - 2);
  rectAbsoluteClient_.rect(rectAbsolute().x(), rectAbsolute().y() + 2, rectAbsolute().width(), rectAbsolute().height() - 2);

  if(rect().height() == 14)
    startButton_.rect(1, 1, 27, rectClient().height() - 2); // Tiny mode
  else
    startButton_.rect(2, 2, 54, rectClient().height() - 4); // Normal mode

  startButton_.visible(true);

  return true;
}


};
