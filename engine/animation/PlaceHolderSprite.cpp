#include "PlaceHolderSprite.hpp"
#include "Camera.hpp"

void PlaceHolderSprite::draw(Camera *cam){
   SDL_Rect dstrect;
   double zoom = cam->GetZoom();
   level* lvl  = cam->GetLevel().get();
   SDL_SetRenderDrawColor(_renderer, 0, 0xa0, 0xff, 0xff);
   dstrect.x = round(zoom * mPosition[0]);
   dstrect.y = round((lvl->mDimmension[1]-mPosition[1]-mDimmension[1]) * zoom);
   dstrect.w = round(zoom * mDimmension[0]);
   dstrect.h = round(zoom * mDimmension[1]);
   SDL_RenderFillRect(_renderer, &dstrect);
   return;
}
