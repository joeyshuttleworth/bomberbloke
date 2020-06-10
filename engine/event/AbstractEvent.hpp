/* Abstract class for events to be sent to the client/server.*/

#include <cereal/types/base_class.hpp>
#include <cereal/types/vector.hpp>

#ifndef ABSTRACTEVENT_HPP

#define ABSTRACTEVENT_HPP

enum EventType{
  JOIN,
  CLIENT_MESSAGE,
  SERVER_MESSAGE,
  QUERY,
  SERVER_INFO,
  MOVE,
  CREATE,
  REMOVE,
  SYNC,
  NEW_GAME,
  START,
  STOP,
  COMMAND
};

class AbstractEvent{
public:
  int mType = 0;
  template <class Archive>
  void serialize(Archive &ar){
    ar(cereal::make_nvp("EventType", mType));
  }
};


#endif
