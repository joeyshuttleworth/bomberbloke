#ifndef STATICSPRITE_HPP
#define STATICSPRITE_HPP
#include "AbstractSpriteHandler.hpp"
#include "SDL2/SDL_image.h"
#include "engine.hpp"

extern SDL_Renderer *_renderer;
SDL_Texture* get_sprite(std::string);

class staticSprite : public AbstractSpriteHandler{
public:
  staticSprite(double xpos, double ypos, double xdim, double ydim, std::string asset_name)
    : AbstractSpriteHandler(xpos, ypos, xdim, ydim){
      mpSprite = get_sprite(asset_name);
      return;
    }

    void draw(Camera *cam){
      SDL_Rect dstrect;
      double zoom = cam->GetZoom();
      dstrect.x = round(zoom * mPosition[0]);
      dstrect.y = round((_pScene->mDimmension[1]-mPosition[1]-mDimmension[1]) * zoom);
      dstrect.w = round(zoom * mDimmension[0]);
      dstrect.h = round(zoom * mDimmension[1]);

      SDL_SetRenderTarget(_renderer, cam->getFrameBuffer());
      SDL_RenderCopy(_renderer, mpSprite, nullptr, &dstrect);

      return;
    }

    ~staticSprite(){return;}

    SDL_Texture *mpSprite;
  };
#endif
