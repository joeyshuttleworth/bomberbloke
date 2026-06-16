#include "Explosion.hpp"
#include "engine.hpp"

class IGraphicsManager;

void
Explosion::draw_legacy(Camera* cam)
{
  // Simpler explosion animation used in menu and earlier Bloke releases, red+white flash
  unsigned int frame_no = (_tick - mStartTick) % mAnimationSpeed;
  Uint8 alpha = 0xFF * (1 - (double)(_tick - mStartTick) / (2 * mTimeout));
  Uint8 backAlpha = 0xFF - alpha;

  Uint32 colour = 0xffff00 ^ alpha;

  if (frame_no < mAnimationSpeed / 2.0) {
    /*Set colour to white*/
    colour = 0xffffff;
    colour = colour ^ alpha;
  }
  else {
    /*Set colour to red*/
    colour = 0xff000000 ^ (backAlpha << 16) ^ (backAlpha << 8) ^ alpha;
  }
  /*  Copy our texture across to the window */
  auto dstrect = cam->getScreenRect(mPosition[0], mPosition[1],
                                    mDimension[0], mDimension[1]);

  int glowAmount = mMaxGlowAmount * (1 - (_tick - mStartTick) / mTimeout);
  mrGraphicsManager.renderFillRect(dstrect, colour, cam->getFrameBuffer(mIsPostProcessed));

  auto bloom_colour = (colour & 0xFFFFFF00) ^ glowAmount;
  mrGraphicsManager.renderFillRect(dstrect, bloom_colour, cam->getBloomBuffer());
  return;
}

void
Explosion::draw(Camera* cam)
{
  if (!mStarted && _tick < mStartTick)
    return;
  if (mRemove)
    return;

  if (!mStarted) {
    mStarted = true;
    if (mRumble)
      cam->rumble();
  }

  if (_tick - mStartTick >= mTimeout) {
    mRemove = true;
    return;
  }
  if(mRenderLegacy) {
    this->draw_legacy(cam);
    return;
  }

  // frame_no in [0, ... , N_SPRITESHEET_SIZE - 1]
  int frame_no = (int) ( (float) N_SPRITESHEET_SIZE * ( (float) (_tick - mStartTick) / (float) mTimeout) );

  std::string asset_name = mSpriteNames[frame_no];
  auto dstrect = cam->getScreenRect(mPosition[0], mPosition[1],
                                    mDimension[0], mDimension[1]);
  mrGraphicsManager.drawSprite(
                               asset_name,
                               dstrect,
                               cam->getFrameBuffer(mIsPostProcessed)
                               );

  return;
}
