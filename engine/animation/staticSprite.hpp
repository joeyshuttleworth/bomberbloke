#ifndef STATICSPRITE_HPP
#define STATICSPRITE_HPP
#include "AbstractSpriteHandler.hpp"
#include <SDL_image.h>

class staticSprite : public AbstractSpriteHandler{
public:
  int getType() const{
    return SPRITE_STATIC;
  }
  staticSprite(double xpos, double ypos, double xdim, double ydim, std::string asset_name)
    : AbstractSpriteHandler(xpos, ypos, xdim, ydim){
      mpSprite = get_sprite(asset_name);
      return;
    }

    void draw(Camera *cam){
      if(mpGraphicsManager){
        auto dstrect = mpGraphicsManager->getScreenRect(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
        mpGraphicsManager->renderCopy(mpSprite, nullptr, &dstrect);
      }
      return;
    }

    virtual ~staticSprite(){}

protected:
    SDL_Texture *mpSprite;
  };
#endif
