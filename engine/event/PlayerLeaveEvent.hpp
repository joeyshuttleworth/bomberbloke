#ifndef PLAYERLEAVEEVENT_HPP
#define PLAYERLEAVEEVENT_HPP

#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>

class PlayerLeaveEvent : public AbstractEvent {
public:
  int id;
  EventType getType() const{
    return EVENT_PLAYERLEAVE;
  }

  PlayerLeaveEvent(int peerId) {
    id = peerId;
  };
  PlayerLeaveEvent(){};

  template<class Archive>
  /*Used by cereal to serialize the event for it to be sent/received*/
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("id", id));
  }
};

CEREAL_REGISTER_TYPE(PlayerLeaveEvent)

#endif
