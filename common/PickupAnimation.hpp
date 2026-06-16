#ifndef PICKUPANIMATION_HPP
#define PICKUPANIMATION_HPP

#include "AbstractPickup.hpp"
#include "bomberbloke_actors.hpp"
#include "staticSprite.hpp"
#include "IGraphicsManager.hpp"
#include <string>

class PickupAnimation : public AbstractSpriteHandler{
private:
  std::string mAssetName = "";
public:
  int getType() const{return AbstractPickup::PICKUP_NONE;}

  PickupAnimation() = default;

  using AbstractSpriteHandler::AbstractSpriteHandler;

  PickupAnimation(IGraphicsManager& gfx_manager, double xpos, double ypos, double xdim, double ydim,
                  const std::string& asset_name)
    : AbstractSpriteHandler(gfx_manager, xpos, ypos, xdim, ydim),
      mAssetName(asset_name)
  {
  }

  void draw(Camera* cam){
    // int bloom = std::abs((int)(_tick - mStartTick) % (2*50) - 50);
    // TODO bloom
    auto dstrect = cam->getScreenRect(mPosition[0], mPosition[1], mDimension[0], mDimension[1]);

    mrGraphicsManager.drawSprite(mAssetName, dstrect, cam->getFrameBuffer(mIsPostProcessed));
    return;
  }

  virtual ~PickupAnimation(){}

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractSpriteHandler>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(PickupAnimation)

#endif
