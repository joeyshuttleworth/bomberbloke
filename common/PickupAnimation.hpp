#ifndef PICKUPANIMATION_HPP
#define PICKUPANIMATION_HPP

#include "AbstractPickup.hpp"
#include "bomberbloke_actors.hpp"
#include "staticSprite.hpp"

class PickupAnimation : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}


  PickupAnimation(){}

  PickupAnimation(double xpos, double ypos, double xdim, double ydim, std::string asset_name)
    : AbstractSpriteHandler(xpos, ypos, xdim, ydim){
    mpSprite = get_sprite(asset_name);
    return;
  }

  void draw(Camera *cam){
    SDL_Rect dstrect = cam->getScreenRect(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
    cam->renderCopy(mpSprite, nullptr, &dstrect);
    return;
  }

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(PickupAnimation)

#endif
