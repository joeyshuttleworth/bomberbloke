/*
  Removeevent

*/

#ifndef REMOVEEVENT_HPP

#define REMOVEEVENT_HPP
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>
#include "actor.hpp"

extern unsigned int _tick;

class RemoveEvent : public AbstractEvent{
private:
  unsigned int mActorId;
  unsigned int mTick;

public:
  int getType() const{
    return EVENT_REMOVE;
  }

  /* Default constructor needed for cereal */
  RemoveEvent(){}

  unsigned int getId(){return mActorId;}

  RemoveEvent(std::shared_ptr<actor> a){
    mActorId = a->getId();
    mTick = _tick;
    return;
  };

  template<class Archive>
  /* Used by cereal to serialize the event for it to be sent/received */
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("tick", mTick), cereal::make_nvp("actorId", mActorId));
  }
};

CEREAL_REGISTER_TYPE(RemoveEvent)

#endif
