/* Abstract class for events to be sent to the client/server.*/

#include <cereal/types/base_class.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>

#ifndef ABSTRACTEVENT_HPP
#define ABSTRACTEVENT_HPP


enum EventType{
  EVENT_JOIN,
  EVENT_MESSAGE,
  EVENT_QUERY,
  EVENT_INFO, // 3
  EVENT_PROPERTIES, // 4
  EVENT_MOVE,
  EVENT_PLAYERLEAVE,
  EVENT_CREATE,
  EVENT_REMOVE,
  EVENT_SYNC, // 9
  EVENT_COMMAND, // 10
  EVENT_ERROR,
  EVENT_ACCEPT, // 12
  EVENT_KICK,
};



class AbstractEvent{
public:
  virtual EventType getType() const = 0;

  virtual ~AbstractEvent(){}

  /** For printing displaying printing debug messages mainly */
  virtual std::string output(){return "";};

  /* This is required for cereal */
  template <class Archive>
  void serialize(Archive&){}
};

#endif
