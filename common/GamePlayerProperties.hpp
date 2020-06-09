/*This class is used by the engine to keep track of
   what powerups each player has. It allows for this
   information to be sent via a single AbstractPlayerPropertiesEvent.
   GamePlayerProperties inherits from AbstractPlayerProperties and
   is unique to the bomberbloke game.
*/


#ifndef  GAMEPLAYERPROPERTIES_HPP
#define  GAMEPLAYERPROPERTIES_HPP
#include "AbstractPlayerProperties.hpp"

class GamePlayerProperties : public AbstractPlayerProperties{
public:
  GamePlayerProperties(){
    mSpeed     = 1;
    mMaxBombs  = 1;
    mBombKick  = 1;
    mSatellite = false;
    mBigBomb   = false;
    return;
  };

  template<class Archive>
  void serialize(Archive &archive){
    archive(mSpeed, mMaxBombs, mBombKick, mSatellite, mBigBomb);
  }

  std::vector<std::string> GetProperties(){
    std::vector<std::string> rc = {std::string("TODO: player properties\n")};
    return rc;
  }
  int  mSpeed;
  int  mMaxBombs;
  int  mPower;
  bool mBombKick;
  bool mSatellite;
  bool mBigBomb;
};



#endif
