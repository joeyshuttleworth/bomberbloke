#include "BigBombPickup.hpp"
#include "bloke.hpp"

void BigBombPickup::pickup(std::shared_ptr<bloke> b){
  log_message(DEBUG, "Picked up big bomb");
  if(_server){
    if(!b->mBigBomb){
      b->mBigBomb = true;
      std::shared_ptr<AbstractPlayerProperties> props = std::make_shared<GamePlayerProperties>(b);
      b->getPlayer()->resetPlayerProperties(props);
    }
    else
      log_message(INFO, "Max bombs reached");
  }
}

