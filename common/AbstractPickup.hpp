#ifndef ABSTRACTPICKUP_HPP
#define ABSTRACTPICKUP_HPP

#define PICKUP_SIZE 0.4

#include "PlaceHolderSprite.hpp"
#include "bomberbloke_actors.hpp"
#include "actor.hpp"
#include "AbstractPlayer.hpp"

class bloke;
class scene;

class AbstractPickup : public actor{
public:

  enum{
    PICKUP_NONE,
    PICKUP_SPEED,
    PICKUP_BOMB,
    PICKUP_POWER,
    PICKUP_BIG_BOMB
  };

  AbstractPickup(scene* scn, double xpos = 0, double ypos = 0, const std::string& asset_name="") :
    actor(scn, xpos, ypos, PICKUP_SIZE, PICKUP_SIZE, false),
    mAssetName(asset_name)
  {
    /*Centralise*/
    const int square[] = {int(xpos), int(ypos)};

    for(int i = 0; i < 2; i++)
      mPosition[i] = square[i] + 0.5 - (double)PICKUP_SIZE/2.0;
  }

  AbstractPickup(AbstractPickup& other) :
    actor(other),
    mAssetName(other.mAssetName)
  {
    init();
  }

  AbstractPickup(AbstractPickup& other, IOSystem& ctx) :
    actor(other, ctx),
    mAssetName(other.mAssetName)
  {
    init();
  }

  void update();

  void init();

  void handleCommand(std::string command){
    if(command == "kill" || command == "+kill"){
      remove();
    }
  }


  virtual ~AbstractPickup(){};

protected:
  virtual void pickup(std::shared_ptr<bloke>) = 0;

  const std::string mAssetName = "";
};

#endif
