#include "Explosion.hpp"

void Explosion::draw(){
  if(mRemove){
    return;
  }

  if(mDelay > 0){
    mDelay--;
    return;
  }

  if(_tick - mStartTick >= mTimeout){
    mRemove = true;
    return;
  }

  unsigned int frame_no = (_tick - mStartTick) % mAnimationSpeed;
  unsigned int alpha = 0xFF * (1 - (double)(_tick - mStartTick) / (2*mTimeout));

  /*  Set our blend mode so that our shapes blend nicely */
  SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

  if(frame_no < mAnimationSpeed/2){
    /*Set colour to white*/
    SDL_SetRenderDrawColor(_renderer, 0xff, 0xff, 0xff, alpha);
  }

  else{
    /*Set colour to red*/
    SDL_SetRenderDrawColor(_renderer, 0xff, 1 - alpha, 1 - alpha, alpha);
  }

  /*  Set renderer target back to the window */
  SDL_SetRenderTarget(_renderer, nullptr);
  /*  Copy our texture across to the window */
  SDL_Rect dstrect;
  dstrect.x = round(_zoom * mPosition[0]);
  dstrect.y = round((_level.mDimmension[1]-mPosition[1]-mDimmension[1]) * _zoom);
  dstrect.w = round(_zoom * mDimmension[0]);
  dstrect.h = round(_zoom * mDimmension[1]);
  SDL_RenderFillRect(_renderer, &dstrect);
  return;
}
