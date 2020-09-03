#include "InputField.hpp"

#include <array>

InputField::InputField(std::shared_ptr<Text> text, int xPos, int yPos, int xDim,
                       int yDim, void (*onClickFn)(), AlignFlag xAlignFlag, AlignFlag yAlignFlag)
        : AbstractHudElement(xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag),
          ClickableHudElement(xPos, yPos, xDim, yDim, onClickFn, xAlignFlag, yAlignFlag),
          TextHudElement(text, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag) {
  // Use text colour as default colour.
  mDefaultColour = text->getTextColour();
  mMouseOverColour = mDefaultColour;
  mOnClickColour = mDefaultColour;

  // Set Input Field Green
  text->setBackgroundColour(SDL_Color{0,255,0,255});


  // Use text offset as default offset.
  std::array<int, 2> mOffsetArray = text->getTextOffset();
  mDefaultOffset[0] = mOffsetArray[0];
  mDefaultOffset[1] = mOffsetArray[1];

  mMouseOverOffset[0] = mOffsetArray[0];
  mMouseOverOffset[1] = mOffsetArray[1];

  mOnClickOffset[0] = mOffsetArray[0];
  mOnClickOffset[1] = mOffsetArray[1];
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
