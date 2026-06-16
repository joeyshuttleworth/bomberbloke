#include "engine.hpp"
#include "Camera.hpp"
#include <algorithm>


Camera::Camera(IGraphicsManager& r_graphics_manager, scene *lvl) :
  AbstractCamera(r_graphics_manager, lvl)
{

  /* Call this to get screen dimensions */
  onResize();

  /* Different buffers processing effects
   * mpFrameBuffer - current buffer with post processing effects
   * mpNoProcessingBuffer - current buffer with no post processing effects
   * mpBloomBuffer - buffer with blur and "addition" to create a bloom effect.
   * */

  auto dims = mrGraphicsManager.getScreenDimensions();
  int w = dims[0], h = dims[1];

  mpFrameBuffer = mrGraphicsManager.createTexture(w, h);
  mpNoProcessingBuffer = mrGraphicsManager.createTexture(w, h);
  mpBloomBuffer = mrGraphicsManager.createTexture(w, h);

  init();
  return;
}

std::array<int, 4>
Camera::getScreenRect(double x, double y, double w, double h)
{
  std::array<int, 2> screen_dims = {0, 0};
  screen_dims = mrGraphicsManager.getScreenDimensions();

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
  auto dims = getScreenDimensions();

  mrGraphicsManager.destroyTexture(mpFrameBuffer);
  mpFrameBuffer = mrGraphicsManager.createTexture(dims[0], dims[1]);

  mrGraphicsManager.destroyTexture(mpNoProcessingBuffer);
  mpNoProcessingBuffer = mrGraphicsManager.createTexture(dims[0], dims[1]);
}

void
Camera::draw()
{
  LOCK_GUARD(mMutex);

  // /* Apply postprocessing to everything in mpFrameBuffer*/
  mrGraphicsManager.applyBloom(mBloomAlpha, mBloomSize, mBloomPasses, mpBloomBuffer,
                               nullptr, mpBloomBuffer);
  mrGraphicsManager.renderCopy(mpBloomBuffer, nullptr, nullptr, mpFrameBuffer);
  mrGraphicsManager.applyBlur(mBlurSize, mBlurPasses, mpFrameBuffer);
  mrGraphicsManager.setBrightness(mBrightness);

  auto dims = getScreenDimensions();
  std::array<int, 4> screen_rect = {0, 0, dims[0], dims[1]};

  screen_rect[0] += mRumbleOffset[0];
  screen_rect[1] += mRumbleOffset[1];

  mrGraphicsManager.renderCopy(mpFrameBuffer, nullptr, &screen_rect,
                                0);

  mrGraphicsManager.renderCopy(mpNoProcessingBuffer, nullptr, nullptr,
                                0);
}

void Camera::resetFrameBuffers(){
  mrGraphicsManager.renderClear(mpFrameBuffer);
  mrGraphicsManager.renderClear(mpNoProcessingBuffer);
  mrGraphicsManager.renderClear(mpBloomBuffer);

  // If we manage our own blurbuffers, they should be cleared
  // for(auto i : mpBlurBuffers)
  //   mrGraphicsManager.renderClear(i);

  mrGraphicsManager.resetFrameBuffers();

}

void
Camera::update()
{
  /*  Rumble effect */

  std::array<int, 2> screen_dims = {0, 0};

  screen_dims = mrGraphicsManager.getScreenDimensions();

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
