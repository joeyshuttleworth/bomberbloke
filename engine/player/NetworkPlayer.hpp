#ifndef NETWORKPLAYER_HPP
#define NETWORKPLAYER_HPP

#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include "AbstractPlayer.hpp"
#include <enet/enet.h>

class NetworkPlayer : public AbstractPlayer{
private:
  ENetPeer *mpPeer;

public:
  unsigned int state = DISCONNECTED;

  NetworkPlayer(std::string nickname, ENetPeer* peer){
    mNickname = nickname;
    mpPeer = peer;
    return;
  }

  ~NetworkPlayer(){
    free(mpPeer);
    return;
  }

  void ping(){
    //int mLastPingElapsedTime = 0;
    return;
  }
  bool synced = false;

};

/*Polymorphic types have to be registered for cereal*/
CEREAL_REGISTER_TYPE(NetworkPlayer)


#endif

