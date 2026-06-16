#include "TextHudElement.hpp"

TextHudElement::TextHudElement(scene& r_scene,
                               std::shared_ptr<Text> text,
                               int xPos,
                               int yPos,
                               int xDim,
                               int yDim,
                               AlignFlag xAlignFlag,
                               AlignFlag yAlignFlag)
  : AbstractHudElement(r_scene, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag)
{
  // Set text object
  mText = text;
}

void
TextHudElement::updatePosition(Camera* camera)
{
  AbstractHudElement::updatePosition(camera);

  // Update the text position using the new HUD position parameters
  if (mText) {
    mText->setDimensions(mDimensions[0], mDimensions[1]);
    mText->setPosition(mPosition[0], mPosition[1]);
  }
}

void
TextHudElement::draw(Camera* camera)
{
  if (!mIsVisible)
    return;

  AbstractHudElement::draw(camera);
  if (mText)
    mText->draw(camera, mIsPostProcessed);
}
