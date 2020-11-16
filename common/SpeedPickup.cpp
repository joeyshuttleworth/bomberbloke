#include "SpeedPickup.hpp"
#include "bloke.hpp"

void SpeedPickup::pickup(std::shared_ptr<bloke> b){
  log_message(DEBUG, "Picked up extra speed");
  if(_server){
    if(b->mMaxSpeed < 9){
      b->mMaxSpeed++;
      std::shared_ptr<AbstractPlayerProperties> props = std::make_shared<GamePlayerProperties>(b);
      b->getPlayer()->resetPlayerProperties(props);
    }
    else
      log_message(INFO, "Max speed reached");
    }
}
