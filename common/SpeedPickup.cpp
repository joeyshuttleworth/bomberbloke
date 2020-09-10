#include "SpeedPickup.hpp"
#include "bloke.hpp"

void SpeedPickup::pickup(std::shared_ptr<bloke> b){
  log_message(INFO, "Picked up extra speed");
    if(_server){
      if(b->mMaxSpeed < DEFAULT_MAX_SPEED){
        b->mMaxSpeed + DEFAULT_SPEED;
      std::shared_ptr<AbstractPlayerProperties> props = std::make_shared<GamePlayerProperties>(b);
      _local_player_list.back().resetPlayerProperties(props);
    }
  }
  else
    log_message(INFO, "Max speed reached");
  }
