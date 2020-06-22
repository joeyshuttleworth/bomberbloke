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

class actor;

class AbstractPlayer{
public:
  AbstractPlayer(std::string);
  AbstractPlayer(){};
  ~AbstractPlayer(){};

  std::string mNickname;

  int getId(){
    return mId;
  }

  void setCharacter(std::shared_ptr<actor>);
  std::shared_ptr<actor> getCharacter(){
    return mpCharacter;
  }

protected:
  int mType;
  std::shared_ptr<actor> mpCharacter;
  virtual void ping() = 0;
  double mPingElapsedTime = 0;
  unsigned int mLastPingSent = 0;
  int mId;
  double getPing(){
    return mPingElapsedTime;
  }

};

#endif
