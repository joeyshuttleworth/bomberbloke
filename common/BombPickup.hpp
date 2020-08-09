#ifndef BOMBPICKUP_HPP
#define BOMBPICKUP_HPP

#include "AbstractPickup.hpp"
#include "bomberbloke.h"

class BombPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */

  BombPickup(double xpos = 0, double ypos = 0) : AbstractPickup(xpos, ypos){
    /*TODO: draw asset*/
    return;
  }

  void pickup(std::shared_ptr<bloke> b){
    log_message(INFO, "Picked up extra bomb");
    if(b->mMaxBombs < MAX_BOMBS)
      b->mMaxBombs++;
    else
      log_message(INFO, "Max bombs reached");
  }

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }

};

CEREAL_REGISTER_TYPE(BombPickup)

#endif
