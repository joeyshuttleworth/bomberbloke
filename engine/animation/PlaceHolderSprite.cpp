#include "PlaceHolderSprite.hpp"
#include "Camera.hpp"

PlaceHolderSprite::PlaceHolderSprite(IGraphicsManager* gfx_manager, double x_pos, double y_pos, double x_dim, double y_dim, uint32_t colour)
  : AbstractSpriteHandler(gfx_manager, x_pos, y_pos, x_dim, y_dim, 300, 0, 0) {
  setColour(colour);
}

void
PlaceHolderSprite::draw(Camera* cam)
{
  if(!mpGraphicsManager)
    return;

  auto dstrect = cam->getScreenRect(
                                    mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
  mpGraphicsManager->renderFillRect(dstrect, mColour, 0, cam->getFrameBuffer(mIsPostProcessed));
  return;
}

void
PlaceHolderSprite::setColour(uint32_t colour){
  mColour = colour;
  return;
}
