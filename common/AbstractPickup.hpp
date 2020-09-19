#ifndef ABSTRACTPICKUP_HPP
#define ABSTRACTPICKUP_HPP

#define PICKUP_SIZE 0.4

#include "PlaceHolderSprite.hpp"
#include "bomberbloke_actors.hpp"
#include "scene.hpp"
#include "actor.hpp"

enum{
     PICKUP_NONE,
     PICKUP_SPEED,
     PICKUP_BOMB,
     PICKUP_POWER,
     PICKUP_BIG_BOMB
};


class AbstractPickup : public actor{
public:
  AbstractPickup(double xpos = 0, double ypos = 0) : actor(xpos, ypos, PICKUP_SIZE, PICKUP_SIZE, false){
    /*Centralise*/
    const int square[] = {int(xpos), int(ypos)};

    for(int i = 0; i < 2; i++)
      mPosition[i] = square[i] + 0.5 - (double)PICKUP_SIZE/2.0;
  }

  void update();

  void handleCommand(std::string command){
    if(command == "kill" || command == "+kill"){
      remove();
    }
  }

  virtual ~AbstractPickup(){};

protected:
  virtual void pickup(std::shared_ptr<bloke>) = 0;
};

#endif
