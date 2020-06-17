#include "bomberbloke.h"
#include "bloke.hpp"
#include "bomb.hpp"
#include "Explosion.hpp"

unsigned int _default_bomb_timer = DEFAULT_BOMB_TIMER;

void bomb::init(bloke *bloke){
  mDimmension[0]=BOMB_SIZE;
  mDimmension[1]=BOMB_SIZE;
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 128, 128);
  SDL_SetRenderTarget(_renderer, mpSprite);
  SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_SetRenderTarget(_renderer, NULL);
  mCollides = false;
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
      std::shared_ptr<actor> placed_by = _level.GetActor(mPlacedById);
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
  auto actor = _level.mActors.begin();

  if(_server){
    /*Iterate over all actors and kill the ones if they are in the right (wrong) zone.*/
    while(actor!=_level.mActors.end()){
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
  _level.mParticleList.push_back(std::shared_ptr<Explosion>(new Explosion(mPosition[0] - 0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE), mPosition[1] - 0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE), 1 ,1)));

  mRemove = true;

  /*Cast to a bloke pointer.*/
  std::shared_ptr<bloke> placed_by = std::dynamic_pointer_cast<bloke>(_level.GetActor(mPlacedById));
  if(placed_by)
    placed_by->mBombs--;
  return;
}
