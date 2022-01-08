#include "PlaceHolderSprite.hpp"
#include "Camera.hpp"

// TODO: remove flashing stuff from abstract class to clean this clean up
PlaceHolderSprite::PlaceHolderSprite(SDL_Color colour, double x_pos=0, double y_pos=0, double x_dim=0, double y_dim=0)
: AbstractSpriteHandler(x_pos, y_pos, x_dim, y_dim, 300, 0, 0) {
  mColour = colour;
}

void
PlaceHolderSprite::draw(Camera* cam)
{
  SDL_Rect dstrect = cam->getScreenRect(
    mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
  cam->renderFillRect(&dstrect, mColour);
  return;
}

void
PlaceHolderSprite::setColour(uint32_t colour){

  mColour = {
    (uint8_t) (colour >> 24), // Red channel
    (uint8_t) (colour >> 16), // Green channel
    (uint8_t) (colour >> 8), // Blue channel
    (uint8_t) colour // Alpha channel
  };
  return;
}
