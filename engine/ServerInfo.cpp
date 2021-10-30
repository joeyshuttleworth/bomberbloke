#include "ServerInfo.hpp"
#include "engine.hpp"

ServerInfo::ServerInfo(std::string title, std::string description, int max)
{
  mDescription = description;
  mMaxPlayers = max;
  mTitle = title;
}

std::string mTitle = "Bomberbloke server";
std::string mDescription =
  "A server for the game multiplayer game, bomberbloke - enjoy!!";

std::string
ServerInfo::getInfo()
{
  std::stringstream info_s;
  info_s << mTitle << "\n";
  info_s << mDescription << "\n";
  info_s << mMaxPlayers << ":" << _player_list.size() << "\n";
  info_s << "password no";
  return info_s.str();
}
