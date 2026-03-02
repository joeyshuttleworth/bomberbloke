#include "Camera.hpp"
#include <algorithm>


Camera::Camera(IGraphicsManager* p_graphics_interface, scene *lvl=nullptr){

  mpGraphicsManager = p_graphics_interface;
  mpScene = lvl;

  /* Call this to get screen dimensions */
  onResize();

  mScreenRectangle.x=0;
  mScreenRectangle.y=0;

  /* Different buffers processing effects
   * mpFrameBuffer - current buffer with post processing effects
   * mpNoProcessingBuffer - current buffer with no post processing effects
   * mpBloomBuffer - buffer with blur and "addition" to create a bloom effect.
   * */

  mpScene->updateHudPositions();

  mpGraphicsInterface = graphics_interface

  init();
  return;
}

void
Camera::rumble(double amplitude, double timeout)
{
  mRumbleTimeout = timeout;
  mRumbleAmplitude = amplitude;
  return;
}

void
Camera::setBlur(double size, int passes)
{
  mBlurSize = size;
  if (passes > 0)
    mBlurPasses = passes;
}

void
Camera::setBloom(double size, int alpha, int passes)
{
  mBloomSize = size;
  mBloomAlpha = alpha;
  if (passes > 0)
    mBloomPasses = passes;
}

void
Camera::setBrightness(int brightness)
{
  mBrightness = brightness;
}

void
Camera::onResize()
{
}

void
Camera::draw()
{
  if(!mpGraphicsInterface){
    return;
  }

  LOCK_GUARD(mMutex);

  // Update the screen rectangle for applying the rumble effect
  mScreenRectangle.x = mRumbleOffset[0];
  mScreenRectangle.y = mRumbleOffset[1];

  /* Do postprocessing */
  mpGraphicsInterface->applyBloom(mBloomAlpha, mBloomSize, mBlurPasses);
  mpGraphicsInterface->applyBrightness(mBrightness);
}

void
Camera::update()
{
  /*  Rumble effect */
  const int width = mScreenRectangle.w;
  const int height = mScreenRectangle.h;
  if (mRumbleTimeout > 0) {
    mRumbleTimeout--;
    mRumbleOffset[0] =
      mRumbleAmplitude * width * std::sin((double)mRumbleTimeout / 2);
    mRumbleOffset[1] =
      mRumbleAmplitude * height * std::cos((double)mRumbleTimeout / 5);
  } else {
    mRumbleOffset[0] = 0;
    mRumbleOffset[1] = 0;
  }
  return;
}


void
Camera::SetZoom(double zoom)
{
  zoom = std::max(zoom, mMinZoom);
  zoom = std::min(zoom, mMaxZoom);
  log_message(INFO, "setting zoom to " + std::to_string(zoom));
  if (!std::isfinite(zoom))
    zoom = 1;
  mZoom = zoom;
}

