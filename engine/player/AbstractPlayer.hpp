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
#include <cereal/types/base_class.hpp>
#include <cereal/archives/json.hpp>
#include "GamePlayerProperties.hpp"
#include "Metadata.hpp"

class actor;
class scene;
class NetClient;

class AbstractPlayer{
  friend scene;
  friend NetClient;
public:
  AbstractPlayer(std::string);
  AbstractPlayer(){
    mpProperties = std::make_shared<GamePlayerProperties>();
  }

  virtual  ~AbstractPlayer(){};

  void setId(int id);

  std::string mNickname;

  // Player metadata, updated by protocol - not serialized as part of AbstractPlayer
  Metadata mMetadata;

  void addWin(){mWins++;}

  int getWins(){return mWins;}

  int getId(){return mId;}

  std::string getNickname(){return mNickname;}

  std::shared_ptr<actor> getCharacter(){return mpCharacter;}

  /*Used by cereal to serialize the event for it to be sent/received*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::make_nvp("nickname", mNickname),
            cereal::make_nvp("colour", mColour), mWins, mId);
  }

  virtual int getPing(){return 0;}

  /*
   * Change the player properties object relating to this player
   *
   * @param a pointer to the new properties object
   */

  void resetPlayerProperties(std::shared_ptr<AbstractPlayerProperties> p_properties = nullptr);

  std::shared_ptr<GamePlayerProperties> getPlayerProperties(){
    return mpProperties;
  }

  void detachActor(){
    mpCharacter = nullptr;
  }

  uint32_t getColour(){
    return mColour;
  }

  void setColour(uint64_t colour){
    mColour = colour;
  }

  std::string getInfoString();

protected:
  void setCharacter(std::shared_ptr<actor>);
  std::shared_ptr<GamePlayerProperties> mpProperties;
  std::shared_ptr<actor> mpCharacter;
  virtual void ping(){}
  int mId;
  unsigned int mWins = 0;

  // Default is blue
  uint64_t mColour = 0x00a0FFFF;
};

#endif
