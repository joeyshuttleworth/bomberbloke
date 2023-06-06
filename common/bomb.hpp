#ifndef BOMB_HPP
#define BOMB_HPP

#include "actor.hpp"
#include "bloke.hpp"
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include "staticSprite.hpp"
#include "AbstractSpriteHandler.hpp"
#include "Sound.hpp"

typedef std::shared_ptr<actor> TargetSquare;

class bomb : public actor {
 protected:
  unsigned int mTimer = DEFAULT_BOMB_TIMER;
  Uint8 mPower = 2;
  int  mPlacedById = 0;
  bool mPenetration;
  bool mBigBomb;
  bool mSatellite;
  bool mInitialised = false;
  std::vector<std::vector<BombSquare>> identifyTargetSquares();

 public:
  /*Cereal serialisation*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this), mPower, mTimer, mSatellite, mBigBomb);
    return;
  }

  void init(bloke*);

  void init(){}

  void explode();
  void update();
  void handleCommand(std::string command);

  bomb(actor* placed_by) : actor(int(placed_by->mPosition[0] + placed_by->mDimmension[0]/2) + 0.5 - BOMB_SIZE/2.0, int(placed_by->mPosition[1] + placed_by->mDimmension[1]/2) + 0.5 - BOMB_SIZE/2.0, BOMB_SIZE, BOMB_SIZE, false){
    if(placed_by)
      mPlacedById = placed_by->getId();
    bomb();
    return;
  };

  bomb() : actor(0,0, BOMB_SIZE, BOMB_SIZE){
    mpSpriteHandler = std::make_shared<staticSprite>(mPosition[0], mPosition[1], BOMB_SIZE, BOMB_SIZE, "bomb.png");
  }

  int getType() const{
    return ACTOR_BOMB;
  };
};

/*This is required for classes using polymorphism*/
CEREAL_REGISTER_TYPE(bomb)
#endif
