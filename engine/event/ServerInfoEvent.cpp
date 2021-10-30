#include "ServerInfoEvent.hpp"

ServerInfoEvent::ServerInfoEvent(
  ServerInfo& info,
  std::list<std::shared_ptr<AbstractPlayer>> players,
  std::string nickname)
{
  mTitle = info.mTitle;
  mDescription = info.mDescription;
  mMaxPlayers = info.mMaxPlayers;
  mNumberOfPlayers = players.size();
  if (nickname == "") {
    /* empty user name - tell the client they cannot use it */
    mUserNameFree = false;
  } else {
    /*  Check if the username is already taken */
    auto iter = find_if(
      players.begin(), players.end(), [&](std::shared_ptr<AbstractPlayer> p) {
        return nickname == p->mNickname;
      });
    if (iter == players.end() || players.size() == 0)
      mUserNameFree = true;
    else
      mUserNameFree = false;
  }
  return;
}
