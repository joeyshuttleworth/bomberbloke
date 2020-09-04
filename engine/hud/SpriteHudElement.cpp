#include "SpriteHudElement.hpp"

#include <SDL2/SDL.h>

SpriteHudElement::SpriteHudElement(std::string asset, int xPos, int yPos,
        int xDim, int yDim, AlignFlag xAlignFlag, AlignFlag yAlignFlag)
    : AbstractHudElement(xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag) {
    // Get sprite from asset name
    mSprite = get_sprite(asset);
}

void SpriteHudElement::draw(Camera *camera) {
    if (!mIsVisible)
        return;

    // Base class draw function
    AbstractHudElement::draw(camera);

    // Create destination rectangle
    SDL_Rect dstRect;
    dstRect.x = mPosition[0];
    dstRect.y = mPosition[1];
    dstRect.w = mDimensions[0];
    dstRect.h = mDimensions[1];

    // Copy sprite to destination rectangle
    camera->displayTexture(mSprite, nullptr, &dstRect, mIsPostProcessed, mGlowAmount);
}
