#include "BombPickup.hpp"
#include "bloke.hpp"

void BombPickup::pickup(std::shared_ptr<bloke> b){
  log_message(INFO, "Picked up extra bomb");
  if(b->mMaxBombs < MAX_BOMBS)
    b->mMaxBombs++;
  if(_server){
      std::shared_ptr<AbstractPlayerProperties> props = std::make_shared<GamePlayerProperties>(b);
      _local_player_list.back().resetPlayerProperties(props);
    }
  else
    log_message(INFO, "Max bombs reached");
}

