#ifndef POWERPICKUP_HPP
#define POWERPICKUP_HPP
#include "AbstractPickup.hpp"
#include "staticSprite.hpp"
#include "bomberbloke_actors.hpp"

class PowerPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_POWER;}

  /* Need a default constructor for cereal */

  PowerPickup(double xpos = 0, double ypos = 0) : AbstractPickup(xpos, ypos){
    /*TODO: draw asset*/
    mpSpriteHandler = std::make_shared<staticSprite>(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1], "flames.png");
    return;
  }

  void pickup(std::shared_ptr<bloke> b);

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(PowerPickup)
#endif
