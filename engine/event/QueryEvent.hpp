/*
Event used to request information from a server
*/

#ifndef QUERYEVENT_HPP

#define QUERYEVENT_HPP
#include "AbstractEvent.hpp"
//#include <cereal/types/vector.hpp>
#include <cereal/types/base_class.hpp>

class QueryEvent :  public  AbstractEvent{
private:
  std::string mNickname;
public:

  QueryEvent(std::string nickname){
    mType = QUERY;
    mNickname = nickname;
  };

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), mNickname);
      };

};

#endif
