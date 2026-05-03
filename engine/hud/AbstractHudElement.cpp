#include "AbstractHudElement.hpp"

#include <SDL.h>

#include "Camera.hpp"
#include "scene.hpp"

AbstractHudElement::AbstractHudElement(
                                       scene& r_scene,
                                       int xPos,
                                       int yPos,
                                       int xDim,
                                       int yDim,
                                       AlignFlag xAlignFlag,
                                       AlignFlag yAlignFlag
                                       )
  : mrGraphicsManager(r_scene.getGraphicsManager()),
    mrSoundManager(r_scene.getSoundManager()),
    mrScene(r_scene)
{

  // Actual position is set in updatePosition
  mRelativePosition[0] = xPos;
  mRelativePosition[1] = yPos;

  mDimensions[0] = xDim;
  mDimensions[1] = yDim;

  mAlignFlags[0] = xAlignFlag;
  mAlignFlags[1] = yAlignFlag;

  mPropertiesUpdated = true;
}

void
AbstractHudElement::draw(Camera* camera)
{
  if (!mIsVisible)
    return;

  // If properties have been updated re-position HUD element.
  if (mPropertiesUpdated) {
    updatePosition(camera);
    mPropertiesUpdated = false;
  }
}

void
AbstractHudElement::updatePosition(Camera* camera)
{

  if(!camera)
    return;

  IGraphicsManager& gfx_manager = camera->getGraphicsManager();

  std::array<int, 2> screen_dimensions = gfx_manager.getScreenDimensions();

  switch (mAlignFlags[0]) {
    case ALIGN_CENTER:
      // Centred positioning
      mPosition[0] =
        (screen_dimensions[0] - mDimensions[0]) / 2 + mRelativePosition[0];
      break;
    case ALIGN_RIGHT:
      // Right-aligned positioning
      mPosition[0] =
        screen_dimensions[0] - mDimensions[0] + mRelativePosition[0];
      break;
    default:
      // Left-aligned positioning
      mPosition[0] = mRelativePosition[0];
  }

  switch (mAlignFlags[1]) {
    case ALIGN_CENTER:
      // Centred positioning
      mPosition[1] =
        (screen_dimensions[1] - mDimensions[1]) / 2 + mRelativePosition[1];
      break;
    case ALIGN_BOTTOM:
      // Right-aligned positioning
      mPosition[1] =
        screen_dimensions[1] - mDimensions[1] + mRelativePosition[1];
      break;
    default:
      // Left-aligned positioning
      mPosition[1] = mRelativePosition[1];
  }
}
