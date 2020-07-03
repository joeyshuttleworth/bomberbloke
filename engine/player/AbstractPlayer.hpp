/*Class to store info for players. These may
  be clients connected to a server, or LocalPlayers
  connected to a client (or server).

  This class stores some game specific AbstractProperties
  (such as mMaxSpeed in bomberbloke). It is also used by
  the server to handle the input from local players and
  clients.
 */

#ifndef ABSTRACTPLAYER_HPP
#define ABSTRACTPLAYER_HPP
#include<string>
#include<memory>
#include <enet/enet.h>
#include <cereal/types/base_class.hpp>
#include <cereal/archives/json.hpp>

class actor;

class AbstractPlayer{
public:
  AbstractPlayer(std::string);
  AbstractPlayer(){};
  ~AbstractPlayer(){};

  void setId(int id);

  std::string mNickname;

  int getId(){
    return mId;
  }

  /** Get a pointer the ENetAddress of the player.
      this is only non-null for NetworkPlayers
   */

  virtual ENetPeer *getPeer(){return nullptr;}

  void setCharacter(std::shared_ptr<actor>);
  std::shared_ptr<actor> getCharacter(){return mpCharacter;}

  /*Used by cereal to serialize the event for it to be sent/received*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::make_nvp("nickname", mNickname));
  }

  virtual int getPing(){return 0;}

protected:
  std::shared_ptr<actor> mpCharacter;
  virtual void ping(){}
  int mId;
};

#endif
