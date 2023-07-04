#ifndef NETWORKPLAYER_HPP
#define NETWORKPLAYER_HPP

#include "AbstractPlayer.hpp"

class NetworkPlayer : public AbstractPlayer{
public:
  unsigned int state = DISCONNECTED;

  NetworkPlayer(std::string nickname, int id) : AbstractPlayer(){
    mNickname = nickname;
    mId = id;
  }

  ~NetworkPlayer(){}

  bool synced = false;
};
#endif

