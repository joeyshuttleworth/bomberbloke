#include "TextHudElement.hpp"

TextHudElement::TextHudElement(std::shared_ptr<Text> text, int xPos, int yPos,
        int xDim, int yDim, AlignFlag xAlignFlag, AlignFlag yAlignFlag)
    : AbstractHudElement(xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag) {
    // Set text object
    mText = text;
}

void TextHudElement::updatePosition(Camera* camera) {
    AbstractHudElement::updatePosition(camera);

    // Update the text position using the new HUD position parameters
    mText->setDimensions(mDimensions[0], mDimensions[1]);
    mText->setPosition(mPosition[0], mPosition[1]);
}
