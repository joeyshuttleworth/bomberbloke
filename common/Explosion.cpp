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
  Uint8 alpha = 0xFF * (1 - (double)(_tick - mStartTick) / (2*mTimeout));
  Uint8 backAlpha = 0xFF - alpha;
  int glowAmount = mMaxGlowAMount * (1 - (_tick - mStartTick) / mTimeout);

  /*  Set our blend mode so that our shapes blend nicely */
  SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

  SDL_Color colour;

  if(frame_no < mAnimationSpeed/2){
    /*Set colour to white*/
    colour = SDL_Color({0xff, 0xff, 0xff, alpha});
  }

  else{
    /*Set colour to red*/
    colour = SDL_Color({0xff, backAlpha, backAlpha, alpha});
  }
  /*  Copy our texture across to the window */
  SDL_Rect dstrect;
  dstrect.x = round(zoom * mPosition[0]);
  dstrect.y = round((_pScene->mDimmension[1]-mPosition[1]-mDimmension[1]) * zoom);
  dstrect.w = round(zoom * mDimmension[0]);
  dstrect.h = round(zoom * mDimmension[1]);
  cam->renderFillRect(&dstrect, colour, true, glowAmount);
  return;
}
