#include "PlaceHolderSprite.hpp"
#include "Camera.hpp"

void PlaceHolderSprite::draw(Camera *cam){
   SDL_Rect dstrect;
   SDL_Color colour({0, 0xa0, 0xff, 0xff});
   double zoom = cam->GetZoom();
   dstrect.x = round(zoom * mPosition[0]);
   dstrect.y = round((_pScene->mDimmension[1]-mPosition[1]-mDimmension[1]) * zoom);
   dstrect.w = round(zoom * mDimmension[0]);
   dstrect.h = round(zoom * mDimmension[1]);
   cam->renderFillRect(&dstrect, colour);
   return;
}
