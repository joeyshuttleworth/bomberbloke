#include "InputField.hpp"

#include <array>
#include <string>

#include "engine.hpp"

const int CURSOR_UPDATE_TICKS = TICK_RATE / 4;

InputField::InputField(std::shared_ptr<Text> text, int xPos, int yPos,
    int xDim, int yDim, AlignFlag xAlignFlag, AlignFlag yAlignFlag)
  : AbstractHudElement(xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag),
  TextHudElement(text, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag),
  ClickableHudElement(xPos, yPos, xDim, yDim, nullptr, xAlignFlag, yAlignFlag) {
  // Use text colour as default colour.
  mDefaultColour = text->getTextColour();
  mInputColour = mDefaultColour;

  mDefaultText = text->getText();
}



void InputField::onInput(SDL_Event *event) {
  ClickableHudElement::onInput(event);

  if (mHasFocus) {
    if (event->type == SDL_KEYDOWN) {
      if (event->key.keysym.sym == SDLK_BACKSPACE && textInput.length() > 0) {
        mPropertiesUpdated = true;
        textInput.pop_back();
      } else if (event->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
        textInput = SDL_GetClipboardText();
      }
    } else if (event->type == SDL_TEXTINPUT) {
      mPropertiesUpdated = true;
      if (!(SDL_GetModState() & KMOD_CTRL && (event->text.text[0] == 'v' || event->text.text[0] == 'V'))) {
        textInput += event->text.text;
        log_message(DEBUG, textInput.c_str());
      }
    }
  }

}


void InputField::draw(Camera *camera) {
  if (mPropertiesUpdated) {
    // Set colour and text offset according to mIsClicked and mIsMouseOver
    if (!textInput.empty()) {
      mText->setText(textInput);
    }
    if (mIsClicked) {
      //Enter TextBox Input
      mText->setTextColour(mOnClickColour);
      mText->setTextOffset(mOnClickOffset[0], mOnClickOffset[1]);
    }
  }

  // Call draw function inherited fromm TextHUDElement
  TextHudElement::draw(camera);
}
