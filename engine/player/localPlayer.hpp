#ifndef LOCALPLAYER_HPP
#define LOCALPLAYER_HPP
 
#include "AbstractPlayer.hpp"
#include <cereal/types/polymorphic.hpp>

class localPlayer : public AbstractPlayer{
 public:
  std::list<command_binding> mControlScheme;
  
  localPlayer(std::string _nickname){
    for(unsigned int i = 0; i < _default_bindings.size(); i++)
      mControlScheme.push_back(_default_bindings[i]);
    return;
  }

  void ping(){
    return;
  }

  int getPing(){
    return 0;
  }

  using AbstractPlayer::AbstractPlayer;
  using AbstractPlayer::serialize;

};

CEREAL_REGISTER_TYPE(localPlayer);

#endif
