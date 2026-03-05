#ifndef STATICSPRITE_HPP
#define STATICSPRITE_HPP
#include "AbstractSpriteHandler.hpp"
#include "IGraphicsManager.hpp"
#include <SDL_image.h>

class staticSprite : public AbstractSpriteHandler{
protected:
  std::string mAssetName;
public:
  int getType() const{
    return SPRITE_STATIC;
  }
  staticSprite(IGraphicsManager* gfx_manager, double xpos, double ypos, double xdim, double ydim, std::string asset_name)
    : AbstractSpriteHandler(gfx_manager, xpos, ypos, xdim, ydim){
    mAssetName = asset_name;
    return;
  }

    void draw(Camera* cam){
      if(mpGraphicsManager){
        auto dstrect = cam->getScreenRect(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
        mpGraphicsManager->drawSprite(mAssetName, dstrect);
      }
      return;
    }

    virtual ~staticSprite(){}

  };
#endif
