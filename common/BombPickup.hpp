#ifndef BOMBPICKUP_HPP
#define BOMBPICKUP_HPP

#include "AbstractPickup.hpp"
#include "bomberbloke_actors.hpp"
#include "staticSprite.hpp"
#include "PickupAnimation.hpp"

class BombPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */

  BombPickup(scene *scn=nullptr, double xpos = 0, double ypos = 0) : AbstractPickup(scn, xpos, ypos){
    mpSpriteHandler = std::make_shared<PickupAnimation>(mpGraphicsManager, mPosition[0], mPosition[1], mDimension[0], mDimension[1], "bomb_pickup.png");
    return;
  }

  void pickup(std::shared_ptr<bloke> b);

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(BombPickup)

#endif
