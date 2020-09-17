#ifndef WOODENCRATE_HPP
#define WOODENCRATE_HPP

#include <random>
#include "staticSprite.hpp"
#include "bomberbloke.h"
#include "SpeedPickup.hpp"
#include "PowerPickup.hpp"
#include "BombPickup.hpp"

class woodenCrate : public actor{
public:

  int getType() const{
    return ACTOR_WOODEN_CRATE;
  }

  woodenCrate(int x=0, int y=0) : actor(double(x), double(y), true){
  mpSpriteHandler = std::shared_ptr<staticSprite>(new staticSprite(double(x), double(y), 1.0, 1.0, "crate.png"));
    return;
  }

  void handleCommand(std::string command){
    std::list<std::string> tokens = split_to_tokens(command);
    if(tokens.size() == 0)
      return;
    else if(_server && (tokens.front() == "kill" || tokens.front() == "+kill")){
      /* TODO drop powerups and draw particle effect */
      remove();

      std::random_device rd;
      std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
      std::uniform_int_distribution<> distrib(0, 3);

      switch(distrib(gen)){
      case PICKUP_SPEED:{
        _pScene->addActor(std::make_shared<SpeedPickup>(mPosition[0], mPosition[1]));
        break;
      }
      case PICKUP_BOMB:{
        std::shared_ptr<actor> act = std::make_shared<BombPickup>(mPosition[0], mPosition[1]);
        _pScene->addActor(act);
        break;
      }
      case PICKUP_POWER:{
        std::shared_ptr<actor> act = std::make_shared<PowerPickup>(mPosition[0], mPosition[1]);
        _pScene->addActor(act);
        break;
      }
      case PICKUP_NONE:
      default: break;
      }
    }
  }
  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(woodenCrate)
#endif
