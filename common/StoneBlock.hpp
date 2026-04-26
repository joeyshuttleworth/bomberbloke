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

  using actor::actor;

  void init(){
    mpSpriteHandler = std::shared_ptr<staticSprite>(new staticSprite(mrIOSystem.getGraphicsManager(),
                                                                     double(mPosition[0]),
                                                                     double(mPosition[1]),
                                                                     mDimension[0], mDimension[1],
                                                                     "stone.png"));
    return;
    actor::init();
  }

  void handleCommand(std::string command){
    (void) command;
    return; // Stone blocks are unbreakable
  }

  std::shared_ptr<actor> clone(IOSystem& ctx) override{
    return std::make_shared<StoneBlock>(*this, ctx);
  }

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(StoneBlock)
