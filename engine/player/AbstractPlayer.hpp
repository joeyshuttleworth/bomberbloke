/*Class to store info for players. These may
  be clients connected to a server, or localPlayers
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

class actor;

class AbstractPlayer{
public:
  AbstractPlayer(std::string);
  AbstractPlayer();
  std::string mNickname;
  int getId(){
    return mId;
  }
  std::shared_ptr<actor> mpCharacter;
protected:
  int mType;
  virtual void ping() = 0;
  void init(actor*);
  double mPingElapsedTime = 0;
  unsigned int mLastPingSent = 0;
  int mId;
  double getPing(){
    return mPingElapsedTime;
  }
  /*Cereal serialisation*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(mNickname);
    return;
  }
};
#endif
