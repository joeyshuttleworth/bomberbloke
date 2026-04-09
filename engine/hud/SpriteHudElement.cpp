#include "SpriteHudElement.hpp"

#include "engine.hpp"
#include "Camera.hpp"

SpriteHudElement::SpriteHudElement(
                                   scene &r_scene,
                                   std::string asset,
                                   int xPos,
                                   int yPos,
                                   int xDim,
                                   int yDim,
                                   AlignFlag xAlignFlag,
                                   AlignFlag yAlignFlag)
  : AbstractHudElement(r_scene, xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag)
{
  // Get sprite from asset name
  mAssetName = asset;
}

void
SpriteHudElement::draw(Camera* camera)
{
  if (!mIsVisible)
    return;

  // Base class draw function
  AbstractHudElement::draw(camera);

  // Create destination rectangle
  std::array<int, 4> dstRect;
  dstRect[0] = mPosition[0];
  dstRect[1] = mPosition[1];
  dstRect[2] = mDimensions[0];
  dstRect[3] = mDimensions[1];

  if(!camera)
    return;
  auto gfx_manager = camera->getGraphicsManager();
  if(!gfx_manager)
    return;
  // Copy sprite to destination rectangle
  gfx_manager->drawSprite(mAssetName, dstRect, mGlowAmount, camera->getFrameBuffer(mIsPostProcessed));
}
