#ifndef BIGBOMBPICKUP_HPP
#define BIGBOMBPICKUP_HPP

#include "AbstractPickup.hpp"
#include "staticSprite.hpp"
#include "PickupAnimation.hpp"

class BigBombPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */

  BigBombPickup(scene *scn=nullptr, double xpos = 0, double ypos = 0) :
    AbstractPickup(scn, xpos, ypos, "bigredbomb.png")
  {
    init();
  }

  using AbstractPickup::AbstractPickup;

  virtual std::shared_ptr<actor> clone(IOSystem& io_system_ctx){
    auto pickup = std::make_shared<BigBombPickup>(*this, io_system_ctx);
    pickup->init();
    return pickup;
  };

  void pickup(std::shared_ptr<bloke> b);

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(BigBombPickup)

#endif
