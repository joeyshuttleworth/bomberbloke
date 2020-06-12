#ifndef ABSTRACTSPRITEHANDLER_HPP
#define ABSTRACTSPRITEHANDLER_HPP
#include <SDL2/SDL.h>
#include "engine.hpp"

class AbstractSpriteHandler{
public:

  AbstractSpriteHandler(){
    return;
  }

  virtual void draw() = 0;

 AbstractSpriteHandler(double x_pos, double y_pos, double x_dim, double y_dim, int speed = 300, int timeout = 600){
   mPosition[0] = x_pos;
   mPosition[1] = y_pos;
   mDimmensions[0] = x_dim;
   mDimmensions[1] = y_dim;
   mStartTick = _tick;
   mAnimationSpeed = speed;
   mTimeout = timeout;
   mRemove = false;
   /*  This sprite is probably too big */
   return;
  }

  virtual void ReloadSprite() = 0;

  ~AbstractSpriteHandler(){
    if(mpSprite){
      SDL_DestroyTexture(mpSprite);
    }
    return;
  }
  bool mRemove;
protected:
  SDL_Texture *mpSprite = nullptr;
  double mPosition[2];
  double mDimmensions[2];
  unsigned int mStartTick;
  unsigned int mAnimationSpeed;
  unsigned int mTimeout;
};

extern std::list<std::shared_ptr<AbstractSpriteHandler>> _particle_list;
#endif
