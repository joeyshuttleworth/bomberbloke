#include "TextButton.hpp"

#include <array>
#include <string>

#include "Sound.hpp"
#include "engine.hpp"

const std::string CLICK_SOUND_NAME = "click";

TextButton::TextButton(scene& r_scene,
                       std::shared_ptr<Text> text,
                       int xPos,
                       int yPos,
                       int xDim,
                       int yDim,
                       std::function<void()> onClickFn,
                       AlignFlag xAlignFlag,
                       AlignFlag yAlignFlag)
  :
  AbstractHudElement(r_scene, xPos, yPos, xDim, yDim,xAlignFlag, yAlignFlag),
  ClickableHudElement(r_scene, xPos,
                        yPos,
                        xDim,
                        yDim,
                        onClickFn,
                        xAlignFlag,
                        yAlignFlag)
  , TextHudElement(r_scene, text, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag)
{
  // Use text colour as default colour.
  if (text)
    mDefaultColour = text->getTextColour();

  mMouseOverColour = mDefaultColour;
  mOnClickColour = mDefaultColour;

  // Use text offset as default offset.
  std::array<int, 2> mOffsetArray =
    text ? text->getTextOffset() : std::array<int, 2>{ 0, 0 };
  mDefaultOffset[0] = mOffsetArray[0];
  mDefaultOffset[1] = mOffsetArray[1];

  mMouseOverOffset[0] = mOffsetArray[0];
  mMouseOverOffset[1] = mOffsetArray[1];

  mOnClickOffset[0] = mOffsetArray[0];
  mOnClickOffset[1] = mOffsetArray[1];

  ISoundManager& sound_manager = r_scene.getIOSystem().getSoundManager();

  // Get click sound
  mClickSound = sound_manager.createSound(CLICK_SOUND_NAME);
  if(mClickSound)
    mClickSound->setGroup(SOUND_FX);
}

void
TextButton::draw(Camera* camera)
{
  if (!mIsVisible)
    return;

  if (mPropertiesUpdated && mText) {
    // Set colour and text offset according to mIsClicked and mIsMouseOver
    if (mIsClicked) {
      mText->setTextColour(mOnClickColour);
      mText->setTextOffset(mOnClickOffset[0], mOnClickOffset[1]);
    } else if (mIsMouseOver) {
      mText->setTextColour(mMouseOverColour);
      mText->setTextOffset(mMouseOverOffset[0], mMouseOverOffset[1]);
    } else {
      mText->setTextColour(mDefaultColour);
      mText->setTextOffset(mDefaultOffset[0], mDefaultOffset[1]);
    }
  }

  // Call draw function inherited fromm TextHUDElement
  TextHudElement::draw(camera);
}

void
TextButton::onClick(int x, int y)
{

  // Play click sound
  mrSoundManager.playSound(mClickSound.get());

  // Call mOnClick function.
  ClickableHudElement::onClick(x, y);
}
