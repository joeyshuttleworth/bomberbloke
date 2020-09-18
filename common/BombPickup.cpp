#include "BombPickup.hpp"
#include "bloke.hpp"

void BombPickup::pickup(std::shared_ptr<bloke> b){
  log_message(INFO, "Picked up extra bomb");
    if(_server){
      if(b->mMaxBombs < 10){
        b->mMaxBombs++;
        std::shared_ptr<AbstractPlayerProperties> props = std::make_shared<GamePlayerProperties>(b);
        b->getPlayer()->resetPlayerProperties(props);
      }
      else
        log_message(INFO, "Max bombs reached");
    }
}

