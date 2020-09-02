#include "PlaceHolderSprite.hpp"
#include "Camera.hpp"

void PlaceHolderSprite::draw(Camera *cam){
  SDL_Color colour({0, 0xa0, 0xff, 0xff});
  SDL_Rect dstrect = cam->getScreenRect(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
  cam->renderFillRect(&dstrect, colour);
  return;
}
