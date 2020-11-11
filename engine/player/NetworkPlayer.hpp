#ifndef NETWORKPLAYER_HPP
#define NETWORKPLAYER_HPP

#include "AbstractPlayer.hpp"
#include <enet/enet.h>

class NetworkPlayer : public AbstractPlayer{
private:
  ENetPeer *mpPeer;

public:
  unsigned int state = DISCONNECTED;

  NetworkPlayer(std::string nickname, ENetPeer* peer) : AbstractPlayer(){
    mNickname = nickname;
    mpPeer = peer;
    return;
  }

  ENetPeer *getPeer(){return mpPeer;}

  ~NetworkPlayer(){
    // free(mpPeer);
    return;
  }

  void ping(){
    int mLastPingElapsedTime = 0;
    return;
  }

  bool synced = false;
};
#endif

