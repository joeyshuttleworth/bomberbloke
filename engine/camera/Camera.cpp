#include "engine.hpp"
#include "Camera.hpp"

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
    if(_pScene)
       _pScene->updateHudPositions();

    // Clear frame buffer
    if (mpFrameBuffer)
        SDL_DestroyTexture(mpFrameBuffer);
    mpFrameBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

    // TODO: Move this somewhere else
    // Set zoom value
    double min = width;
    if (height > min)
        min = height;
    if(_pScene)
      mZoom = ((double)min / _pScene->mDimmension[0]);
    else
      mZoom = 70;
    log_message(DEBUG, "Zoom level is " + std::to_string(mZoom));
    return;
}


void Camera::draw() {
    update();
    mScreenRectangle.x = mRumbleOffset[0];
    mScreenRectangle.y = mRumbleOffset[1];
    SDL_SetRenderTarget(_renderer, nullptr);
    SDL_RenderCopy(_renderer, mpFrameBuffer, nullptr, &mScreenRectangle);
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
