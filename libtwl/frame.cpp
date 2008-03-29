#include "frame.h"


namespace twl
{


/*
CPixmap CFrame::pxWinTopLeft    ("images/mg1/top-left.png");
CPixmap CFrame::pxWinTop        ("images/mg1/top.png");
CPixmap CFrame::pxWinTopRight   ("images/mg1/top-right.png");
CPixmap CFrame::pxWinRight      ("images/mg1/right.png");
CPixmap CFrame::pxWinBottomRight("images/mg1/bottom-right.png");
CPixmap CFrame::pxWinBottom     ("images/mg1/bottom.png");
CPixmap CFrame::pxWinBottomLeft ("images/mg1/bottom-left.png");
CPixmap CFrame::pxWinLeft       ("images/mg1/left.png");
*/
/*
CPixmap CFrame::pxWinTopLeft    ("images/mg2/top-left.png");
CPixmap CFrame::pxWinTop        ("images/mg2/top.png");
CPixmap CFrame::pxWinTopRight   ("images/mg2/top-right.png");
CPixmap CFrame::pxWinRight      ("images/mg2/right.png");
CPixmap CFrame::pxWinBottomRight("images/mg2/bottom-right.png");
CPixmap CFrame::pxWinBottom     ("images/mg2/bottom.png");
CPixmap CFrame::pxWinBottomLeft ("images/mg2/bottom-left.png");
CPixmap CFrame::pxWinLeft       ("images/mg2/left.png");
*/

//---------------------------------------------------------------------------
CFrame::CFrame(CWidget * parent, EFrameType type)
 : CWidget(parent)
 , eType_(type)
{
}

//---------------------------------------------------------------------------
CFrame::~CFrame()
{
}

//---------------------------------------------------------------------------
void
CFrame::setFrameType(EFrameType type)
{
  eType_ = type;
}

//---------------------------------------------------------------------------
bool
CFrame::eventRedraw(const CEventRedraw & event)
{
  const CRect & rect = this->rectAbsolute();

  switch(eType_)
  {
    case ftNone:
      pWindow_->fillRect(CRect(rect.x(), rect.y(), rect.width(), rect.height()), clPanelFill);
      break;
    case ftPlain:
      pWindow_->fillRect(CRect(rect.x() + 1, rect.y() + 1, rect.width() - 2, rect.height() - 2), clPanelFill);
      pWindow_->drawHLine(rect.x(), rect.y(),          rect.width(), clPanelShade1);         // Top
      pWindow_->drawHLine(rect.x(), rect.bottom() - 1, rect.width(), clPanelShade1);         // Bottom
      pWindow_->drawVLine(rect.x(),         rect.y() + 1, rect.height() - 2, clPanelShade1); // Left
      pWindow_->drawVLine(rect.right() - 1, rect.y() + 1, rect.height() - 2, clPanelShade1); // Right
      break;
    case ftRaised:
      pWindow_->fillRect(CRect(rect.x() + 1, rect.y() + 1, rect.width() - 3, rect.height() - 3), clPanelFill);
      pWindow_->drawHLine(rect.x(), rect.bottom() - 1, rect.width(), clPanelShade1);         // Bottom
      pWindow_->drawVLine(rect.right() - 1, rect.y(), rect.height() - 1, clPanelShade1);     // Right
      pWindow_->drawHLine(rect.x(), rect.y(), rect.width() - 1, clPanelLight);               // Top
      pWindow_->drawVLine(rect.x(), rect.y() + 1, rect.height() - 2, clPanelLight);          // Left
      pWindow_->drawHLine(rect.x() + 1, rect.bottom() - 2, rect.width() - 2, clPanelShade2); // Bottom
      pWindow_->drawVLine(rect.right() - 2, rect.y() + 1, rect.height() - 3, clPanelShade2); // Right
      break;
    case ftSunken:
      pWindow_->fillRect(CRect(rect.left() + 2, rect.top() + 2, rect.width() - 3, rect.height() - 3), clPanelFill);
      pWindow_->drawHLine(rect.x(), rect.y(), rect.width(), clPanelShade1);                    // Top
      pWindow_->drawVLine(rect.x(), rect.y() + 1, rect.height() - 1, clPanelShade1);           // Left
      pWindow_->drawHLine(rect.x() + 1, rect.bottom() - 1, rect.width() - 1, clPanelLight);    // Bottom
      pWindow_->drawVLine(rect.right() - 1, rect.y() + 1, rect.height() - 2, clPanelLight);    // Right
      pWindow_->drawHLine(rect.x() + 1, rect.y() + 1, rect.width() - 2, clPanelShade2);        // Top
      pWindow_->drawVLine(rect.x() + 1, rect.y() + 2, rect.height() - 3, clPanelShade2);       // Left
      break;
  };

/*
  // Top-Left
  pWindow_->bitBlt(0, 0, pxWinTopLeft, eBT_COPY);
  // Top
  for(int i(pxWinTopLeft.width()); i < (this->width() - pxWinTopRight.width()); i++)
    pWindow_->bitBlt(i, 0, pxWinTop, eBT_COPY);
  // Top-Right
  pWindow_->bitBlt(this->width() - pxWinTopRight.width(), 0, pxWinTopRight, eBT_COPY);
  // Right
  for(int i(pxWinTopRight.height()); i < (this->height() - pxWinBottomRight.height()); i++)
    pWindow_->bitBlt(this->width() - pxWinRight.width(), i, pxWinRight, eBT_COPY);
  // Bottom-Right
  pWindow_->bitBlt(this->width() - pxWinBottomRight.width(), this->height() - pxWinBottomRight.height(), pxWinBottomRight, eBT_COPY);
  // Bottom
  for(int i(pxWinBottomLeft.width()); i < (this->width() - pxWinBottomRight.width()); i++)
    pWindow_->bitBlt(i, this->height() - pxWinBottom.height(), pxWinBottom, eBT_COPY);
  // Bottom-Left
  pWindow_->bitBlt(0, this->height() - pxWinBottomLeft.height(), pxWinBottomLeft, eBT_COPY);
  // Left
  for(int i(pxWinTopLeft.height()); i < (this->height() - pxWinBottomLeft.height()); i++)
    pWindow_->bitBlt(0, i, pxWinLeft, eBT_COPY);
*/

  return true;
}


};
