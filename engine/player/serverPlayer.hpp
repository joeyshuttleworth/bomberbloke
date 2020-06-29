#ifndef SERVERPLAYER_HPP
#define SERVERPLAYER_HPP

#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include "AbstractPlayer.hpp"
#include <enet/enet.h>

class serverPlayer : public AbstractPlayer{
private:
  ENetPeer *mpPeer;
  bool mLocal;

public:
  unsigned int state = DISCONNECTED;

  bool isLocal(){return mLocal;}

  serverPlayer(std::shared_ptr<AbstractPlayer> player, bool local = false){
    mNickname = player->mNickname;
    mLocal = local;
    mId = player->getId();
    return;
  }

  serverPlayer(){}

  ~serverPlayer(){
    return;
  }

  int getId(){return mId;}

  bool synced = false;

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::make_nvp("nickname", mNickname), cereal::make_nvp("isLocal", mLocal), cereal::make_nvp("ID", mId));
  }
};

/*Polymorphic types have to be registered for cereal*/
CEREAL_REGISTER_TYPE(serverPlayer)


#endif

