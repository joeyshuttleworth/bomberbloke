#ifndef PLACEHOLDERSPIRTE_HPP
#define PLACEHOLDERSPIRTE_HPP
#include <SDL.h>
#include "AbstractSpriteHandler.hpp"

extern unsigned int _tick;

class PlaceHolderSprite : public AbstractSpriteHandler{
public:
  int getType() const{
    return SPRITE_PLACEHOLDER;
  }

  void setColour(uint32_t colour);

  PlaceHolderSprite(double=0, double=0, double=0, double=0, uint32_t = 0x00a0ffff);

  void draw(Camera *cam);

  virtual ~PlaceHolderSprite(){}

protected:
  SDL_Color mColour = {0, 0xa0, 0xff, 0xff};

};
#endif
