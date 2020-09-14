#include "TextButton.hpp"

#include <array>
#include <string>

#include "Sound.hpp"

const std::string CLICK_SOUND_NAME = "click";

TextButton::TextButton(std::shared_ptr<Text> text, int xPos, int yPos, int xDim,
        int yDim, std::function<void()> onClickFn, AlignFlag xAlignFlag,
        AlignFlag yAlignFlag)
    : AbstractHudElement(xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag),
    ClickableHudElement(xPos, yPos, xDim, yDim, onClickFn, xAlignFlag, yAlignFlag),
    TextHudElement(text, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag) {
    // Use text colour as default colour.
    mDefaultColour = text->getTextColour();
    mMouseOverColour = mDefaultColour;
    mOnClickColour = mDefaultColour;

    // Use text offset as default offset.
    std::array<int, 2> mOffsetArray = text->getTextOffset();
    mDefaultOffset[0] = mOffsetArray[0];
    mDefaultOffset[1] = mOffsetArray[1];

    mMouseOverOffset[0] = mOffsetArray[0];
    mMouseOverOffset[1] = mOffsetArray[1];

    mOnClickOffset[0] = mOffsetArray[0];
    mOnClickOffset[1] = mOffsetArray[1];

    // Get click sound
    mClickSound = soundManager.createSound(CLICK_SOUND_NAME);
}

void TextButton::draw(Camera *camera) {
    if (!mIsVisible)
        return;

    if (mPropertiesUpdated) {
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

void TextButton::onClick() {
    // Play click sound
    soundManager.playSound(mClickSound);

    // Call mOnClick function.
    ClickableHudElement::onClick();
}
