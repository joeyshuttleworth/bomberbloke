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

void AbstractPlayer::setId(int id){
  mId = id;
  for(auto i = _pScene->mActors.begin(); i != _pScene->mActors.end(); i++){
    if((*i)->getId() == id){
      mpCharacter = *i;
      return;
    }
  }
}


void AbstractPlayer::ResetPlayerProperties(std::shared_ptr<AbstractPlayerProperties> p_properties){
   if(mpPlayerProperties){
     /*  TODO: send EVENT_PROPERTIES message to the relevant player */
     mpPlayerProperties->reset(p_properties);
   }
   else
     log_message(ERROR, "Tried to reset mpPlayerProperties when it didn't exist");
 }

