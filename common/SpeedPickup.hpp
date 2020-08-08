#ifndef SPEEDPICKUP_HPP
#define SPEEDPICKUP_HPP
#include "AbstractPickup.hpp"

class SpeedPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  void update(){
    std::list<std::shared_ptr<actor>> actor_list = _pScene->ActorsCollidingWith(this);
    for(auto i = actor_list.begin(); i != actor_list.end(); i++){
      if((*i)->getType() == ACTOR_BLOKE){
        log_message(DEBUG, "bloke picked up speed pickup");
        remove();
      }
    }
  }

  /* Need a default constructor for cereal */

  SpeedPickup(double xpos = 0, double ypos = 0) : AbstractPickup(xpos, ypos){
    return;
  }

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }

};

CEREAL_REGISTER_TYPE(SpeedPickup)
#endif
