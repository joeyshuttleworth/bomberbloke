#include "engine.hpp"
#include "Camera.hpp"
#include <algorithm>

void Camera::rumble(double amplitude, double timeout){
    mRumbleTimeout = timeout;
    mRumbleAmplitude = amplitude;
    return;
}

void Camera::blur(double size, int passes){
    mBlurSize = size;
    mBlurPasses = passes;
}

void Camera::onResize() {
    // Get window size
    int width, height;
    if (_window)
        SDL_GetWindowSize(_window, &width, &height);
    else {
        width = DEFAULT_WINDOW_HEIGHT;
        height = DEFAULT_WINDOW_WIDTH;
    }

    mScreenRectangle.w = width;
    mScreenRectangle.h = height;

    // Update HUD positions
    if(mpScene)
       mpScene->updateHudPositions();

    // Clear frame buffer
    if (mpFrameBuffer)
        SDL_DestroyTexture(mpFrameBuffer);
    mpFrameBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

    const double min = std::min(width, height);

    if(mpScene)
      mZoom = ((double)min / mpScene->mDimmension[0]);
    else
      mZoom = 70;
    log_message(DEBUG, "Zoom level is " + std::to_string(mZoom));
    return;
}

void Camera::draw() {
    update();

    blurTexture(mpFrameBuffer, mBlurSize, mBlurPasses);

    SDL_SetRenderTarget(_renderer, nullptr);
    mScreenRectangle.x = mRumbleOffset[0];
    mScreenRectangle.y = mRumbleOffset[1];
    SDL_RenderCopy(_renderer, mpFrameBuffer, nullptr, &mScreenRectangle);
    SDL_SetTextureBlendMode(mpNoProcessingBuffer, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(_renderer, mpNoProcessingBuffer, nullptr, nullptr);

    return;
}

void Camera::resetFrameBuffer() {
    // Set background colour
    SDL_SetRenderTarget(_renderer, mpFrameBuffer);
    SDL_SetRenderDrawColor(_renderer, 0x00, 0x10, 0xff, 0xff);
    SDL_RenderFillRect(_renderer, nullptr);

    // Clear no-processing-buffer
    SDL_SetRenderTarget(_renderer, mpNoProcessingBuffer);
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
    SDL_RenderClear(_renderer);
}

void Camera::update() {
    /*  Rumble effect */
    const int width = mScreenRectangle.w;
    const int height = mScreenRectangle.h;
    if(mRumbleTimeout > 0){
        mRumbleTimeout--;
        mRumbleOffset[0] = mRumbleAmplitude*width*std::sin((double)mRumbleTimeout/2);
        mRumbleOffset[1] = mRumbleAmplitude*height*std::cos((double)mRumbleTimeout/5);
    } else {
        mRumbleOffset[0] = 0;
        mRumbleOffset[1] = 0;
    }
    return;
}

void Camera::blurTexture(SDL_Texture *texture, double size, int passes) {
    if (size <= 0)
        return;

    // Get width and height of texture
    int width, height;
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

    // Create temporary texture for blurring passes
    SDL_Texture *tmpTexture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, width, height);

    SDL_SetRenderTarget(_renderer, tmpTexture);
    SDL_SetTextureBlendMode(tmpTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);

    // Copy tmpTexture back onto texture
    SDL_SetTextureBlendMode(tmpTexture, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(_renderer, tmpTexture);
    SDL_SetTextureAlphaMod(texture, 255);
    SDL_RenderCopy(_renderer, texture, nullptr, nullptr);

    // Set appropriate blend modes
    SDL_SetTextureBlendMode(tmpTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Perform multiple passes of blurring
    SDL_Rect dstRect({0, 0, width, height});
    for (int i = 0; i < passes; i++) {
        // Gradually reduce offset size
        int offset = (int) fmax(size - i * size / passes, 1);

        // Alternate the direction of the offset to prevent the blur shifting
        // the image
        if (i % 2 == 0) {
            offset *= -1;
        }

        // Copy texture onto tmpTexture shifted horizontally with 50% alpha
        dstRect.x = offset;
        dstRect.y = 0;
        SDL_SetRenderTarget(_renderer, tmpTexture);
        SDL_SetTextureAlphaMod(texture, 127);
        SDL_RenderCopy(_renderer, texture, nullptr, &dstRect);

        // Copy tmpTexture back onto texture
        SDL_SetRenderTarget(_renderer, texture);
        SDL_SetTextureAlphaMod(texture, 255);
        SDL_RenderCopy(_renderer, tmpTexture, nullptr, nullptr);

        // Copy texture onto tmpTexture shifted vertically with 50% alpha
        dstRect.x = 0;
        dstRect.y = offset;
        SDL_SetRenderTarget(_renderer, tmpTexture);
        SDL_SetTextureAlphaMod(texture, 127);
        SDL_RenderCopy(_renderer, texture, nullptr, &dstRect);

        // Copy tmpTexture back onto texture
        SDL_SetRenderTarget(_renderer, texture);
        SDL_SetTextureAlphaMod(texture, 255);
        SDL_RenderCopy(_renderer, tmpTexture, nullptr, nullptr);
    }

    SDL_DestroyTexture(tmpTexture);
}
