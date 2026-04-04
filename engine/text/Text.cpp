#include "Text.hpp"
#include <algorithm>
#include <array>

#include "IGraphicsManager.hpp"
#include "Camera.hpp"
#include "AbstractTexture.hpp"

const std::string CURSOR_CHAR = "|";

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
}

  void
  Text::updateTexture()
  {
    int width = 0;
    int height = 0;

    if(!mpGraphicsManager)
      return;

    if (mCursorVisible) {
      // Render text to texture
      std::string textBefore = mTextString.substr(0, mCursorIndex);
      AbstractTexture* textBeforeTexture =
        mpGraphicsManager->renderSolidText(mFont, mFontSize, textBefore, mColour,
                                           mTextTexture);
      if (textBeforeTexture) {
        auto dims = textBeforeTexture->getDimensions();
        int w = dims[0];
        int h = dims[1];
        width = w;
        height = h;
      }

      std::string textAfter = mTextString.substr(mCursorIndex);
      AbstractTexture* textAfterTexture =
        mpGraphicsManager->renderSolidText(mFont, mFontSize, textAfter, mColour, mTextTexture);
      if (textAfterTexture) {
        auto dims = textAfterTexture->getDimensions();
        int w = dims[0];
        int h = dims[1];
        height = std::max(h, height);
        width += w;
      }

      AbstractTexture* textCursorTexture =
        mpGraphicsManager->renderSolidText(mFont, mFontSize, CURSOR_CHAR, mColour);
      if (textCursorTexture) {
        auto dims = textAfterTexture->getDimensions();
        int w = dims[0];
        int h = dims[1];

        height = std::max(h, height);
        width += w;
      }

      // Create a render target texture
      auto fullTexture = mpGraphicsManager->createTexture(
                                                          width,
                                                          height
                                                          );

      if (textBeforeTexture) {
        auto dims = textBeforeTexture->getDimensions();
        auto w = dims[0];
        auto h = dims[1];
        std::array<int, 4> dst = {0, 0, w, h};
        mpGraphicsManager->renderCopy(textBeforeTexture, nullptr, &dst, false, 0,
                                      fullTexture);
      }

      if (textAfterTexture) {
        auto dims = textAfterTexture->getDimensions();
        auto w = dims[0];
        auto h = dims[1];
        std::array<int, 4> dst = {0, 0, w, h};

        if (textBeforeTexture) {
          auto dims = textBeforeTexture->getDimensions();
          auto beforeW = dims[0];
          dst[0] = beforeW;
        }

        mpGraphicsManager->renderCopy(textAfterTexture, nullptr, &dst, false, 0,
                                      fullTexture);
      }

      if (textCursorTexture) {

        auto dims = textCursorTexture->getDimensions();
        auto w = dims[0];
        auto h = dims[1];
        std::array<int, 4> dst = {0, 0, w, h};

        if (textBeforeTexture) {
          auto dims = textBeforeTexture->getDimensions();
          auto beforeW = dims[0];
          dst[0] = beforeW - dst[2] / 2;
        }

        mpGraphicsManager->renderCopy(textCursorTexture, nullptr, &dst, false, 0,
                                      fullTexture);
        if(mTextTexture)
          mpGraphicsManager->destroyTexture(mTextTexture);
        // Store final texture
        mTextTexture = fullTexture;
      }
    }

    else {
      if(mTextTexture)
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
