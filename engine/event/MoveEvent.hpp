/*
  Event that describes how an actor has moved.
  This is used by both the server and client to
  update the position of actors. An up to date
  list of actors is required for this to work.
  Such a list is sent using the SyncEvent class
  and update using the CreateEvent and RemoveEvent
  classes
*/

#ifndef MOVEEVENT_HPP

#define MOVEEVENT_HPP
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>

class MoveEvent : public AbstractEvent{
private:
  int mActorId;
  double mPosition[2];
  double mVelocity[2];
public:

  MoveEvent(actor* Actor){
    mType = MOVE; 
    mActorId  = Actor->mId;
    mPosition[0] = Actor->mPosition[0];
    mPosition[1] = Actor->mPosition[1];
    mVelocity[0]  = Actor->mVelocity[0];
    mVelocity[1]  = Actor->mVelocity[1];
    return;
  };
  template<class Archive>
  /*Used by cereal to serialize the event for it to be sent/received*/
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("id", mActorId), cereal::make_nvp("position", mPosition), cereal::make_nvp("velocity", mVelocity));
  }

};

#endif


