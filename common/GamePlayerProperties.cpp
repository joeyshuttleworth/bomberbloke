#include "GamePlayerProperties.hpp"
#include "bloke.hpp"

GamePlayerProperties::GamePlayerProperties(std::shared_ptr<bloke> b){
  mSpeed = b->mMaxSpeed;
  mMaxBombs = b->mMaxBombs;
  mBigBomb = b->mBigBomb;
}
