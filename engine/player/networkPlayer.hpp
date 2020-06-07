#ifndef NETWORKPLAYER_HPP
#define NETWORKPLAYER_HPP

#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include "AbstractPlayer.hpp"

class networkPlayer : public AbstractPlayer{
 public:
  unsigned int state = DISCONNECTED;
  void ping(){
    int mLastPingElapsedTime = 0;
    int mPingStart = _tick;
    return;
  }
  bool synced = false;

  using AbstractPlayer::AbstractPlayer;
  using AbstractPlayer::serialize;
};

/*Polymorphic types have to be registered for cereal*/
CEREAL_REGISTER_TYPE(networkPlayer);


#endif

