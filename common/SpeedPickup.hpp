#ifndef SPEEDPICKUP_HPP
#define SPEEDPICKUP_HPP
#include "AbstractPickup.hpp"
#include "bomberbloke_actors.hpp"

class SpeedPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */

  SpeedPickup(double xpos = 0, double ypos = 0) : AbstractPickup(xpos, ypos){
    /*TODO: draw asset*/
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
