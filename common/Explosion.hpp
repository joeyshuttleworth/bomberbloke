#ifndef EXPLOSION_HPP
#define EXPLOSION_HPP

#include "AbstractSpriteHandler.hpp"

#define EXPLOSION_SPRITE_SIZE 1000

class Explosion : public AbstractSpriteHandler{
public:

  Explosion(){
    return;
  }

  /*  Use the default constructor for everything except creating the texture */
  Explosion(double x_pos, double y_pos, double x_dim, double y_dim, int speed = 30, int timeout = 60)
    :AbstractSpriteHandler(x_pos, y_pos, x_dim, y_dim, speed, timeout){
    return;
  }

  /*  In draw() we program a simple flash between red and white. */

  void draw(Camera*);

protected:
};

#endif
