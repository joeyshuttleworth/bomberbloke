/*
  Event to show that some form of error has occurred,
  a message may be included. Almost identical to the
  acceptEvent class.
  TODO: consider implementing a message id system. May not
  be necessary.
*/


#ifndef ACCEPTEVENT_HPP

#define ACCEPTEVENT_HPP
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>

class acceptEvent : public AbstractEvent{
private:
  std::string mMessage;
public:

  int getType() const{
    return EVENT_ACCEPT;
  }

  acceptEvent(std::string message = ""){
    mMessage = message;
    return;
  };

  template<class Archive>
  /*Used by cereal to serialize the event for it to be sent/received*/
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("message", mMessage));
  }

};

CEREAL_REGISTER_TYPE(acceptEvent)

#endif
