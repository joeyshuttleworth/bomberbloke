#ifndef WOODENCRATE_HPP
#define WOODENCRATE_HPP

#include <random>
#include "staticSprite.hpp"
#include "bomberbloke.h"

class StoneBlock : public actor{
public:

  int getType() const{
    return ACTOR_STONE_BLOCK;
  }

  StoneBlock(int x=0, int y=0) : actor(double(x), double(y), true){
  mpSpriteHandler = std::shared_ptr<staticSprite>(new staticSprite(double(x), double(y), 1.0, 1.0, "stone.png"));
    return;
  }

  void handleCommand(std::string command){
    (void) command;
    return; // Stone blocks are unbreakable
  }

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(StoneBlock)

#endif
