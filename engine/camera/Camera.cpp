#include "engine.hpp"
#include "Camera.hpp"
#include <algorithm>

void Camera::rumble(double amplitude, double timeout){
    mRumbleTimeout = timeout;
    mRumbleAmplitude = amplitude;
    return;
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

    SDL_SetRenderTarget(_renderer, nullptr);
    mScreenRectangle.x = mRumbleOffset[0];
    mScreenRectangle.y = mRumbleOffset[1];
    SDL_SetRenderTarget(_renderer, nullptr);
    SDL_RenderCopy(_renderer, mpFrameBuffer, nullptr, &mScreenRectangle);
    SDL_SetTextureBlendMode(mpNoProcessingBuffer, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(_renderer, mpNoProcessingBuffer, nullptr, nullptr);

    return;
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
