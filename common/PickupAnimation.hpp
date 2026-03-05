#ifndef PICKUPANIMATION_HPP
#define PICKUPANIMATION_HPP

#include "AbstractPickup.hpp"
#include "bomberbloke_actors.hpp"
#include "staticSprite.hpp"
#include <string>

class PickupAnimation : public AbstractSpriteHandler{
private:
  std::string mAssetName;
public:
  int getType() const{return AbstractPickup::PICKUP_NONE;}


  PickupAnimation(){}

  PickupAnimation(IGraphicsManager* gfx_manager, double xpos, double ypos, double xdim, double ydim, std::string asset_name)
    : AbstractSpriteHandler(gfx_manager, xpos, ypos, xdim, ydim){
    mAssetName = asset_name;
    return;
  }

  void draw(Camera* cam){
    int bloom = std::abs((int)(_tick - mStartTick) % (2*50) - 50);
    auto dstrect = cam->getScreenRect(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
    mpGraphicsManager->drawSprite(mAssetName, dstrect, true, bloom);
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
