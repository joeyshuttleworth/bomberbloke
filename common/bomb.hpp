#ifndef BOMB_HPP
#define BOMB_HPP

#include "actor.hpp"
#include "bloke.hpp"
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include "PlaceHolderSprite.hpp"
#include "AbstractSpriteHandler.hpp"
#include "Sound.hpp"

const int N_EXPLOSION_SOUNDS = 2;
const std::string EXPLOSION_SOUND_NAMES[N_EXPLOSION_SOUNDS] = { "bomb_1", "bomb_2" };

class bomb : public actor {
 protected:
  Uint8 mTimer;
  Uint8 mPower = 2;
  int  mPlacedById = 0;
  bool mPenetration;
  bool mBigBomb;
  bool mSatellite;

  std::array<std::shared_ptr<Sound>, N_EXPLOSION_SOUNDS> explosionSounds;
 public:

  /*Cereal serialisation*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this), mPower, mTimer, mSatellite, mBigBomb);
    return;
  }

  void init(bloke*);
  void explode();
  void update();
  void handle_command(std::string command);

  bomb() : actor(0, 0, BOMB_SIZE, BOMB_SIZE){
    mpSpriteHandler = std::shared_ptr<AbstractSpriteHandler>(new PlaceHolderSprite(mPosition[0], mPosition[1], mDimmension[0], mDimmension[0]));
  }

  bomb(std::shared_ptr<actor> placed_by) : actor(placed_by->mPosition[0], placed_by->mDimmension[1], BOMB_SIZE, BOMB_SIZE, false){
    if(placed_by)
      mPlacedById = placed_by->getId();
    mpSpriteHandler = std::shared_ptr<AbstractSpriteHandler>(new PlaceHolderSprite(mPosition[0], mPosition[1], mDimmension[0], mDimmension[0]));
    return;
  };

  int getType() const{
    return ACTOR_BOMB;
  };
  using actor::actor;
};

/*This is required for classes using polymorphism*/
CEREAL_REGISTER_TYPE(bomb)
#endif
