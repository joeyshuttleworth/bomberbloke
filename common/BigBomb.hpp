#ifndef BIGBOMB_HPP
#define BIGBOMB_HPP
#include "bomb.hpp"
class bloke;

class BigBomb : public bomb{
public:
  int getType() const {
    return ACTOR_BIG_BOMB;
  }

  BigBomb() : bomb(){
    mpSpriteHandler = std::make_shared<staticSprite>(mPosition[0], mPosition[1], BOMB_SIZE, BOMB_SIZE, "bigredbomb.png");
    mPower = 100;
  }
  BigBomb(bloke* b) : bomb(b){
    BigBomb();
  }

  /*Used by cereal to serialize the event for it to be sent/received*/
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this), mTimer, mSatellite, mBigBomb);
    return;
  }

};

CEREAL_REGISTER_TYPE(BigBomb)
#endif
