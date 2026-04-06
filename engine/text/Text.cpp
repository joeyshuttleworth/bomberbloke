#include "Text.hpp"
#include <algorithm>
#include <array>

#include "IGraphicsManager.hpp"
#include "Camera.hpp"
#include "AbstractTexture.hpp"

using rect = std::array<int, 4>;

void
Text::draw(Camera*, bool)
{
  if(!mpGraphicsManager)
    return;

  if (mPropertiesUpdated) {
    updateTexture();
    mPropertiesUpdated = false;
  }

  // Copy rendered text into the text box
  if ((mBackColour & 0xFF) > 0) {
    rect backgroundRect{mPosition[0], mPosition[1], mDimensions[0], mDimensions[1]};
    mpGraphicsManager->renderFillRect(
                                      backgroundRect, mBackColour, false, mGlowAmount);
  }

  if (mTextTexture) {
    mpGraphicsManager->renderCopy(
                                  mTextTexture, &mSrcRect, &mDstRect, false, mGlowAmount);
  }

  if (mCursorVisible) {
    std::string textBefore = mTextString.substr(0, mCursorIndex);
    int width = mpGraphicsManager->sizeText(mFont, mFontSize, textBefore)[0];
    int xCursor = mDstRect[0] + width * mTextScale[0];
    rect cursorRect{xCursor, mDstRect[1], 1, mDstRect[3]};
    mpGraphicsManager->renderFillRect(cursorRect, mColour, false, mGlowAmount);
  }
}

  void
  Text::updateTexture()
  {
    int width = 0;
    int height = 0;

    if(!mpGraphicsManager)
      return;

    if(mTextTexture && (mTextString!=""))
      mpGraphicsManager->destroyTexture(mTextTexture);
    mTextTexture =
      mpGraphicsManager->renderSolidText(mFont, mFontSize, mTextString.c_str(), mColour);

    if (mTextTexture) {
      auto dims = mTextTexture->getDimensions();
      width = dims[0];
      height = dims[1];
    } else {
      width = 0;
      height = 0;
    }


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
    mSrcRect[0] = std::max(0, -xDisplacement) / mTextScale[0];
    mSrcRect[1] = std::max(0, -yDisplacement) / mTextScale[0];

    // Crop the texture according to the dimensions of the text box and the
    // displacement
    mSrcRect[2] = std::min(double(width) - mSrcRect[0], mDimensions[0] / mTextScale[0]);
    mSrcRect[3] = std::min(double(height) - mSrcRect[1], mDimensions[1] / mTextScale[1]);

    // If displacement is positive, change the start position of the
    // destination rectangle
    mDstRect[0] = mPosition[0] + std::max(0, xDisplacement);
    mDstRect[1] = mPosition[1] + std::max(0, yDisplacement);

    // Scale the source rectangle dimensions
    mDstRect[2] = mSrcRect[2] * mTextScale[0];
    mDstRect[3] = mSrcRect[3] * mTextScale[1];

    // Texture has been updated - set boolean back to false.
    mPropertiesUpdated = false;
  }

int
Text::getCursorIndex(int x)
{
  double targetX = ((double)(x - mDstRect[0])) / mTextScale[0];
  int lastWidth = 0;

  for (std::string::size_type i = 0; i < mTextString.size(); i++) {

    std::string render_text = mTextString.substr(0, i + 1);
    int w = 0;

    auto dims = mpGraphicsManager->sizeText(mFont, mFontSize, render_text);
    w = dims[0];

    if (w > targetX) {
      // Check if the cursor is closer to the right or left of the new character
      if (2 * (w - targetX) < (w - lastWidth)) {
        return i + 1;
      } else {
        return i;
      }
    }
    lastWidth = w;
  }
  return mTextString.size();
}

Text::~Text(){
  if(mpGraphicsManager)
    mpGraphicsManager->destroyTexture(mTextTexture);
}
