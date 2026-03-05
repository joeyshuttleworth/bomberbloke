#ifndef BIGBOMBPICKUP_HPP
#define BIGBOMBPICKUP_HPP

#include "AbstractPickup.hpp"
#include "staticSprite.hpp"
#include "PickupAnimation.hpp"

class BigBombPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */

  BigBombPickup(scene *scn=nullptr, double xpos = 0, double ypos = 0) : AbstractPickup(scn, xpos, ypos){

    if(!scn)
      return;

    auto gfx_manager = scn->getGraphicsManager();
    /*TODO: draw asset*/
    mpSpriteHandler = std::make_shared<PickupAnimation>(gfx_manager, mPosition[0], mPosition[1], mDimension[0], mDimension[1], "bigredbomb.png");
    return;
  }

  void pickup(std::shared_ptr<bloke> b);

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(BigBombPickup)

#endif
