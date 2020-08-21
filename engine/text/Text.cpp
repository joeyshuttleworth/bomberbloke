#include "Text.hpp"

#include <SDL2/SDL.h>

void Text::draw(Camera *camera) {
    // If properties have been updated re-render text.
    if (mPropertiesUpdated) {
        // Render text to texture
        SDL_Surface *mTextSurface = TTF_RenderText_Solid(mFont, mTextString.c_str(), colour);
        mTextTexture = SDL_CreateTextureFromSurface(_renderer, mTextSurface);
        
        // Offset text by changing start coordinates of source rectangle.
        // Align text by changing offset.
        
        // Alignment/offset in x-direction.
        switch (mAlignment[0]) {
            case ALIGN_RIGHT:
                mSrcRect.x = -(mSrcRect.w - mTextSurface->w + mOffset[0]);
                break;
            case ALIGN_CENTER:
                mSrcRect.x = -((mSrcRect.w - mTextSurface->w) / 2 + mOffset[0]);
                break;
            default:
                mSrcRect.x = -mOffset[0];
        }
        
        // Alignment/offset in y-direction.
        switch (mAlignment[0]) {
            case ALIGN_BOTTOM:
                mSrcRect.y = -(mSrcRect.h - mTextSurface->h + mOffset[1]);
                break;
            case ALIGN_CENTER:
                mSrcRect.y = -((mSrcRect.h - mTextSurface->h) / 2 + mOffset[1]);
                break;
            default:
                mSrcRect.y = -mOffset[1];
        }
        
        // Texture has been updated - set boolean back to false.
        mPropertiesUpdated = false;
    }
    
    // Copy rendered text into the text box (in the renderer)
    SDL_RenderCopy(_renderer, mTextTexture, &mSrcRect, &mTextBox);
}