#include "Explosion.hpp"
#include "Sound.hpp"
#include "engine.hpp"

class IGraphicsManager;

Explosion::Explosion(IGraphicsManager* gfx_manager)
{
  if(_server)
    return;

  mpGraphicsManager = gfx_manager;

  /* Create sound objects for explosion sound effects */
  if(mSound) {
    for (int i = 0; i < N_EXPLOSION_SOUNDS; i++) {
      std::shared_ptr<Sound> sound =
        soundManager.createSound(mExplosionSoundNames[i]);
      sound->mGroup = SOUND_FX;
      mExplosionSounds[i] = sound;
    }
  }

  /* We need to tell the BLOKE engine to get textures ready if we need them */
  /* TODO fix or remove */
  if(!mRenderLegacy) {
    for (int i = 1; i <= N_SPRITESHEET_SIZE; i++)
      mSpriteNames[i-1] = "explosion_frame_" + std::to_string(i) + ".png";
  }

  return;
}

void
Explosion::draw_legacy(Camera* cam)
{
  // Simpler explosion animation used in menu and earlier Bloke releases, red+white flash
  unsigned int frame_no = (_tick - mStartTick) % mAnimationSpeed;
  Uint8 alpha = 0xFF * (1 - (double)(_tick - mStartTick) / (2 * mTimeout));
  Uint8 backAlpha = 0xFF - alpha;
  int glowAmount = mMaxGlowAmount * (1 - (_tick - mStartTick) / mTimeout);

  /*  Do we need to set blend mode i.e.: */
  // SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

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
  if(mpGraphicsManager){
    auto dstrect = cam->getScreenRect(mPosition[0], mPosition[1],
                                                    mDimmension[0], mDimmension[1]);

    mpGraphicsManager->renderFillRect(dstrect, colour, true, glowAmount, cam->getFrameBuffer(mIsPostProcessed));
  }
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
      std::shared_ptr<Sound> bomb_sound = mExplosionSounds[randIndex];
      soundManager.playSound(bomb_sound);
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
                                    mDimmension[0], mDimmension[1]);
  if(mpGraphicsManager)
    mpGraphicsManager->drawSprite(
                                  asset_name,
                                  dstrect
                                  );

  return;
}
