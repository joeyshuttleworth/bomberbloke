#include "CreationEvent.hpp"
#include "bomberbloke.h"
#include "bloke.hpp"
#include "bomb.hpp"
#include "Explosion.hpp"
#include "engine.hpp"
#include "actor.hpp"
#include "BigBomb.hpp"

void bomb::init(bloke *bloke){
  mDimmension[0]=BOMB_SIZE;
  mDimmension[1]=BOMB_SIZE;
  if(bloke){
    mPlacedById = bloke->mId;
    mPower = bloke->mPower;
  }
  else{
    log_message(ERR, "Bomb placed by malformed actor");
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
      if(placed_by){
        if(std::abs(mPosition[0]-placed_by->mPosition[0]) > 0.5*(mDimmension[0]+placed_by->mDimmension[0])){
          mCollides=true;
        }
        else if(std::abs(mPosition[1]-placed_by->mPosition[1]) > 0.5*(mDimmension[1]+placed_by->mDimmension[1])){
          mCollides = true;
        }
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

  if(getType() == ACTOR_BIG_BOMB)
    mPower = 100;

  if(_server){
    /*Iterate over all the sqares the bomb can reach and kill the ones if they are in the right (wrong) zone.*/

    int bomb_square[] = {int(mPosition[0]), int(mPosition[1])};

    for(unsigned int i = 0; i < 2; i++){
      for(unsigned int j = 1; j <= mPower; j++){
        std::shared_ptr<actor> square;

        if(i==0)
          square = std::make_shared<actor>(bomb_square[0] + j, bomb_square[1], 1, 1, false);
        else if(i==1)
          square = std::make_shared<actor>(bomb_square[0], bomb_square[1] + j, 1, 1, false);

        std::list<std::shared_ptr<actor>> actor_list = _pScene->ActorsCollidingWith(square.get());
        bool stopped = false;
        for(std::shared_ptr<actor> pActor : actor_list){
          if(pActor.get() == this)
            continue;
          else{
            pActor->handleCommand("+kill");
            switch(pActor->getType()){
            case ACTOR_WOODEN_CRATE:
            case ACTOR_BLOKE:
            case ACTOR_BOMB:
              stopped = true;
              break;
            default: break;
            }
          }
        }
        if(stopped)
          break;
      }
      for(unsigned int j = 0; j <= mPower; j++){
        std::shared_ptr<actor> square;

        if(i==0)
          square = std::make_shared<actor>(bomb_square[0] - j, bomb_square[1], 1, 1, false);
        else if(i==1)
          square = std::make_shared<actor>(bomb_square[0], bomb_square[1] - j, 1, 1, false);

        std::list<std::shared_ptr<actor>> actor_list = _pScene->ActorsCollidingWith(square.get());
        bool stopped = false;
        for(std::shared_ptr<actor> pActor : actor_list){
          if(pActor.get() == this)
            continue;
          else{
            pActor->handleCommand("+kill");
            switch(pActor->getType()){
            case ACTOR_WOODEN_CRATE:
            case ACTOR_BLOKE:
            case ACTOR_BOMB:
              stopped = true;
              break;
            default:
              stopped = false;
              break;
            }
          }
        }
        if(stopped)
          break;
      }
    }

    /*Cast to a bloke pointer.*/
    std::shared_ptr<bloke> placed_by = std::dynamic_pointer_cast<bloke>(_pScene->GetActor(mPlacedById));
    if(placed_by && _server){
      placed_by->mBombs--;
      if(getType() == ACTOR_BIG_BOMB){
        placed_by->mBigBombPlaced = false;
      }
    }

    /* Create an explosion particle on all clients */
    std::shared_ptr<AbstractSpriteHandler> explosion = std::make_shared<Explosion>(int(mPosition[0]), int(mPosition[1]), 1, 1);
    std::unique_ptr<AbstractEvent> c_event(new CreationEvent(explosion));
    _net_server.broadcastEvent(c_event);
  }

  return;
}
