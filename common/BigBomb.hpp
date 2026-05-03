#ifndef BIGBOMB_HPP
#define BIGBOMB_HPP
#include "bomb.hpp"
class bloke;

class BigBomb : public bomb{
public:
  int getType() const {
    return ACTOR_BIG_BOMB;
  }

  BigBomb(scene *scn=nullptr, double x=0, double y=0) : bomb(){
    if(scn){
      IGraphicsManager& gfx_manager = scn->getIOSystem().getGraphicsManager();
      mpSpriteHandler = std::make_shared<staticSprite>(gfx_manager, x, y, BOMB_SIZE, BOMB_SIZE, "bigredbomb.png");
    }

    mPower = 100;
  }

  BigBomb(scene *scn, bloke& b) : bomb(scn, b){
    BigBomb();
    mPower = 100;
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
