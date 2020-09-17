#include "PowerPickup.hpp"
#include "bloke.hpp"

void PowerPickup::pickup(std::shared_ptr<bloke> b){
  log_message(INFO, "Picked up extra power");
  if(_server){
    if(b->mPower < 9){
      b->mPower++;
      std::shared_ptr<AbstractPlayerProperties> props = std::make_shared<GamePlayerProperties>(b);
      b->getPlayer()->resetPlayerProperties(props);
    }
  }
  else
    log_message(INFO, "Max power reached");
}
