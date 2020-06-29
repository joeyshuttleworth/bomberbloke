#include "AbstractPlayer.hpp"
#include "actor.hpp"
#include "engine.hpp"


AbstractPlayer::AbstractPlayer(std::string _nickname){
  mNickname = _nickname;
  return;
}

void AbstractPlayer::setCharacter(std::shared_ptr<actor> a){
  if(!a){
    log_message(WARNING, "Tried setting the player character to a null actor");
    return;
  }
  mpCharacter = a;
  mpCharacter->setPlayerId(mId);
  return;
}
