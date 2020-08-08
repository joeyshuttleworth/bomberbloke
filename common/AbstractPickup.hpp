#ifndef ABSTRACTPICKUP_HPP
#define ABSTRACTPICKUP_HPP
#define PICKUP_SIZE 0.2
#include "actor.hpp"

enum{
     PICKUP_SPEED
};


class AbstractPickup : public actor{
public:
  AbstractPickup(double xpos, double ypos) : actor(xpos, ypos, PICKUP_SIZE, PICKUP_SIZE, false){
    /*Centralise*/
    const int square[] = {int(xpos), int(ypos)};
    for(int i = 0; i < 2; i++)
      mPosition[i] = square[i] + 0.5 - (double)PICKUP_SIZE/2.0;
    mpSpriteHandler = std::make_shared<PlaceHolderSprite>(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);
  }

  void handle_command(std::string command){
    if(command == "kill" || command == "+kill"){
      remove();
    }
  }

  ~AbstractPickup(){};

};

#endif
