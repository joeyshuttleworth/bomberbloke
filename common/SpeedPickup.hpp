#ifndef SPEEDPICKUP_HPP
#define SPEEDPICKUP_HPP
#include "AbstractPickup.hpp"
#include "PickupAnimation.hpp"

class SpeedPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */

  SpeedPickup(scene* scn=nullptr, double xpos = 0, double ypos = 0) : AbstractPickup(scn, xpos, ypos){
    init("lightning.png");
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
