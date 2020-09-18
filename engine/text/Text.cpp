#include "Text.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>

#include "Camera.hpp"

const std::string CURSOR_CHAR = "|";

void Text::draw(Camera *camera, bool isPostProcessed) {
    if (mPropertiesUpdated) {
        updateTexture(camera);
        mPropertiesUpdated = false;
    }

    // Copy rendered text into the text box
    camera->renderFillRect(&mDstRect, mBackColour, isPostProcessed, mGlowAmount);
    camera->renderCopy(mTextTexture, &mSrcRect, &mDstRect, isPostProcessed, mGlowAmount);
}

void Text::updateTexture(Camera *camera) {
    int width = 0;
    int height = 0;
    if (mCursorVisible) {
        // Render text to texture
        std::string textBefore = mTextString.substr(0, mCursorIndex);
        SDL_Surface *textBeforeSurface = TTF_RenderText_Solid(mFont, textBefore.c_str(), mColour);
        if (textBeforeSurface) {
            width = textBeforeSurface->w;
            height = textBeforeSurface->h;
        }

        std::string textAfter = mTextString.substr(mCursorIndex);
        SDL_Surface *textAfterSurface = TTF_RenderText_Solid(mFont, textAfter.c_str(), mColour);
        if (textAfterSurface) {
            width += textAfterSurface->w;
            height = std::max(textAfterSurface->h, height);
        }

        SDL_Surface *textCursorSurface = TTF_RenderText_Solid(mFont, CURSOR_CHAR.c_str(), mColour);
        if (textCursorSurface) {
            height = std::max(textCursorSurface->h, height);
            width += textCursorSurface->w;
        }

        // Combine surfaces onto one text surface
        SDL_Surface *fullSurface = SDL_CreateRGBSurface(0, width, height, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
        if (textBeforeSurface) {
            SDL_BlitSurface(textBeforeSurface, nullptr, fullSurface, nullptr);
        }
        if (textAfterSurface) {
            if (textBeforeSurface) {
                SDL_Rect afterRect = SDL_Rect({textBeforeSurface->w, 0, textAfterSurface->w, textAfterSurface->h});
                SDL_BlitSurface(textAfterSurface, nullptr, fullSurface, &afterRect);
            } else {
                SDL_BlitSurface(textAfterSurface, nullptr, fullSurface, nullptr);
            }
        }
        if (textCursorSurface) {
            if (textBeforeSurface) {
                SDL_Rect cursorRect = SDL_Rect({textBeforeSurface->w, 0, textCursorSurface->w, textCursorSurface->h});
                SDL_BlitSurface(textCursorSurface, nullptr, fullSurface, &cursorRect);
            } else {
                SDL_BlitSurface(textCursorSurface, nullptr, fullSurface, nullptr);
            }
        }

        // Create texture from combined surface
        mTextTexture = SDL_CreateTextureFromSurface(_renderer, fullSurface);

        SDL_FreeSurface(textBeforeSurface);
        SDL_FreeSurface(textAfterSurface);
        SDL_FreeSurface(textCursorSurface);
        SDL_FreeSurface(fullSurface);
    } else {
        // Render text to texture
        SDL_Surface *mTextSurface = TTF_RenderText_Solid(mFont, mTextString.c_str(), mColour);
        mTextTexture = SDL_CreateTextureFromSurface(_renderer, mTextSurface);
        SDL_FreeSurface(mTextSurface);
        width = mTextSurface->w;
        height = mTextSurface->h;
    }

    // Convert alignment and texture dimensions to displacement from the
    // top-left corner of the text box
    int xDisplacement = 0;
    int yDisplacement = 0;

    // Note that mTextScale is used to scale the texture dimensions and the
    // source rectangle such that the render of the text is scaled

    // Displacement in the x-direction
    switch (mAlignment[0]) {
        case TEXT_ALIGN_RIGHT:
            xDisplacement = mDimensions[0] - width * mTextScale[0]
                + mOffset[0];
            break;
        case TEXT_ALIGN_CENTER:
            xDisplacement = (mDimensions[0] - width
                * mTextScale[0]) / 2 + mOffset[0];
            break;
        default:
            xDisplacement = mOffset[0];
    }
    // Displacement in the y-direction
    switch (mAlignment[1]) {
        case TEXT_ALIGN_BOTTOM:
            yDisplacement = mDimensions[1] - height * mTextScale[1]
                + mOffset[1];
            break;
        case TEXT_ALIGN_CENTER:
            yDisplacement = (mDimensions[1] - height
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
    mSrcRect.w = fmin(width - mSrcRect.x, mDimensions[0] / mTextScale[0]);
    mSrcRect.h = fmin(height - mSrcRect.y, mDimensions[1] / mTextScale[1]);;

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

int Text::getCursorIndex(int x) {
    int targetX = (x - mDstRect.x) * mTextScale[0];
    int totalX = 0;
    // TODO: Make this more efficient
    for (std::string::size_type i = 0; i < mTextString.size(); i++) {
        // Render character and add width to toal
        std::string charString(1, mTextString[i]);
        SDL_Surface *charSurface = TTF_RenderText_Solid(mFont, charString.c_str(), mColour);
        if (charSurface) {
            totalX += charSurface->w;

            // Check if we have reached the target x-coord
            if (totalX > targetX) {
                if ((totalX - targetX) <= charSurface->w / 2) {
                    return i;
                } else {
                    return i - 1;
                }
            }
        }
    }
    return mTextString.size();
}
