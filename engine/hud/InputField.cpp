#include "InputField.hpp"

#include <array>
#include <string>

#include "engine.hpp"

const int CURSOR_UPDATE_TICKS = TICK_RATE / 4;

InputField::InputField(scene& r_scene,
                       std::shared_ptr<Text> text,
                       int xPos,
                       int yPos,
                       int xDim,
                       int yDim,
                       AlignFlag xAlignFlag,
                       AlignFlag yAlignFlag)
  : AbstractHudElement(r_scene, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag)
  , ClickableHudElement(r_scene, xPos, yPos, xDim, yDim, nullptr, xAlignFlag, yAlignFlag)
  , TextHudElement(r_scene, text, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag)
{
  // Use text colour as default colour.
  if (text)
    mDefaultColour = text->getTextColour();
  mInputColour = mDefaultColour;

  if (text)
    mDefaultText = text->getText();
}

void
InputField::onInput(const AbstractInputEvent& event)
{
  if (event.getInputType() == IEVENT_MOUSEBUTTONUP && mHasFocus == true) {
    // If mouse button up, reset mHasFocus before checking
    // If button up is on the input field onClick will be called
    mHasFocus = false;
    if (mText)
      mText->setCursorVisible(false);
    mPropertiesUpdated = true;
  }
  ClickableHudElement::onInput(event);

  // Handle text input
  if (mHasFocus)
    {
      IInputManager& input_manager = mrScene.getInputManager();
      auto ret_pair = input_manager.handleTextInput(mTextInput, mCursorIndex,
                                                    event);

      mTextInput = ret_pair.first;
      mText->setCursorIndex(ret_pair.second);
      mCursorIndex = ret_pair.second;
      mPropertiesUpdated = true;
    }
}

void
InputField::onClick(int x, int)
{
  mHasFocus = true;
  mPropertiesUpdated = true;

  // Set cursor position
  if (mTextInput != "") {
    mCursorIndex = mText->getCursorIndex(x);
  } else {
    mCursorIndex = 0;
  }
  if (mText) {
    mText->setCursorIndex(mCursorIndex);
    mText->setCursorVisible(true);
    mText->setText(mTextInput);
  }
}

void
InputField::update()
{
  // Flashing cursor animation
  if (mHasFocus && _tick % CURSOR_UPDATE_TICKS == 0) {
    if ((_tick / CURSOR_UPDATE_TICKS) % 2 == 0) {
      mText->setCursorVisible(false);
    } else {
      if (mText)
        mText->setCursorVisible(true);
    }
  }
}

void
InputField::draw(Camera* camera)
{
  if (mText && mPropertiesUpdated) {
    if (mTextInput == "") {
      mText->setText(mDefaultText);
      mText->setTextColour(mDefaultColour);
    } else {
      mText->setText(mTextInput);
      mText->setTextColour(mInputColour);
    }

    if (mHasFocus) {
      mText->setText(mTextInput);
      mText->setCursorVisible(true);
      mText->setTextColour(mInputColour);
    } else {
      mText->setCursorVisible(false);
    }
  }

  TextHudElement::draw(camera);
}
