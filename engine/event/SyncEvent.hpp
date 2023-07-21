/**
  This event is used to sync the entire game state between
  the server and client. Game state information is stored
  across a few variables:
            -  _player_list
            -  _state
            -  _pScene->getType
            -  _pScene->mActorList
            -  GamePlayerProperties for the client
*/

#ifndef SYNCEVENT_HPP
#define SYNCEVENT_HPP

#include "GamePlayerProperties.hpp"
#include "serverPlayer.hpp"
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>

class SyncEvent : public AbstractEvent{
private:
  std::vector<serverPlayer> mPlayers;
  std::vector<actor>    mActors;
public:
  int mState;

  EventType getType() const{
    return EVENT_SYNC;
  }

  std::vector<serverPlayer> getPlayers(){
    return mPlayers;
  }

  SyncEvent(){};

  explicit SyncEvent(int to_id){   /* Generate the list of players */
    for(auto i = _player_list.begin(); i != _player_list.end(); i++){
      if(to_id > 0 && to_id == (*i)->getId())
        mPlayers.push_back(serverPlayer(*i, true));
      else
        mPlayers.push_back(serverPlayer(*i, false));
    }
    if(_pScene)
      mState = _pScene->getState();
    return;
  }

  /*Used by cereal to serialize the event for it to be sent/received*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this),
      cereal::make_nvp("state", mState),
            cereal::make_nvp("mActors", _pScene->mActors),
            cereal::make_nvp("players", mPlayers));
  }
};

CEREAL_REGISTER_TYPE(SyncEvent)

#endif

