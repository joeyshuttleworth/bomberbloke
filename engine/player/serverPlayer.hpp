#ifndef SERVERPLAYER_HPP
#define SERVERPLAYER_HPP

#include "AbstractPlayer.hpp"

class serverPlayer : public AbstractPlayer{
private:
  bool mLocal;

public:
  unsigned int state = DISCONNECTED;

  bool isLocal(){return mLocal;}

  serverPlayer(std::shared_ptr<AbstractPlayer> player, bool local = false){
    mNickname = player->mNickname;
    mLocal = local;
    mId = player->getId();
    mWins = player->getWins();
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
    archive(cereal::make_nvp("nickname", mNickname), cereal::make_nvp("isLocal", mLocal), cereal::make_nvp("ID", mId), cereal::make_nvp("wins", mWins));
  }
};

#endif

