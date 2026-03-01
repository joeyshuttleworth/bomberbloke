#ifndef LOCALPLAYER_HPP
#define LOCALPLAYER_HPP
#include "AbstractPlayer.hpp"
#include <list>


struct CommandBinding;
extern std::vector<CommandBinding> _default_bindings;

class LocalPlayer : public AbstractPlayer{
 public:
  std::list<CommandBinding> mControlScheme;

  ~LocalPlayer(){};

  LocalPlayer(std::string nickname = "player", int id=1) : AbstractPlayer(){
    for(unsigned int i = 0; i < _default_bindings.size(); i++)
      mControlScheme.push_back(_default_bindings[i]);
    mNickname = nickname;
    mId = id;
    return;
  }

  void ping(){
    return;
  }

  int getPing(){
    return 0;
  }

  void setId(int id){
    mId = id;
    return;
  }

};

#endif
