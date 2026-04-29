#ifndef PLACEHOLDERSPIRTE_HPP
#define PLACEHOLDERSPIRTE_HPP
#include <SDL.h>
#include "AbstractSpriteHandler.hpp"
#include "IGraphicsManager.hpp"

extern unsigned int _tick;

class PlaceHolderSprite : public AbstractSpriteHandler{
public:
  int getType() const{
    return SPRITE_PLACEHOLDER;
  }

  void setColour(uint32_t colour);

  using AbstractSpriteHandler::AbstractSpriteHandler;
  PlaceHolderSprite(IGraphicsManager& gfx_manager, double=0, double=0, double=0,
                    double=0, uint32_t = 0x00a0ffff);

  void draw(Camera *cam);

  virtual ~PlaceHolderSprite(){}

protected:
  Uint32 mColour = 0x0000FFFF;

};
#endif
