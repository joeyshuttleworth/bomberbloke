/*
Event used to request information from a server
*/

#ifndef QUERYEVENT_HPP
#define QUERYEVENT_HPP

#include <cereal/types/vector.hpp>
#include "AbstractEvent.hpp"
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>

class QueryEvent :  public  AbstractEvent{
public:
  std::string mNickname;

  EventType getType() const{
    return EVENT_QUERY;
  }

  std::string getNickname(){
    return mNickname;
  }

  QueryEvent(std::string nickname = "player"){
    mNickname = nickname;
  };

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this),
            cereal::make_nvp("nickname", mNickname));
  }
};

CEREAL_REGISTER_TYPE(QueryEvent)

#endif
