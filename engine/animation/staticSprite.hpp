#ifndef STATICSPRITE_HPP
#define STATICSPRITE_HPP
#include "AbstractSpriteHandler.hpp"
#include <SDL3/SDL_image.h>
#include "engine.hpp"

extern SDL_Renderer *_renderer;
SDL_Texture* get_sprite(std::string);

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
      SDL_Rect dstrect = cam->getScreenRect(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
      cam->renderCopy(mpSprite, nullptr, &dstrect);

      return;
    }

    virtual ~staticSprite(){}

protected:
    SDL_Texture *mpSprite;
  };
#endif
