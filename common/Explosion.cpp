#include "engine.hpp"
#include "Explosion.hpp"

void Explosion::draw(Camera *cam){

  double zoom = cam->GetZoom();

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
  /*  Copy our texture across to the window */
  SDL_Rect dstrect;
  dstrect.x = round(zoom * mPosition[0]);
  dstrect.y = round((_pScene->mDimmension[1]-mPosition[1]-mDimmension[1]) * zoom);
  dstrect.w = round(zoom * mDimmension[0]);
  dstrect.h = round(zoom * mDimmension[1]);
  SDL_RenderFillRect(_renderer, &dstrect);
  
  mBombText->draw(cam);
  return;
}
