#include "Explosion.hpp"
#include "Sound.hpp"
#include "engine.hpp"

class IGraphicsManager;

void Explosion::initSounds(){
  /* Create sound objects for explosion sound effects */

  if(mSound) {
    for (int i = 0; i < N_EXPLOSION_SOUNDS; i++) {
      std::unique_ptr<Sound> sound =
        mrSoundManager.createSound(mExplosionSoundNames[i]);
      mExplosionSounds[i] = std::move(sound);
      if(sound)
        sound->setGroup(SOUND_FX);
    }
  }
}

void
Explosion::draw_legacy(Camera* cam)
{
  // Simpler explosion animation used in menu and earlier Bloke releases, red+white flash
  unsigned int frame_no = (_tick - mStartTick) % mAnimationSpeed;
  Uint8 alpha = 0xFF * (1 - (double)(_tick - mStartTick) / (2 * mTimeout));
  Uint8 backAlpha = 0xFF - alpha;
  int glowAmount = mMaxGlowAmount * (1 - (_tick - mStartTick) / mTimeout);

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

  mrGraphicsManager.renderFillRect(dstrect, colour, glowAmount, cam->getFrameBuffer(mIsPostProcessed));
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
    if (mSound && !_server) {
      /* Play explosion sound effect */
      int randIndex = std::rand() % N_EXPLOSION_SOUNDS;
      auto bomb_sound = mExplosionSounds[randIndex].get();

      if(bomb_sound)
        mrSoundManager.playSound(bomb_sound);
    }
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
                               0,
                               cam->getFrameBuffer(mIsPostProcessed)
                               );

  return;
}
