#ifndef EXPLOSION_HPP
#define EXPLOSION_HPP

#include "AbstractSpriteHandler.hpp"

#define EXPLOSION_SPRITE_SIZE 1000

class Explosion : public AbstractSpriteHandler{
public:

  using AbstractSpriteHandler::AbstractSpriteHandler;

  Explosion(){
    return;
  }

  /*  Use the default constructor for everything except creating the texture */
  Explosion(double x_pos, double y_pos, double x_dim, double y_dim, int speed = 60, int timeout = 600)
    :AbstractSpriteHandler(x_pos, y_pos, x_dim, y_dim, speed, timeout){
    ReloadSprite();
    return;
  }

  /*  In draw() we program a simple flash between red and white.
  */

  void draw();

  ~Explosion(){
    if(mpSprite){
      SDL_DestroyTexture(mpSprite);
    }
  };

void ReloadSprite(){
  if(mpSprite){
    SDL_DestroyTexture(mpSprite);
  }
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, EXPLOSION_SPRITE_SIZE, EXPLOSION_SPRITE_SIZE);
}


protected:
  int mCurrentColour;
};

#endif
