
/*
  Event that describes messages sent by the player to the server or messages
  from the server to all players.
*/

#ifndef MESSAGEEVENT_HPP
#define MESSAGEEVENT_HPP

#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>

class MessageEvent : public AbstractEvent{
private:
  std::string mMessage;
public:

  int getType() const{
    return EVENT_MESSAGE;
  }

  std::string getMessage(){
    return mMessage;
  }

  MessageEvent(std::string message){
    mMessage = message;
    return;
  };

  /* Needed for cereal */
  MessageEvent(){}

  template<class Archive>
  /*Used by cereal to serialize the event for it to be sent/received*/
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("Message", mMessage));
  }

};

CEREAL_REGISTER_TYPE(MessageEvent)

#endif
