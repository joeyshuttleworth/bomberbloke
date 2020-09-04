#include "CreateEvent.hpp"
#include "bomberbloke.h"
#include "bloke.hpp"
#include "bomb.hpp"
#include "Explosion.hpp"
#include "engine.hpp"
#include "actor.hpp"

void bomb::init(bloke *bloke){
  mDimmension[0]=BOMB_SIZE;
  mDimmension[1]=BOMB_SIZE;
  if(bloke){
    mPlacedById = bloke->mId;
    mPower = bloke->GetProperties()->mPower;
  }
  else{
    log_message(ERROR, "Bomb placed by malformed actor");
  }

  return;
}

void bomb::handleCommand(std::string command){
  if(mRemove==false){
    if(command == "+kill" && _server){
      explode();
    }
  }
  return;
}

void bomb::update(){
  /*Bomb collision is only turned on when the actor which placed it has
    moved away*/
  if(mPlacedById != 0){
    if(mCollides == false){
      std::shared_ptr<actor> placed_by = _pScene->GetActor(mPlacedById);
      /*TODO this is wrong. Use a corner or midpoint here*/
        if(std::abs(mPosition[0]-placed_by->mPosition[0]) > 0.5*(mDimmension[0]+placed_by->mDimmension[0])){
        mCollides=true;
      }
      else if(std::abs(mPosition[1]-placed_by->mPosition[1]) > 0.5*(mDimmension[1]+placed_by->mDimmension[1])){
        mCollides = true;
      }
    }
  }
    if(mTimer==0)
      explode();
    else
      mTimer--;
    return;
}

void bomb::explode(){
  remove();
  if(_server){
    /*Iterate over all actors and kill the ones if they are in the right (wrong) zone.*/
    const std::list<std::shared_ptr<actor>> actor_list = _pScene->mActors;
      for(auto i = actor_list.begin(); i != actor_list.end(); i++){
        /* Do not kill this bomb*/
        if(i->get() == this)
          continue;
        int bomb_square[] = {int(mPosition[0]), int(mPosition[1])};
        int actor_square[] = {int((*i)->mPosition[0] + (*i)->mDimmension[0]/2), int((*i)->mPosition[1] + (*i)->mDimmension[1]/2)};
       /* Check we are in the blast zone - if so set dead to true */
        for(int j = 0; j < 2; j++){
          if(bomb_square[j] == actor_square[j] && std::abs(bomb_square[!j] - actor_square[!j]) <= mPower+1){
            (*i)->handleCommand("+kill");
            break;
          }
        }
      }
    /*Cast to a bloke pointer.*/
    std::shared_ptr<bloke> placed_by = std::dynamic_pointer_cast<bloke>(_pScene->GetActor(mPlacedById));
    if(placed_by && _server)
      placed_by->mBombs--;

    /* Create an explosion particle on all clients */
    std::shared_ptr<AbstractSpriteHandler> explosion = std::make_shared<Explosion>(int(mPosition[0]), int(mPosition[1]), 1, 1);
    std::unique_ptr<AbstractEvent> c_event(new CreateEvent(explosion));
    _net_server.broadcastEvent(c_event);
  }

  return;
}
