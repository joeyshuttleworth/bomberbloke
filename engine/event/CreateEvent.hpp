/*
  CreateEvent

 */

#ifndef CREATEEVENT_HPP

#define CREATEEVENT_HPP
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>
#include "actor.hpp"

extern unsigned int _tick;

class CreateEvent : public AbstractEvent{
private:
  std::shared_ptr<actor> mActor;
  unsigned int mTick;
public:
  int getType() const{
    return EVENT_CREATE;
  }

  std::shared_ptr<actor> getActor(){return mActor;};

  /* Default constructor needed for cereal */
  CreateEvent(){}

  CreateEvent(std::shared_ptr<actor> Actor){
    mActor = Actor;
    mTick = _tick;
    return;
  };

  template<class Archive>
  /* Used by cereal to serialize the event for it to be sent/received */
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("tick", mTick), cereal::make_nvp("actor", mActor));
  }
};

CEREAL_REGISTER_TYPE(CreateEvent)

#endif


