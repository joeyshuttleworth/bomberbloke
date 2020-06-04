
/*
  Event that describes commands sent by the player.
  For example, such a command could describe movement or
  some in game action
*/

#ifndef COMMANDEVENT_HPP

#define COMMANDEVENT_HPP
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>

class CommandEvent : public AbstractEvent{
private:
  std::string mCommand;
public:

  CommandEvent(std::string command){
    mCommand = command;
    mType = COMMAND;
    return;
  };
  template<class Archive>
  /*Used by cereal to serialize the event for it to be sent/received*/
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("Command", mCommand));
  }

};

#endif
