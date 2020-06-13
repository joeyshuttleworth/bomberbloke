/*
 The ServerInfo class is used to store information about the server
 */
#ifndef SERVERINFO_HPP
#define    SERVERINFO_HPP
#include <sstream>
#include "engine.hpp"

class ServerInfo{
public:
  void SetTitle(std::string title){
    mTitle = title;
  };
  void SetDescription(std::string description){
    mDescription = description;
  };
  void SetMaxPlayers(int max){
    mMaxPlayers = max;
  }
  ServerInfo(std::string title= "Bomberbloke server", std::string description = "A server for the game multiplayer game, bomberbloke - enjoy!!", int max = 2){
    mDescription = description;
    mMaxPlayers = max;
    mTitle = title;
  };
  std::string mTitle = "Bomberbloke server";
  std::string mDescription = "A server for the game multiplayer game, bomberbloke - enjoy!!";
  int mMaxPlayers = 2;

  std::string getInfo(){
    std::stringstream info_s;
    info_s << mTitle << "\n";
    info_s << mDescription << "\n";
    info_s << mMaxPlayers << ":" << _player_list.size() << "\n";
    info_s << "password no";
    return info_s.str();
  }
};

#endif
