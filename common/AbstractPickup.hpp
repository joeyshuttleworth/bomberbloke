#ifndef ABSTRACTPICKUP_HPP
#define ABSTRACTPICKUP_HPP

#define PICKUP_SIZE 0.2

#include "actor.hpp"
#include "bloke.hpp"

enum{
     PICKUP_NONE,
     PICKUP_SPEED,
     PICKUP_BOMB
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

  void update(){
    if(!_server)
      return;
    std::list<std::shared_ptr<actor>> actor_list = _pScene->ActorsCollidingWith(this);
    for(auto i = actor_list.begin(); i != actor_list.end(); i++){
      if((*i)->getType() == ACTOR_BLOKE){
        log_message(DEBUG, "bloke picked up pickup");
        std::shared_ptr<bloke> bloke1 = std::dynamic_pointer_cast<bloke>(*i);
        pickup(bloke1);
        remove();
        break;
      }
    }
  }

  void handle_command(std::string command){
    if(command == "kill" || command == "+kill"){
      remove();
    }
  }

  ~AbstractPickup(){};

protected:
  virtual void pickup(std::shared_ptr<bloke>) = 0;
};

#endif
