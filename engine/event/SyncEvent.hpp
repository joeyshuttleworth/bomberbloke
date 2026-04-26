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

#include <cereal/types/vector.hpp>
#include <memory>
#include <array>

#include "GamePlayerProperties.hpp"
#include "serverPlayer.hpp"
#include "AbstractEvent.hpp"
#include "actor.hpp"

class SyncEvent : public AbstractEvent{
private:
public:
  int mState;
  std::vector<serverPlayer> mPlayers;
  std::list<std::shared_ptr<actor>> mActors;
  std::list<std::shared_ptr<AbstractSpriteHandler>> mParticles;
  std::vector<double> mDimension = {0, 0};

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

    for(auto a : _pScene->mActors){
      mActors.push_back(a);
    }

    for(auto a : _pScene->mParticles){
      mParticles.push_back(a);
    }

    auto dims = _pScene->getDimension();
    mDimension[0] = dims[0];
    mDimension[1] = dims[1];
    return;
  }

  /*Used by cereal to serialize the event for it to be sent/received*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this),
            cereal::make_nvp("mState", mState),
            cereal::make_nvp("mActors", mActors),
            cereal::make_nvp("mParticles", mActors),
            cereal::make_nvp("mDimension", mDimension),
            cereal::make_nvp("mPlayers", mPlayers)
            );
  }
};

CEREAL_REGISTER_TYPE(SyncEvent)

#endif

