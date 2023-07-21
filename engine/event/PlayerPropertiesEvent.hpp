/*
  Event used to set a players properties.

  Users of this class should note that PlayerProperties are NOT observed across
  clients. Clients will only receive their own PlayerProperties. If this is
  not desired, the metadata framework via MetadataEvent may be suitable.
*/

#ifndef PLAYERPROPERTIESEVENT_HPP
#define PLAYERPROPERTIESEVENT_HPP

#include <memory>
#include <cereal/types/vector.hpp>
#include "AbstractEvent.hpp"
#include <cereal/types/base_class.hpp>

class PlayerPropertiesEvent :  public  AbstractEvent{
private:
  GamePlayerProperties mProperties;
public:
  GamePlayerProperties getProperties(){
    return mProperties;
  }

  EventType getType() const{
    return EVENT_PROPERTIES;
  }

  PlayerPropertiesEvent(GamePlayerProperties &properties){
    mProperties = properties;
  };

  PlayerPropertiesEvent(){};

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), mProperties);
  }
};

CEREAL_REGISTER_TYPE(PlayerPropertiesEvent)
#endif
