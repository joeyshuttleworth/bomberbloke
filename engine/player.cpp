#include "engine.h"

void player::init(actor *actor){
  character = actor;
  actor->setController(this);
  return;
}

player::player(){
  return;
}

player::player(std::string _nickname){
    nickname = _nickname;
    return;
}

player::~player(){
  return;
}

localPlayer :: localPlayer(std::string _nickname){
  for(unsigned int i = 0; i < _default_bindings.size(); i++)
    mControlScheme.push_back(_default_bindings[i]);
  return;
}

void player :: ping(){
  return;
}

void networkPlayer :: ping(){
  //TODO
  return;
}

networkPlayer::~networkPlayer(){
  return;
}
