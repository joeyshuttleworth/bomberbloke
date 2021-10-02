#include "InputField.hpp"

#include <array>
#include <string>

#include "engine.hpp"

const int CURSOR_UPDATE_TICKS = TICK_RATE / 4;

InputField::InputField(std::shared_ptr<Text> text,
                       int xPos,
                       int yPos,
                       int xDim,
                       int yDim,
                       AlignFlag xAlignFlag,
                       AlignFlag yAlignFlag)
  : AbstractHudElement(xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag)
  , TextHudElement(text, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag)
  , ClickableHudElement(xPos, yPos, xDim, yDim, nullptr, xAlignFlag, yAlignFlag)
{
  // Use text colour as default colour.
  if (text)
    mDefaultColour = text->getTextColour();
  mInputColour = mDefaultColour;

  if (text)
    mDefaultText = text->getText();
}

void
InputField::onInput(SDL_Event* event)
{
  if (event->type == SDL_MOUSEBUTTONUP && mHasFocus == true) {
    // If mouse button up, reset mHasFocus before checking
    // If button up is on the input field onClick will be called
    mHasFocus = false;
    if (mText)
      mText->setCursorVisible(false);
    mPropertiesUpdated = true;
  }
  ClickableHudElement::onInput(event);

  if (mHasFocus) {
    if (event->type == SDL_KEYDOWN) {
      if (event->key.keysym.sym == SDLK_BACKSPACE && mCursorIndex > 0) {
        // Handle backspace
        mTextInput = mTextInput.substr(0, mCursorIndex - 1) +
                     mTextInput.substr(mCursorIndex);
        mCursorIndex--;
        if (mText) {
          mText->setText(mTextInput);
          mText->setCursorIndex(mCursorIndex);
        }
      } else if (event->key.keysym.sym == SDLK_v &&
                 SDL_GetModState() & KMOD_CTRL) {
        // Handle clipboard-paste
        std::string pasteText = SDL_GetClipboardText();
        mTextInput = mTextInput.substr(0, mCursorIndex) + pasteText +
                     mTextInput.substr(mCursorIndex);
        mCursorIndex += pasteText.length();
        if (mText) {
          mText->setText(mTextInput);
          mText->setCursorIndex(mCursorIndex);
        }
      } else if (event->key.keysym.sym == SDLK_LEFT && mCursorIndex > 0) {
        // Handle left button
        mCursorIndex--;
        if (mText)
          mText->setCursorIndex(mCursorIndex);
      } else if (event->key.keysym.sym == SDLK_RIGHT &&
                 mCursorIndex < mTextInput.length()) {
        // Handle right button
        mCursorIndex++;
        if (mText)
          mText->setCursorIndex(mCursorIndex);
      }
    } else if (event->type == SDL_TEXTINPUT) {
      // Handle generic text input
      if (!(SDL_GetModState() & KMOD_CTRL &&
            (event->text.text[0] == 'v' || event->text.text[0] == 'V'))) {
        mTextInput = mTextInput.substr(0, mCursorIndex) + event->text.text +
                     mTextInput.substr(mCursorIndex);
        mCursorIndex++;
        if (mText) {
          mText->setText(mTextInput);
          mText->setCursorIndex(mCursorIndex);
        }
      }
    }
  }
}

void
InputField::onClick(int x, int y)
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
