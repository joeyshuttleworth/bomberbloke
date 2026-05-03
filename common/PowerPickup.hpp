#ifndef POWERPICKUP_HPP
#define POWERPICKUP_HPP
#include "AbstractPickup.hpp"
#include "staticSprite.hpp"
#include "bomberbloke_actors.hpp"
#include "PowerPickup.hpp"
#include "PickupAnimation.hpp"

class PowerPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_POWER;}

  /* Need a default constructor for cereal */

  PowerPickup(scene *scn=nullptr, double xpos = 0, double ypos = 0) :
    AbstractPickup(scn, xpos, ypos, "flames.png"){
    /*TODO: draw asset*/
    init();
    return;
  }

  using AbstractPickup::AbstractPickup;

  virtual std::shared_ptr<actor> clone(IOSystem& io_system_ctx){
    auto pickup = std::make_shared<PowerPickup>(*this, io_system_ctx);
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

CEREAL_REGISTER_TYPE(PowerPickup)
#endif
