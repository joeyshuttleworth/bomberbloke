#ifndef PLACEHOLDERSPIRTE_HPP
#define PLACEHOLDERSPIRTE_HPP
#include <SDL2/SDL.h>
#include "AbstractSpriteHandler.hpp"

extern unsigned int _tick;

class PlaceHolderSprite : public AbstractSpriteHandler{
public:
  using AbstractSpriteHandler :: AbstractSpriteHandler;
  void draw(Camera *cam);
};
#endif
