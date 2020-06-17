/*
  Event used to set a players properties.
*/

#ifndef PLAYERPROPERTIESEVENT_HPP
#define PLAYERPROPERTIESEVENT_HPP

#include <cereal/types/vector.hpp>
#include "AbstractEvent.hpp"
#include <cereal/types/base_class.hpp>

class PlayerPropertiesEvent :  public  AbstractEvent{
private:
  std::shared_ptr<GamePlayerProperties> mProperties;
public:

  int getType() const{
    return PROPERTIES;
  }

  QueryEvent(GamePlayerProperties &properties){
    std::shared_ptr<GamePlayerProperties> pProperties = new GamePlayerProperties;
    *pProperties = properties;
    mProperties.reset(new GamePlayerProperties(&properties));
  };

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), mNickname);
  };

};

#endif
