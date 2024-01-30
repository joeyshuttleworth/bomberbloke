#include "Explosion.hpp"
#include "Sound.hpp"
#include "engine.hpp"

Explosion::Explosion()
{
  if(_server)
    return;

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
  if(!mRenderLegacy) {
    for (int i = 1; i <= N_SPRITESHEET_SIZE; i++)
      mSpritesheet[i-1] =
        get_sprite("explosion_frame_" + std::to_string(i) + ".png");
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

  /*  Set our blend mode so that our shapes blend nicely */
  SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
  SDL_Color colour;

  if (frame_no < mAnimationSpeed / 2.0) {
    /*Set colour to white*/
    colour = SDL_Color({ 0xff, 0xff, 0xff, alpha });
  }
  else {
    /*Set colour to red*/
    colour = SDL_Color({ 0xff, backAlpha, backAlpha, alpha });
  }
  /*  Copy our texture across to the window */
  SDL_Rect dstrect = cam->getScreenRect(
    mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
  cam->renderFillRect(&dstrect, colour, true, glowAmount);
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
      _pScene->getCamera()->rumble();
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

  SDL_Texture *texture = mSpritesheet[frame_no];
  SDL_Rect dstrect = cam->getScreenRect(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
  cam->renderCopy(texture, nullptr, &dstrect, false, 0);

  return;
}
