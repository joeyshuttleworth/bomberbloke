/*
  CreationEvent

 */

#ifndef CREATIONEVENT_HPP
#define CREATIONEVENT_HPP
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>
#include "actor.hpp"

extern unsigned int _tick;

class CreationEvent;

class CreationEvent : public AbstractEvent{
private:
  std::shared_ptr<actor> mActor = nullptr;
  std::shared_ptr<AbstractSpriteHandler> mParticle = nullptr;
  unsigned int mTick;
public:
  int getType() const{
    return EVENT_CREATE;
  }

  std::shared_ptr<actor> getActor(){return mActor;}

  std::shared_ptr<AbstractSpriteHandler>getParticle(){return mParticle;}

  /* Default constructor needed for cereal */
  CreationEvent(){}

  CreationEvent(std::shared_ptr<actor> Actor){
    mActor = Actor;
    mTick = _tick;
    return;
  }

  CreationEvent(std::shared_ptr<AbstractSpriteHandler> particle){
    mParticle = particle;
    mTick = _tick;
    return;
  }

  template<class Archive>
  /* Used by cereal to serialize the event for it to be sent/received */
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("tick", mTick), cereal::make_nvp("actor", mActor), cereal::make_nvp("particle", mParticle));
  }
};

CEREAL_REGISTER_TYPE(CreationEvent)

#endif


