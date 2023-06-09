/*This class is used by the engine to keep track of
   what powerups each player has. It allows for this
   information to be sent via a single AbstractPlayerPropertiesEvent.
   GamePlayerProperties inherits from AbstractPlayerProperties and
   is unique to the bomberbloke game.
*/


#ifndef  GAMEPLAYERPROPERTIES_HPP
#define  GAMEPLAYERPROPERTIES_HPP
#include "AbstractPlayerProperties.hpp"
#include <cereal/cereal.hpp>

class bloke;

class GamePlayerProperties : public AbstractPlayerProperties{
public:
  GamePlayerProperties(){
    mSpeed     = 1;
    mMaxBombs  = 1;
    mBombKick  = 1;
    mSatellite = false;
    mBigBomb   = false;
    mPower = 1;
    return;
  };

  GamePlayerProperties(std::shared_ptr<bloke> b);
  void reset(std::shared_ptr<AbstractPlayerProperties> p_properties){
    std::shared_ptr<GamePlayerProperties> gp_properties = std::dynamic_pointer_cast<GamePlayerProperties>(p_properties);
    if(gp_properties)
      *this = GamePlayerProperties(*gp_properties);
    else
      *this = GamePlayerProperties();
  }

  std::vector<std::string> GetProperties(){
    std::vector<std::string> rc = {std::string("TODO: player properties\n")};
    return rc;
  }

  int  mSpeed;
  int  mMaxBombs;
  int  mPower;
  int  mLastPingMeasurement;
  bool mBombKick;
  bool mSatellite;
  bool mBigBomb;


  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::make_nvp("speed", mSpeed),
            cereal::make_nvp("power", mPower),
            cereal::make_nvp("Max bombs", mMaxBombs),
            cereal::make_nvp("bomb kick", mBombKick),
            cereal::make_nvp("satellite", mSatellite),
            cereal::make_nvp("bigbomb", mBigBomb));
  }

  virtual ~GamePlayerProperties(){}

};



#endif
