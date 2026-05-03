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

  PlaceHolderSprite(PlaceHolderSprite& other) :
    AbstractSpriteHandler(other),
    mColour(other.mColour)
  {
  }

  PlaceHolderSprite(PlaceHolderSprite& other, IGraphicsManager& gfx) :
    AbstractSpriteHandler(other, gfx),
    mColour(other.mColour)
  {
  }

  void draw(Camera *cam) override;

  virtual ~PlaceHolderSprite(){}

  std::shared_ptr<AbstractSpriteHandler> clone(IGraphicsManager& gfx){
    return std::make_shared<PlaceHolderSprite>(*this, gfx);
  };


protected:
  Uint32 mColour = 0x0000FFFF;

};
#endif
