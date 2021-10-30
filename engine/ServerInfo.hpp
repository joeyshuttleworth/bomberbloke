/*
 The ServerInfo class is used to store information about the server
 */
#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP
#include <sstream>

class ServerInfo{
public:

  ServerInfo(std::string title= "Bomberbloke server", std::string description = "A server for the game multiplayer game, bomberbloke - enjoy!!", int max = 16);
  std::string getInfo();

  void SetTitle(std::string title){
    mTitle = title;
  };

  void SetDescription(std::string description){
    mDescription = description;
  };

  void SetMaxPlayers(int max){
    mMaxPlayers = max;
  }

  std::string mTitle = "Bomberbloke server";
  std::string mDescription = "A server for the game multiplayer game, bomberbloke - enjoy!!";
  unsigned int mMaxPlayers;
};

#endif
