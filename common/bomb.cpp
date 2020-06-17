#include "bomberbloke.h"
#include "bloke.hpp"
#include "bomb.hpp"
#include "Explosion.hpp"

unsigned int _default_bomb_timer = DEFAULT_BOMB_TIMER;

void bomb::init(bloke *bloke){
  mDimmension[0]=BOMB_SIZE;
  mDimmension[1]=BOMB_SIZE;
  std::shared_ptr<PlaceHolderSprite> sprite(new  PlaceHolderSprite(mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]));
  mpSpriteHandler = std::dynamic_pointer_cast<AbstractSpriteHandler>(sprite);
  mCollides = false;
  memset(mVelocity, 0, 2*sizeof(double));
  mTimer = _default_bomb_timer;
  if(bloke){
    mPlacedById = bloke->mId;
    mPower = bloke->GetProperties().mPower;
  }
  else{
    log_message(ERROR, "Bomb placed by malformed actor");
  }
  return;
}

void bomb::handle_command(std::string command){
  if(mRemove==false){
    if(command=="+kill"){
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
      std::shared_ptr<actor> placed_by = _pLevel->GetActor(mPlacedById);
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
  auto actor = _pLevel->mActors.begin();

  if(_server){
    /*Iterate over all actors and kill the ones if they are in the right (wrong) zone.*/
    while(actor!=_pLevel->mActors.end()){
      auto prev = *actor;
      actor++;
      /* Do not kill this bomb*/
      if(prev.get() != this){
        bool dead=false;

        /* Check we are in the kill zone - if so set dead to true */
        for(int i = 0; i < 2; i++){
          if((round(mPosition[!i])==round(prev->mPosition[!i])) && (std::abs(round(mPosition[i])-round(prev->mPosition[i])) <= mPower)){
            dead = true;
            break;
          }
        }
        if(dead)
          prev->handle_command("kill");
      }
    }
  }
  _pLevel->mParticleList.push_back(std::shared_ptr<Explosion>(new Explosion(mPosition[0] - 0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE), mPosition[1] - 0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE), 1 ,1)));

  mRemove = true;

  /*Cast to a bloke pointer.*/
  std::shared_ptr<bloke> placed_by = std::dynamic_pointer_cast<bloke>(_pLevel->GetActor(mPlacedById));
  if(placed_by)
    placed_by->mBombs--;

  /*  Rumble effect */
  _pCamera->rumble();

  return;
}
