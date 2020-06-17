/*
  Event that describes how an actor has moved.
  This is used by both the server and client to
  update the position of actors. An up to date
  list of actors is required for this to work.
  Such a list is sent using the SyncEvent class
  and update using the CreateEvent and RemoveEvent
  classes
*/

#ifndef JOINEVENT_HPP

#define JOINEVENT_HPP
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>

class MoveEvent : AbstractEvent{
private:
 AbstractPlayer mPlayer;
public:

  int getType() const{
    return EVENT_JOIN;
  }

  MoveEvent(player*AbstractPlayer){
    mPlayer = *Player;
    return;
  };

  template<class Archive>
  /*Used by cereal to serialize the event for it to be sent/received*/
  void serialize(Archive &archive){
    archive(cereal::make_nvp("Nickname", mPlayer.nickname))
  }

};

#endif
 
