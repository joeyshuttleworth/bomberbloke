
/*
  Event that describes commands sent by the player.
  For example, such a command could describe movement or
  some in game action
*/

#ifndef SERVERINFOEVENT_HPP

#define SERVERINFOEVENT_HPP
#include "ServerInfo.hpp"
#include "AbstractEvent.hpp"
#include <cereal/types/vector.hpp>

class ServerInfoEvent : public AbstractEvent{
private:
  std::string mCommand;
  //Check whether a nickname attached the query is valid and not taken by any other clients.
  bool mUserNameFree = false;
  int mNumberOfPlayers;
  /*A magic number is used to provide some level of verification when the client decides to join*/
  int mMagicNumber;
  std::string mTitle;
  std::string mDescription;
  int mMaxPlayers;
public:

  ServerInfoEvent(ServerInfo info, std::list<std::shared_ptr<AbstractPlayer>> players){
    mType = SERVER_INFO;
    mTitle = info.mTitle;
    mDescription = info.mDescription;
    mMaxPlayers = info.mMaxPlayers;
    mNumberOfPlayers = players.size();
    return;
  };

  template<class Archive>
  /*Used by cereal to serialize the event for it to be sent/received*/
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), cereal::make_nvp("Title", mTitle), cereal::make_nvp("Description", mDescription), cereal::make_nvp("MaxPlayers", mMaxPlayers), cereal::make_nvp("PlayerCount", mNumberOfPlayers), cereal::make_nvp("MagicNumber", mMagicNumber), cereal::make_nvp("UserNameFree?",mUserNameFree));
  }

};

#endif
