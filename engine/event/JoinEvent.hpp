/*
  Event that describes how an actor has moved.
  This is used by both the server and client to
  update the position of actors. An up to date
  list of actors is required for this to work.
  Such a list is sent using the SyncEvent class
  and update using the CreateEvent and RemoveEvent
  classes
*/

#ifndef JOINEVENT_HPP
#define JOINEVENT_HPP

#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>

class JoinEvent : public AbstractEvent{
private:
  int mMagicNumber;
  std::vector<std::string> mCommands;

public:
  std::string mNickname;
  int getType() const{
    return EVENT_JOIN;
  }

  JoinEvent(std::string nickname, int magic_number, std::vector<std::string> commands = {}){
    mMagicNumber = magic_number;
    mNickname = nickname;
    mCommands = commands;
    return;
  };

  JoinEvent(){};

  std::vector<std::string> getCommands(){return mCommands;}

  template<class Archive>
  /*Used by cereal to serialize the event for it to be sent/received*/
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("Nickname", mNickname),
            cereal::make_nvp("magic number", mMagicNumber),
            cereal::make_nvp("commands", mCommands));
  }

};

CEREAL_REGISTER_TYPE(JoinEvent)

#endif
