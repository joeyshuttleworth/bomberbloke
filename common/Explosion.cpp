#include "Explosion.hpp"

void Explosion::draw(){
  if(mRemove){
    return;
  }

  if(_tick - mStartTick > mTimeout){
    mRemove = true;
    return;
  }

  unsigned int frame_no = (_tick - mStartTick) % mAnimationSpeed;
  if(!mpSprite){
    log_message(ERROR, "Explosion: missing sprite!");
    return;
  }
  /*  Make background transparent (should really only do this when we need to) */
  SDL_SetRenderTarget(_renderer, mpSprite);
  /*  Make background transparent */
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
  SDL_RenderFillRect(_renderer, nullptr);

  if(frame_no < mAnimationSpeed/2){
    /*Set colour to white*/
    SDL_SetRenderDrawColor(_renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderFillRect(_renderer, nullptr);
  }

  else{
    /*Set colour to red*/
    SDL_SetRenderDrawColor(_renderer, 0xff, 0, 0, 0xff);
    SDL_RenderFillRect(_renderer, nullptr);
  }

  /*  Set renderer target back to the window */
  SDL_SetRenderTarget(_renderer, nullptr);
  SDL_SetRenderDrawColor(_renderer, 0x10, 0x10, 0x02, 0xff);

  /*  Copy our texture across to the window */
  SDL_Rect dstrect;
  dstrect.x = round(_zoom * mPosition[0]);
  dstrect.y = round(_zoom * mPosition[1]);
  dstrect.w = round(_zoom * mDimmensions[0]);
  dstrect.h = round (_zoom *mDimmensions[1]);
  SDL_RenderCopy(_renderer, mpSprite, nullptr, &dstrect);

  return;
}
