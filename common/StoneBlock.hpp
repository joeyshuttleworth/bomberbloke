#pragma once

#include <random>
#include "staticSprite.hpp"
#include "bomberbloke.h"
#include "IGraphicsManager.hpp"

class StoneBlock : public actor{
public:

  int getType() const{
    return ACTOR_STONE_BLOCK;
  }

  StoneBlock(scene *scn=nullptr, int x=0, int y=0) : actor(scn, double(x), double(y), true){
    mpSpriteHandler = std::shared_ptr<staticSprite>(new staticSprite(mpGraphicsManager, double(x), double(y), 1.0, 1.0, "stone.png"));
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
