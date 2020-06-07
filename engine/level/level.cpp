#include "engine.h"

level :: level(){
  mDimmension[0] = 10;
  mDimmension[1] = 10;
  /*The SDL_TEXTUREACCESSTARGET flag allows us to modify this sprite*/
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,1024, 1024);
  SDL_SetRenderTarget(_renderer, mpSprite);
  SDL_SetRenderDrawColor(_renderer, 0, 0xa0, 0xFF, 0xFF);
  SDL_RenderClear(_renderer);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_RenderPresent(_renderer);
  SDL_SetRenderTarget(_renderer, NULL);
  return;
}

level :: ~level(){
  return;
}

level :: level(const level &lvl){
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, 0, 1024, 1024);
  memcpy(mpSprite, lvl.mpSprite, sizeof(SDL_Surface));
};

void level :: draw(){
  SDL_Rect rect;
  int rgb[3];

  SDL_SetRenderTarget(_renderer, NULL);
  
  if(!mpSprite){
     mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, 0, 1024, 1024);
  }
  
  SDL_RenderCopy(_renderer, mpSprite, NULL, NULL);

    //Next draw each actor
  for(auto i = mActors.begin(); i!=mActors.end(); i++){
    (*i)->draw();
  }
  return;
}
