#ifndef BLOKE_HPP
#define BLOKE_HPP
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/base_class.hpp>
#include "actor.hpp"
#include "GamePlayerProperties.hpp"
#include "PlaceHolderSprite.hpp"

class bloke : public actor{
  friend bomb;
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
  double mMaxSpeed = double(DEFAULT_MAX_SPEED);
  int    mBombs=0;
  Uint8  mMaxBombs = 1;
  GamePlayerProperties mProperties;
  bool   mAccelerated;
  bool   mDirectionsHeld[4] = {false, false, false, false};
  double mAcceleration[2] = {0,0};

 public:

  bloke(double x=1, double y=1, bool collides = true) : actor(x, y, DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE, true){
    mCollides = collides;
    mPosition[0]=x;
    mPosition[1]=y;
    mpSpriteHandler = std::make_shared<PlaceHolderSprite>(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
    return;
  };

  int getType() const{
    return ACTOR_BLOKE;
  };

  GamePlayerProperties GetProperties(){
    return mProperties;
  }

  void draw();
  void die();
  void handle_command(std::string command);
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
