#include "engine.hpp"
#include "Explosion.hpp"

Explosion::Explosion(){
  /* Create sound objects for explosion sound effects */
  for (int i = 0; i < N_EXPLOSION_SOUNDS; i++) {
    std::shared_ptr<Sound> sound = soundManager.createSound(mExplosionSoundNames[i]);
    mExplosionSounds[i] = sound;
  }
 return;
}

void Explosion::draw(Camera *cam){

  double zoom = cam->GetZoom();

  if(!mStarted && _tick < mStartTick){
    return;
  }

  if(mRemove){
    return;
  }

  if(!mStarted){
    mStarted = true;
    if(mSound && !_server){
      /* Play explosion sound effect */
      int randIndex = std::rand() % N_EXPLOSION_SOUNDS;
      std::shared_ptr<Sound> bomb_sound = mExplosionSounds[randIndex];
      soundManager.playSound(bomb_sound);
    }
    if (mRumble) {
      _pScene->getCamera()->rumble();
    }
  }

  unsigned int frame_no = (_tick - mStartTick) % mAnimationSpeed;
  Uint8 alpha = 0xFF * (1 - (double)(_tick - mStartTick) / (2*mTimeout));
  Uint8 backAlpha = 0xFF - alpha;
  int glowAmount = mMaxGlowAmount * (1 - (_tick - mStartTick) / mTimeout);

  if(_tick - mStartTick >= mTimeout){
    mRemove = true;
    return;
  }

  /*  Set our blend mode so that our shapes blend nicely */
  SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

  SDL_Color colour;

  if(frame_no < mAnimationSpeed/2.0){
    /*Set colour to white*/
    colour = SDL_Color({0xff, 0xff, 0xff, alpha});
  }

  else{
    /*Set colour to red*/
    colour = SDL_Color({0xff, backAlpha, backAlpha, alpha});
  }
  /*  Copy our texture across to the window */
  SDL_Rect dstrect = cam->getScreenRect(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
  cam->renderFillRect(&dstrect, colour, true, glowAmount);
  return;
}
