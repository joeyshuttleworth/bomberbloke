#ifndef PLACEHOLDERSPIRTE_HPP
#define PLACEHOLDERSPIRTE_HPP
#include <SDL2/SDL.h>
#include "AbstractSpriteHandler.hpp"

extern unsigned int _tick;

class PlaceHolderSprite : public AbstractSpriteHandler{
public:
  SDL_Color mColour;

  int getType() const{
    return SPRITE_PLACEHOLDER;
  }
  PlaceHolderSprite(SDL_Color, double, double, double, double);
  void draw(Camera *cam);

  virtual ~PlaceHolderSprite(){}
};
#endif
