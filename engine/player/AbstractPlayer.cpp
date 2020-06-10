#include "AbstractPlayer.hpp"
#include "actor.hpp"
void AbstractPlayer::init(std::shared_ptr<actor> a){
  mpCharacter = a;
  return;
}

AbstractPlayer::AbstractPlayer(){
  return;
}

AbstractPlayer::~AbstractPlayer(){

  return;
}

AbstractPlayer::AbstractPlayer(std::string _nickname){
  mNickname = _nickname;
  return;
}



