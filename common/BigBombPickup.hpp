#ifndef BIGBOMBPICKUP_HPP
#define BIGBOMBPICKUP_HPP

#include "AbstractPickup.hpp"
#include "staticSprite.hpp"
#include "PickupAnimation.hpp"

class BigBombPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */

  BigBombPickup(double xpos = 0, double ypos = 0) : AbstractPickup(xpos, ypos){
    /*TODO: draw asset*/
    mpSpriteHandler = std::make_shared<PickupAnimation>(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1], "bigredbomb.png");
    return;
  }

  void pickup(std::shared_ptr<bloke> b);

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(BigBombPickup)

#endif
