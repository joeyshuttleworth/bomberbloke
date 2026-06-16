#ifndef BIGBOMB_HPP
#define BIGBOMB_HPP
#include "bomb.hpp"
class bloke;

class BigBomb : public bomb{
public:
  int getType() const {
    return ACTOR_BIG_BOMB;
  }

  using bomb::bomb;

  void init() override{
    mPower = 100;
    mpSpriteHandler = std::make_shared<staticSprite>(mrIOSystem.getGraphicsManager(),
                                                     mPosition[0], mPosition[1],
                                                     mDimension[0], mDimension[1],
                                                     "bigredbomb.png");
  }

  std::shared_ptr<actor> clone(IOSystem& io_system_ctx) override{
    return std::make_shared<BigBomb>(*this, io_system_ctx);
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
