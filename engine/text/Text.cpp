#include "Text.hpp"

#include <SDL2/SDL.h>

void Text::draw(Camera *camera) {
    // If properties have been updated re-render text.
    if (mPropertiesUpdated) {
        updateTexture(camera);
        mPropertiesUpdated = false;
    }
  if (mHasBackground) {
    SDL_SetRenderDrawColor(_renderer, mBackColour.r,mBackColour.g, mBackColour.b,mBackColour.a);
    SDL_RenderFillRect(_renderer, &mDstRect);
  }
  // Copy rendered text into the text box (in the renderer)
    SDL_RenderCopy(_renderer, mTextTexture, &mSrcRect, &mDstRect);
}

void Text::updateTexture(Camera *camera) {
    // Render text to texture
    SDL_Surface *mTextSurface = TTF_RenderText_Solid(mFont, mTextString.c_str(), mColour);
    mTextTexture = SDL_CreateTextureFromSurface(_renderer, mTextSurface);
    SDL_FreeSurface(mTextSurface);
    
    // Convert alignment and texture dimensions to displacement from the
    // top-left corner of the text box
    int xDisplacement = 0;
    int yDisplacement = 0;
    
    // Note that mTextScale is used to scale the texture dimensions and the
    // source rectangle such that the render of the text is scaled
    
    // Displacement in the x-direction
    switch (mAlignment[0]) {
        case TEXT_ALIGN_RIGHT:
            xDisplacement = mDimensions[0] - mTextSurface->w * mTextScale[0]
                + mOffset[0];
            break;
        case TEXT_ALIGN_CENTER:
            xDisplacement = (mDimensions[0] - mTextSurface->w
                * mTextScale[0]) / 2 + mOffset[0];
            break;
        default:
            xDisplacement = mOffset[0];
    }
    
    // Displacement in the y-direction
    switch (mAlignment[1]) {
        case TEXT_ALIGN_BOTTOM:
            yDisplacement = mDimensions[1] - mTextSurface->h * mTextScale[1]
                + mOffset[1];
            break;
        case TEXT_ALIGN_CENTER:
            yDisplacement = (mDimensions[1] - mTextSurface->h
                * mTextScale[1]) / 2 + mOffset[1];
            break;
        default:
            yDisplacement = mOffset[1];
    }
    
    // If displacement is negative, change the start position of the source
    // rectangle
    mSrcRect.x = fmax(0, -xDisplacement) / mTextScale[0];
    mSrcRect.y = fmax(0, -yDisplacement) / mTextScale[0];
    // Crop the texture according to the dimensions of the text box and the
    // displacement
    mSrcRect.w = fmin(mTextSurface->w - mSrcRect.x, mDimensions[0] / mTextScale[0]);
    mSrcRect.h = fmin(mTextSurface->h - mSrcRect.y, mDimensions[1] / mTextScale[1]);;
    
    // If displacement is positive, change the start position of the
    // destination rectangle
    mDstRect.x = mPosition[0] + fmax(0, xDisplacement);
    mDstRect.y = mPosition[1] + fmax(0, yDisplacement);
    // Scale the source rectangle dimensions
    mDstRect.w = mSrcRect.w * mTextScale[0];
    mDstRect.h = mSrcRect.h * mTextScale[1];
    
    // Texture has been updated - set boolean back to false.
    mPropertiesUpdated = false;
}
