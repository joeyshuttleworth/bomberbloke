#ifndef NETWORKPLAYER_HPP
#define NETWORKPLAYER_HPP

#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include "AbstractPlayer.hpp"

class NetworkPlayer : public AbstractPlayer{
 public:
  unsigned int state = DISCONNECTED;
  void ping(){
    //int mLastPingElapsedTime = 0;
    return;
  }
  bool synced = false;

  using AbstractPlayer::AbstractPlayer;
};

/*Polymorphic types have to be registered for cereal*/
CEREAL_REGISTER_TYPE(NetworkPlayer)


#endif

