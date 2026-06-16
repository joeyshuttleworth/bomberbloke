#include "ClickableHudElement.hpp"
#include "IInputManager.hpp"
#include "AbstractInputEvent.hpp"

ClickableHudElement::ClickableHudElement(scene& rscene,
                                         int xPos,
                                         int yPos,
                                         int xDim,
                                         int yDim,
                                         std::function<void()> onClickFn,
                                         AlignFlag xAlignFlag,
                                         AlignFlag yAlignFlag)
  : AbstractHudElement(rscene, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag)
{
  // Base class constructor must be called
  mOnClickFn = onClickFn;

  // Set to be interactive so that onInput is called by scene
  mIsInteractive = true;
}

bool
ClickableHudElement::isCoordOnElement(int x, int y)
{
  if (x >= mPosition[0] && x < mPosition[0] + mDimensions[0]) {
    if (y >= mPosition[1] && y < mPosition[1] + mDimensions[1])
      return true;
  }
  return false;
}

void
ClickableHudElement::onInput(const AbstractInputEvent& event)
{
  if (!mIsInteractive)
    return;

  // If its a mouse button up event, it cannot be clicked
  if (event.getInputType() == IEVENT_MOUSEBUTTONUP) {
    if (mIsClicked) {
      mIsClicked = false;
      mPropertiesUpdated = true;
      auto loc = event.getMouseLocation();
      onClick(loc[0], loc[1]);
    }
  } else if (event.getInputType() == IEVENT_MOUSEBUTTONDOWN) {
    // If it is a mouse button down event and the cursor is on the button
    // it must be clicked and the mouse must be over it
    auto loc = event.getMouseLocation();
    if (isCoordOnElement(loc[0], loc[1])) {
      mIsMouseOver = true;
      mIsClicked = true;
      mPropertiesUpdated = true;
    }
  } else if (event.getInputType() == IEVENT_MOUSEMOTION) {
    // If it is a mouse motion event check if the cursor is on the button
    auto loc = event.getMouseLocation();
    bool newIsMouseOver = isCoordOnElement(loc[0], loc[1]);
    // Check if IsMouseOver has changed
    if (newIsMouseOver != mIsMouseOver) {
      mIsMouseOver = newIsMouseOver;
      mPropertiesUpdated = true;
    }
  }
}

void
ClickableHudElement::onClick(int, int)
{
  if (mOnClickFn != nullptr)
    mOnClickFn();
}
