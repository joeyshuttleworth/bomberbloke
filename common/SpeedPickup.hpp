#ifndef SPEEDPICKUP_HPP
#define SPEEDPICKUP_HPP
#include "AbstractPickup.hpp"
#include "staticSprite.hpp"
#include "bomberbloke_actors.hpp"
#include "SpeedPickup.hpp"
#include "PickupAnimation.hpp"

class SpeedPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */

  SpeedPickup(scene* scn=nullptr, double xpos = 0, double ypos = 0) : AbstractPickup(scn, xpos, ypos){
    mpSpriteHandler = std::make_shared<PickupAnimation>(mpGraphicsManager, mPosition[0],
                                                        mPosition[1], mDimension[0], mDimension[1], "lightning.png");
    return;
  }

  void pickup(std::shared_ptr<bloke> b);

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(SpeedPickup)
#endif
