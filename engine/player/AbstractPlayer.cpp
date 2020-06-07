#include "AbstractPlayer.hpp"
#include "actor.hpp"
void AbstractPlayer::init(actor *actor){
  mpCharacter.reset(actor);
  actor->setController(this);
  return;
}

AbstractPlayer::AbstractPlayer(){
  return;
}

AbstractPlayer::AbstractPlayer(std::string _nickname){
  mNickname = _nickname;
  return;
}



