#ifndef NETWORKPLAYER_HPP
#define NETWORKPLAYER_HPP

#include "AbstractPlayer.hpp"
#include <enet/enet.h>

class NetworkPlayer : public AbstractPlayer{
private:
  int mId;

public:
  unsigned int state = DISCONNECTED;

  NetworkPlayer(std::string nickname, int id) : AbstractPlayer(){
    mNickname = nickname;
    mId = id;
    return;
  }

  ~NetworkPlayer(){
    return;
  }

  int getId() { return mId; };

  bool synced = false;
};
#endif

