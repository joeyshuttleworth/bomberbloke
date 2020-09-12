#include "PowerPickup.hpp"
#include "bloke.hpp"

void PowerPickup::pickup(std::shared_ptr<bloke> b){
  log_message(INFO, "Picked up extra power");
  if(_server){
    b->mPower + 1;
    std::shared_ptr<AbstractPlayerProperties> props = std::make_shared<GamePlayerProperties>(b);
    _local_player_list.back().resetPlayerProperties(props);
    }
  else
    log_message(INFO, "Max power reached");
}
