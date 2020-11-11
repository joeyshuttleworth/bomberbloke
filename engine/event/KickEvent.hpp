/**
 *  This event lets the client know that they are being kicked and
 *  allows the server to provide a reason for this kick.
 */

#ifndef KICKEVENT_HPP
#define KICKEVENT_HPP

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/polymorphic.hpp>

extern unsigned int _state;


#define KICKEVENT_HPP
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>

class KickEvent : public AbstractEvent{
private:
  std::string mReason;
public:
  int getType() const{
    return EVENT_KICK;
  }

  std::string getReason(){
    return mReason;
  }

  KickEvent(std::string reason = ""){ 
    mReason = reason;
    return;
  }

  /*Used by cereal to serialize the event for it to be sent/received*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), mReason);
  }
};

// CEREAL_REGISTER_TYPE(KickEvent)

#endif

