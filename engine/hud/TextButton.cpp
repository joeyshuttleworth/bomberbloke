#include "TextButton.hpp"

#include <array>

TextButton::TextButton(std::shared_ptr<Text> text, int xPos, int yPos, int xDim,
        int yDim, void (*onClickFn)(), AlignFlag xAlignFlag, AlignFlag yAlignFlag)
    : AbstractHudElement(xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag),
    ClickableHudElement(xPos, yPos, xDim, yDim, onClickFn, xAlignFlag, yAlignFlag),
    TextHudElement(text, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag) {
    // Use text colour as default colour.
    std::array<int, 4> mColourArray = text->getTextColour();
    
    mDefaultColour.r = mColourArray[0];
    mDefaultColour.g = mColourArray[1];
    mDefaultColour.b = mColourArray[2];
    mDefaultColour.a = mColourArray[3];
    
    mMouseOverColour.r = mColourArray[0];
    mMouseOverColour.g = mColourArray[1];
    mMouseOverColour.b = mColourArray[2];
    mMouseOverColour.a = mColourArray[3];
    
    mOnClickColour.r = mColourArray[0];
    mOnClickColour.g = mColourArray[1];
    mOnClickColour.b = mColourArray[2];
    mOnClickColour.a = mColourArray[3];
    
    
    // Use text offset as default offset.
    std::array<int, 2> mOffsetArray = text->getTextOffset();
    mDefaultOffset[0] = mOffsetArray[0];
    mDefaultOffset[1] = mOffsetArray[1];
    
    mMouseOverOffset[0] = mOffsetArray[0];
    mMouseOverOffset[1] = mOffsetArray[1];
    
    mOnClickOffset[0] = mOffsetArray[0];
    mOnClickOffset[1] = mOffsetArray[1];
}

void TextButton::draw(Camera *camera) {
    if (mPropertiesUpdated) {
        // Set colour and text offset according to mIsClicked and mIsMouseOver
        if (mIsClicked) {
            mText->setTextColour(mOnClickColour.r, mOnClickColour.g,
                mOnClickColour.b, mOnClickColour.a);
            mText->setTextOffset(mOnClickOffset[0], mOnClickOffset[1]);
        } else if (mIsMouseOver) {
            mText->setTextColour(mMouseOverColour.r, mMouseOverColour.g,
                mMouseOverColour.b, mMouseOverColour.a);
            mText->setTextOffset(mMouseOverOffset[0], mMouseOverOffset[1]);
        } else {
            mText->setTextColour(mDefaultColour.r, mDefaultColour.g,
                mDefaultColour.b, mDefaultColour.a);
            mText->setTextOffset(mDefaultOffset[0], mDefaultOffset[1]);
        }
    }
    
    // Call draw function inherited fromm TextHUDElement
    TextHudElement::draw(camera);
}