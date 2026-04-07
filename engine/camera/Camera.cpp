#include "engine.hpp"
#include "Camera.hpp"
#include <algorithm>


Camera::Camera(IGraphicsManager* p_graphics_manager, scene *lvl){

  mpGraphicsManager = p_graphics_manager;
  mpScene = lvl;

  /* Call this to get screen dimensions */
  onResize();

  /* Different buffers processing effects
   * mpFrameBuffer - current buffer with post processing effects
   * mpNoProcessingBuffer - current buffer with no post processing effects
   * mpBloomBuffer - buffer with blur and "addition" to create a bloom effect.
   * */

  mpScene->updateHudPositions();

  auto dims = mpGraphicsManager->getScreenDimensions();
  int w = dims[0], h = dims[1];

  mpFrameBuffer = mpGraphicsManager->createTexture(w, h);
  mpNoProcessingBuffer = mpGraphicsManager->createTexture(w, h);

  init();
  return;
}

std::array<int, 4>
Camera::getScreenRect(double x, double y, double w, double h)
{
  std::array<int, 2> screen_dims = {0, 0};
  if(mpGraphicsManager)
    screen_dims = mpGraphicsManager->getScreenDimensions();

  int pxPerUnit = mZoom * screen_dims[0];

  std::array<int, 4> screen_rect;
  screen_rect[0] = (x - mPosition[0]) * pxPerUnit + screen_dims[0] / 2;
  screen_rect[1] = -(y + h - mPosition[1]) * pxPerUnit + screen_dims[1] / 2;
  screen_rect[2] = w * pxPerUnit;
  screen_rect[3] = h * pxPerUnit;

  return screen_rect;
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
  if(!mpGraphicsManager){
    return;
  }

  LOCK_GUARD(mMutex);

  /* Apply postprocessing to everything in mpFrameBuffer*/
  mpGraphicsManager->applyBloom(mBloomAlpha, mBloomSize, mBloomPasses, mpFrameBuffer);
  mpGraphicsManager->applyBlur(mBlurSize, mBlurPasses, mpFrameBuffer);
  mpGraphicsManager->setBrightness(mBrightness);
}

void Camera::applyBloom(int alpha, int size, int passes){
  mpGraphicsManager->applyBloom(alpha, size, passes);
}

void Camera::applyBlur(double blur_size, int passes){
  mpGraphicsManager->applyBlur(blur_size, passes);
}

void
Camera::update()
{
  /*  Rumble effect */

  std::array<int, 2> screen_dims = {0, 0};

  if(mpGraphicsManager){
    screen_dims = mpGraphicsManager->getScreenDimensions();
  }

  const int width = screen_dims[0];
  const int height = screen_dims[1];

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
Camera::setZoom(double zoom)
{
  zoom = std::max(zoom, mMinZoom);
  zoom = std::min(zoom, mMaxZoom);
  log_message(INFO, "setting zoom to " + std::to_string(zoom));
  if (!std::isfinite(zoom))
    zoom = 1;
  mZoom = zoom;
}

