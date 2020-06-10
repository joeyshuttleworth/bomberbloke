#ifndef LOCALPLAYER_HPP
#define LOCALPLAYER_HPP
 
#include "AbstractPlayer.hpp"
#include <cereal/types/polymorphic.hpp>

class LocalPlayer : public AbstractPlayer{
 public:
  std::list<CommandBinding> mControlScheme;
  
  LocalPlayer(std::string nickname){
    for(unsigned int i = 0; i < _default_bindings.size(); i++)
      mControlScheme.push_back(_default_bindings[i]);
    mNickname = nickname;
    return;
  }

  void ping(){
    return;
  }

  int getPing(){
    return 0;
  }

  using AbstractPlayer::AbstractPlayer;

};

CEREAL_REGISTER_TYPE(LocalPlayer)

#endif
