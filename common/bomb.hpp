#ifndef BOMB_HPP
#define BOMB_HPP

#include "actor.hpp"
#include "bloke.hpp"
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include "staticSprite.hpp"
#include "AbstractSpriteHandler.hpp"
#include "Sound.hpp"

struct BombPath {
  std::vector<std::pair<int, int>> squares;
};

class bomb : public actor {
 protected:
  static constexpr double _bomb_delta = 0.01;
  bool mInitialised = false;

  int  mPlacedById = 0;

  unsigned int mTimer = DEFAULT_BOMB_TIMER;
  bool mBigBomb = false;
  bool mPenetration = false;
  Uint8 mPower = 2;
  bool mSatellite = false;

  std::vector<BombPath> identifyTargetSquares();

 public:
  /*Cereal serialisation*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this), mPower, mTimer, mSatellite, mBigBomb);
    return;
  }

  void init(bloke*);

  void init(){
    mpSpriteHandler = std::make_shared<staticSprite>(mrIOSystem.getGraphicsManager(), mPosition[0],
                                                     mPosition[1], BOMB_SIZE, BOMB_SIZE, "bomb.png");
  }

  void explode();
  void update();
  void handleCommand(std::string command);

  bomb(scene* scn, actor& placed_by) : actor(scn, int(placed_by.mPosition[0] + placed_by.mDimension[0]/2) + 0.5 - BOMB_SIZE/2.0, int(placed_by.mPosition[1] + placed_by.mDimension[1]/2) + 0.5 - BOMB_SIZE/2.0, BOMB_SIZE, BOMB_SIZE, false){
    mPlacedById = placed_by.getId();
    bomb();
    return;
  };

  bomb(scene *scn=nullptr, double x=0, double y=0) : actor(scn, x, y, BOMB_SIZE, BOMB_SIZE, true){
  }

  bomb(bomb& other) :
    actor(other),
    mPlacedById(other.mPlacedById),
    mTimer(other.mTimer),
    mBigBomb(other.mBigBomb),
    mPenetration(other.mPenetration),
    mPower(other.mPower),
    mSatellite(other.mSatellite)
  {
  }

  bomb(bomb& other, IOSystem& io_system_ctx) :
    actor(other, io_system_ctx),
    mPlacedById(other.mPlacedById),
    mTimer(other.mTimer),
    mBigBomb(other.mBigBomb),
    mPenetration(other.mPenetration),
    mPower(other.mPower),
    mSatellite(other.mSatellite)
  {
  }

  virtual std::shared_ptr<actor> clone(IOSystem& io_system_ctx) override{
    return std::make_shared<bomb>(*this, io_system_ctx);
  }

  int getType() const{
    return ACTOR_BOMB;
  };
};

/*This is required for classes using polymorphism*/
CEREAL_REGISTER_TYPE(bomb)
#endif
