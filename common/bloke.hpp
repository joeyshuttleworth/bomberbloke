#ifndef BLOKE_HPP
#define BLOKE_HPP
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/base_class.hpp>
#include <memory>
#include "actor.hpp"
#include "bomberbloke.h"
#include "GamePlayerProperties.hpp"
#include "PlaceHolderSprite.hpp"

class SpeedPickup;
class BombPickup;
class PowerPickup;
class bomb;
class GamePlayerProperties;
class Sound;
class SoundManager;
class BigBombPickup;

extern SoundManager soundManager;

class bloke : public actor{
  friend bomb;
  friend SpeedPickup;
  friend BombPickup;
  friend PowerPickup;
  friend BigBombPickup;
  friend GamePlayerProperties;
protected:
  std::list<int> mPowerups;

  enum DIR{
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
    DIR_NONE
  };

  void place_bomb();

  int mMaxSpeed = 1;
  int    mPower=1;
  int    mBombs=1;
  Uint8  mMaxBombs = 1;
  bool mBigBomb = false;
  bool mBigBombPlaced = false;
  std::shared_ptr<GamePlayerProperties> mProperties;
  bool   mAccelerated;
  bool   mDirectionsHeld[4] = {false, false, false, false};
  double mAcceleration[2] = {0,0};
  std::shared_ptr<Sound> mPlaceBombSound;

  SDL_Color mColour;

public:
  bloke(double x=1, double y=1, bool collides=true);

  int getType() const{
    return ACTOR_BLOKE;
  };

  std::shared_ptr<GamePlayerProperties> GetProperties(){
    return mProperties;
  }

  void draw();
  void die();
  void handleCommand(std::string command);
  void accelerate();
  void update();

  /*Cereal serialisation. No info is needed that isn't provided by actor.
    We could serialise mMaxBombs, mBombKick etc here because they are
    only used by the player's bloke. We will set them using ServerCommandEvent
    instead.
  */

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

/*This is requried by cereal for classes that use polymorphism*/
CEREAL_REGISTER_TYPE(bloke)
#endif
