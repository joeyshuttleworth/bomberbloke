#ifndef WOODENCRATE_HPP
#define WOODENCRATE_HPP
#include "staticSprite.hpp"
#include "bomberbloke.h"

class woodenCrate : public actor{
public:

  int getType() const{
    return ACTOR_WOODEN_CRATE;
  }

  woodenCrate(int x=0, int y=0) : actor(double(x), double(y), true){
  mpSpriteHandler = std::shared_ptr<staticSprite>(new staticSprite(double(x), double(y), 1.0, 1.0, "crate.png"));
    return;
  }

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(woodenCrate)
#endif