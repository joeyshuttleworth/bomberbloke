#include "engine.hpp"
#include "Camera.hpp"
#include <algorithm>

void Camera::rumble(double amplitude, double timeout) {
    mRumbleTimeout = timeout;
    mRumbleAmplitude = amplitude;
    return;
}

void Camera::blur(double size, int passes) {
    mBlurSize = size;
    if (passes > 0)
        mBlurPasses = passes;
}

void Camera::bloom(double size, int alpha, int passes) {
    mBloomSize = size;
    mBloomAlpha = alpha;
    if (passes > 0)
        mBloomPasses = passes;
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

    // Update the screen rectangle for applying the rumble effect
    mScreenRectangle.x = mRumbleOffset[0];
    mScreenRectangle.y = mRumbleOffset[1];

    // Apply blur to mpBloomBuffer and "add" to window to create a bloom effect
    blurTexture(mpBloomBuffer, mBloomSize, mBloomPasses);
    SDL_SetRenderTarget(_renderer, mpFrameBuffer);
    SDL_SetTextureBlendMode(mpBloomBuffer, SDL_BLENDMODE_ADD);
    SDL_SetTextureAlphaMod(mpBloomBuffer, mBloomAlpha);
    SDL_RenderCopy(_renderer, mpBloomBuffer, nullptr, nullptr);

    // Apply blur to mpFrameBuffer and draw to window
    blurTexture(mpFrameBuffer, mBlurSize, mBlurPasses);
    SDL_SetRenderTarget(_renderer, nullptr);
    SDL_RenderCopy(_renderer, mpFrameBuffer, nullptr, &mScreenRectangle);

    // Draw mpNoProcessingBuffer on window
    SDL_SetTextureBlendMode(mpNoProcessingBuffer, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(_renderer, mpNoProcessingBuffer, nullptr, nullptr);

    return;
}

void Camera::resetFrameBuffer() {
    // Set background colour
    SDL_SetRenderTarget(_renderer, mpFrameBuffer);
    SDL_SetRenderDrawColor(_renderer, 0x00, 0x10, 0xff, 0xff);
    SDL_RenderFillRect(_renderer, nullptr);

    // Clear bloom buffer
    SDL_SetRenderTarget(_renderer, mpBloomBuffer);
    SDL_SetTextureBlendMode(mpBloomBuffer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
    SDL_RenderClear(_renderer);

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

void Camera::renderCopy(SDL_Texture *texture, SDL_Rect *srcRect, SDL_Rect *dstRect, bool isPostProcessed, int bloomAmount) {
    // Copy the texture onto the appropriate frame buffer
    SDL_SetRenderTarget(_renderer, getFrameBuffer(isPostProcessed));
    SDL_RenderCopy(_renderer, texture, srcRect, dstRect);

    if (isPostProcessed) {
        // Subtract texture from bloom buffer - this creates the effect that the
        // texture is obscuring the glowing object behind it
        SDL_BlendMode subtractBlendMode = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ZERO,
            SDL_BLENDFACTOR_ONE,
            SDL_BLENDOPERATION_ADD,
            SDL_BLENDFACTOR_ZERO,
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            SDL_BLENDOPERATION_ADD);
        SDL_SetTextureBlendMode(texture, subtractBlendMode);
        SDL_SetRenderTarget(_renderer, mpBloomBuffer);
        SDL_RenderCopy(_renderer, texture, srcRect, dstRect);

        if (bloomAmount > 0) {
            // Add texture to bloom buffer to create a glowing effect
            SDL_BlendMode addBlendMode = SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_ONE,
                SDL_BLENDFACTOR_ZERO,
                SDL_BLENDOPERATION_ADD,
                SDL_BLENDFACTOR_ONE,
                SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                SDL_BLENDOPERATION_ADD);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureAlphaMod(texture, bloomAmount);
            SDL_RenderCopy(_renderer, texture, srcRect, dstRect);
            SDL_SetTextureAlphaMod(texture, 255);
        }

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }
}

void Camera::renderFillRect(SDL_Rect *dstRect, SDL_Color colour, bool isPostProcessed, int bloomAmount) {
    // Draw rect to the appropriate frame buffer
    SDL_SetRenderTarget(_renderer, getFrameBuffer(isPostProcessed));
    SDL_SetRenderDrawColor(_renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderFillRect(_renderer, dstRect);

    if (isPostProcessed) {

        if (bloomAmount > 0) {
            // Add rect to bloom buffer to create a glowing effect
            SDL_SetRenderTarget(_renderer, mpBloomBuffer);
            SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(_renderer, colour.r, colour.g, colour.b, colour.a * bloomAmount / 255);
            SDL_RenderFillRect(_renderer, dstRect);
        } else {
            // Subtract rect from bloom buffer - this creates the effect that
            // the rect is obscuring the glowing object behind it
            SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderTarget(_renderer, mpBloomBuffer);
            SDL_SetRenderDrawColor(_renderer, 0, 0, 0, colour.a);
            SDL_RenderFillRect(_renderer, dstRect);
        }

        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    }
}
