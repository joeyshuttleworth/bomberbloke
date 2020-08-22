#include "AbstractHudElement.hpp"

#include <SDL2/SDL.h>

AbstractHudElement::AbstractHudElement(int xPos, int yPos, int xDim, int yDim,
        AlignFlag xAlignFlag, AlignFlag yAlignFlag) {
    // Actual position is set in updatePosition
    mRelativePosition[0] = xPos;
    mRelativePosition[1] = yPos;
    
    mDimensions[0] = xDim;
    mDimensions[1] = yDim;
    
    mAlignFlags[0] = xAlignFlag;
    mAlignFlags[1] = yAlignFlag;
    
    mPropertiesUpdated = true;
}

void AbstractHudElement::draw(Camera* camera) {
    // If properties have been updated re-position HUD element.
    if (mPropertiesUpdated) {
        updatePosition(camera);
        mPropertiesUpdated = false;
    }
    
    // TODO: remove this placeholder code
    SDL_Rect dstRect;
    dstRect.x = mPosition[0];
    dstRect.y = mPosition[1];
    dstRect.w = mDimensions[0];
    dstRect.h = mDimensions[1];
    
    // Draw a blue box on dstRect
    SDL_SetRenderDrawColor(_renderer, 0xa0, 0xa0, 0xa0, 0xff);
    SDL_RenderFillRect(_renderer, &dstRect);
}

void AbstractHudElement::updatePosition(Camera* camera) {
    std::array<int, 2> screenDimensions = camera->getScreenDimensions();
    
    switch(mAlignFlags[0]) {
        case ALIGN_CENTER:
            // Centred positioning
            mPosition[0] = (screenDimensions[0] - mDimensions[0]) / 2;
            break;
        case ALIGN_RIGHT:
            // Right-aligned positioning
            mPosition[0] = screenDimensions[0] - mDimensions[0] - mRelativePosition[0];
            break;
        default:
            // Left-aligned positioning
            mPosition[0] = mRelativePosition[0];
    }
    
    switch(mAlignFlags[1]) {
        case ALIGN_CENTER:
            // Centred positioning
            mPosition[1] = (screenDimensions[1] - mDimensions[1]) / 2;
            break;
        case ALIGN_RIGHT:
            // Right-aligned positioning
            mPosition[1] = screenDimensions[1] - mDimensions[1] - mRelativePosition[1];
            break;
        default:
            // Left-aligned positioning
            mPosition[1] = mRelativePosition[1];
    }
}