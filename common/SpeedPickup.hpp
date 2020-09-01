#ifndef SPEEDPICKUP_HPP
#define SPEEDPICKUP_HPP
#include "AbstractPickup.hpp"
#include "bomberbloke.h"

class SpeedPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */

  SpeedPickup(double xpos = 0, double ypos = 0) : AbstractPickup(xpos, ypos){
    /*TODO: draw asset*/
    return;
  }

  void pickup(std::shared_ptr<bloke> b){
    log_message(INFO, "Picked up extra speed");
    if(b->mMaxSpeed < DEFAULT_MAX_SPEED)
      b->mMaxSpeed += DEFAULT_SPEED;
    else
      log_message(INFO, "Max speed reached");
  }

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }

};

CEREAL_REGISTER_TYPE(SpeedPickup)
#endif
