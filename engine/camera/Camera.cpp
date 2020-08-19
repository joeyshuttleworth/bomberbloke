#include "engine.hpp"
#include "Camera.hpp"

void Camera::rumble(double amplitude, double timeout){
    mRumbleTimeout = timeout;
    mRumbleAmplitude = amplitude;
    return;
}

void Camera::onResize() {
    // Get window size
    if (_window){
        SDL_GetWindowSize(_window, &mWidth, &mHeight);
    } else {
        mWidth = DEFAULT_WINDOW_HEIGHT;
        mHeight = DEFAULT_WINDOW_WIDTH;
    }
    
    // Update HUD positions
    mpScene->updateHudPositions(this);
    
    // Clear frame buffer
    if (mpFrameBuffer)
        SDL_DestroyTexture(mpFrameBuffer);
    mpFrameBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, mWidth, mHeight);

    // TODO: Move this somewhere else
    // Set zoom value
    double min = mWidth;
    if (mHeight > min)
        min = mHeight;
    mZoom = ((double)min / mpScene->mDimmension[0]);
    log_message(DEBUG, "Zoom scene is " + std::to_string(mZoom));
    return;
}

void Camera::draw() {
    update();
    mScreenRectangle.x = mRumbleOffset[0];
    mScreenRectangle.y = mRumbleOffset[1];
    SDL_SetRenderTarget(_renderer, mpFrameBuffer);
    mpScene->draw(this);
    SDL_SetRenderTarget(_renderer, nullptr);
    SDL_RenderCopy(_renderer, mpFrameBuffer, nullptr, &mScreenRectangle);
    
    return;
}

void Camera::update() {
    /*  Rumble effect */
    if(mRumbleTimeout > 0){
        mRumbleTimeout--;
        mRumbleOffset[0] = mRumbleAmplitude*mWidth*std::sin((double)mRumbleTimeout/2);
        mRumbleOffset[1] = mRumbleAmplitude*mHeight*std::cos((double)mRumbleTimeout/5);
    } else {
        mRumbleOffset[0] = 0;
        mRumbleOffset[1] = 0;
    }
    return;
}