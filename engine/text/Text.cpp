#include "Text.hpp"

#include "SDL.h"
#include <algorithm>

#include "Camera.hpp"

const std::string CURSOR_CHAR = "|";

void
Text::draw(Camera* camera, bool isPostProcessed)
{
  if (mPropertiesUpdated) {
    updateTexture(camera);
    mPropertiesUpdated = false;
  }

  // Copy rendered text into the text box
  if (mBackColour.a > 0) {
    SDL_Rect backgroundRect(
      { mPosition[0], mPosition[1], mDimensions[0], mDimensions[1] });
    camera->renderFillRect(
      &backgroundRect, mBackColour, isPostProcessed, mGlowAmount);
  }

  if (mTextTexture) {
    camera->renderCopy(
      mTextTexture, &mSrcRect, &mDstRect, isPostProcessed, mGlowAmount);
  }
}

void
Text::updateTexture(Camera*)
{
  int width = 0;
  int height = 0;

  if (!mFont)
    return;


  if (mCursorVisible) {
    // Render text to texture
    std::string textBefore = mTextString.substr(0, mCursorIndex);
    SDL_Surface* textBeforeSurface =
      TTF_RenderText_Solid(mFont, textBefore.c_str(), mColour);
    if (textBeforeSurface) {
      width = textBeforeSurface->w;
      height = textBeforeSurface->h;
    }

    std::string textAfter = mTextString.substr(mCursorIndex);
    SDL_Surface* textAfterSurface =
      mFont ? TTF_RenderText_Solid(mFont, textAfter.c_str(), mColour) : nullptr;
    if (textAfterSurface) {
      width += textAfterSurface->w;
      height = std::max(textAfterSurface->h, height);
    }

    SDL_Surface* textCursorSurface =
      mFont ? TTF_RenderText_Solid(mFont, CURSOR_CHAR.c_str(), mColour)
            : nullptr;
    if (textCursorSurface) {
      height = std::max(textCursorSurface->h, height);
      width += textCursorSurface->w;
    }

    // Combine surfaces onto one text surface
    SDL_Surface* fullSurface = SDL_CreateRGBSurface(
      0, width, height, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    if (textBeforeSurface) {
      SDL_BlitSurface(textBeforeSurface, nullptr, fullSurface, nullptr);
    }
    if (textAfterSurface) {
      if (textBeforeSurface) {
        SDL_Rect afterRect = SDL_Rect({ textBeforeSurface->w,
                                        0,
                                        textAfterSurface->w,
                                        textAfterSurface->h });
        SDL_BlitSurface(textAfterSurface, nullptr, fullSurface, &afterRect);
      } else {
        SDL_BlitSurface(textAfterSurface, nullptr, fullSurface, nullptr);
      }
    }
    if (textCursorSurface) {
      if (textBeforeSurface) {
        SDL_Rect cursorRect =
          SDL_Rect({ textBeforeSurface->w - textCursorSurface->w / 2,
                     0,
                     textCursorSurface->w,
                     textCursorSurface->h });
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
    SDL_Surface* mTextSurface =
      mFont ? TTF_RenderText_Solid(mFont, mTextString.c_str(), mColour)
            : nullptr;
    if (mTextSurface) {
      mTextTexture = SDL_CreateTextureFromSurface(_renderer, mTextSurface);
      width = mTextSurface->w;
      height = mTextSurface->h;
      SDL_FreeSurface(mTextSurface);
    } else {
      mTextTexture = nullptr;
      width = 0;
      height = 0;
    }
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
      xDisplacement = mDimensions[0] - width * mTextScale[0] + mOffset[0];
      break;
    case TEXT_ALIGN_CENTER:
      xDisplacement = (mDimensions[0] - width * mTextScale[0]) / 2 + mOffset[0];
      break;
    default:
      xDisplacement = mOffset[0];
  }
  // Displacement in the y-direction
  switch (mAlignment[1]) {
    case TEXT_ALIGN_BOTTOM:
      yDisplacement = mDimensions[1] - height * mTextScale[1] + mOffset[1];
      break;
    case TEXT_ALIGN_CENTER:
      yDisplacement =
        (mDimensions[1] - height * mTextScale[1]) / 2 + mOffset[1];
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
  mSrcRect.h = fmin(height - mSrcRect.y, mDimensions[1] / mTextScale[1]);

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

int
Text::getCursorIndex(int x)
{
  double targetX = ((double)(x - mDstRect.x)) / mTextScale[0];
  std::string renderText = "";
  int lastWidth = 0;

  // TODO: Make this more efficient
  for (std::string::size_type i = 0; i < mTextString.size(); i++) {
    // Render character and add width to toal
    renderText += mTextString[i];
    SDL_Surface* substrSurface =
      mFont ? TTF_RenderText_Solid(mFont, renderText.c_str(), mColour)
            : nullptr;
    if (substrSurface && substrSurface->w > targetX) {
      // Check if the cursor is closer to the right or left of the new character
      if (2 * (substrSurface->w - targetX) < (substrSurface->w - lastWidth)) {
        return i + 1;
      } else {
        return i;
      }
      lastWidth = substrSurface->w;
    }
  }
  return mTextString.size();
}
