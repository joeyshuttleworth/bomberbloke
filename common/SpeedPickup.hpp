#ifndef SPEEDPICKUP_HPP
#define SPEEDPICKUP_HPP
#include "AbstractPickup.hpp"
#include "PickupAnimation.hpp"

class SpeedPickup : public AbstractPickup{
public:
  int getType() const{return PICKUP_SPEED;}

  /* Need a default constructor for cereal */
  SpeedPickup(scene* scn=nullptr, double xpos = 0, double ypos = 0) :
    AbstractPickup(scn, xpos, ypos, "lightning.png"){
    init();
  }

  using AbstractPickup::AbstractPickup;

  void pickup(std::shared_ptr<bloke> b);

  virtual std::shared_ptr<actor> clone(IOSystem& io_system_ctx){
    auto pickup = std::make_shared<SpeedPickup>(*this, io_system_ctx);
    return pickup;
  };

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(SpeedPickup)
#endif
