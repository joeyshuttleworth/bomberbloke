#ifndef ABSTRACTPICKUP_HPP
#define ABSTRACTPICKUP_HPP

#define PICKUP_SIZE 0.5

#include "PlaceHolderSprite.hpp"
#include "bomberbloke_actors.hpp"
#include "scene.hpp"
#include "actor.hpp"

enum{
     PICKUP_NONE,
     PICKUP_SPEED,
     PICKUP_BOMB,
     PICKUP_POWER
};


class AbstractPickup : public actor{
public:
  AbstractPickup(double xpos = 0, double ypos = 0) : actor(xpos, ypos, PICKUP_SIZE, PICKUP_SIZE, false){
    /*Centralise*/
    const int square[] = {int(xpos), int(ypos)};
    for(int i = 0; i < 2; i++)
      mPosition[i] = square[i] + 0.5 - (double)PICKUP_SIZE/2.0;
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
