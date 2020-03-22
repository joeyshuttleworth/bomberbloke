#include "bomberbloke.h"

unsigned int _default_bomb_timer = DEFAULT_BOMB_TIMER;

bomb::~bomb(){
  return;
}

bomb::bomb(){
  if(mpSprite)
    SDL_DestroyTexture(mpSprite);
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, 0, 128, 128);
  SDL_SetRenderTarget(_renderer, mpSprite);
  SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0x00);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_SetRenderTarget(_renderer, NULL);
  return;
}

void bomb::init(bloke *bloke){
  mCollides = false;
  mDimmension[0]=BOMB_SIZE;
  mDimmension[1]=BOMB_SIZE;
  memset(mVelocity, 0, 2*sizeof(double));
  mTimer = _default_bomb_timer;
  mpPlacedBy = bloke;
  mPower = mpPlacedBy->getPower();
  return;
}

void bomb::handle_command(std::string command){
  if(mRemove==false){
    if(command=="kill"){
      explode();
    }
  }
  return;
}

void bomb::update(){
  /*Bomb collision is only turned on when the actor which placed it has 
    moved away*/
  if(mCollides == false){
    if(std::abs(mPosition[0]-mpPlacedBy->mPosition[0]) > 0.5*(mDimmension[0]+mpPlacedBy->mDimmension[0])){
      mCollides=true;
    }
    else if(std::abs(mPosition[1]-mpPlacedBy->mPosition[1]) > 0.5*(mDimmension[1]+mpPlacedBy->mDimmension[1])){
      mCollides = true;
    }
  }
  if(mTimer==0)
    explode();
  else
    mTimer--;
  return;
}

void bomb::explode(){
  std::list<actor*>::iterator actor = _level.actorList.begin();
  /*Iterate over all actors and kill them*/
  while(actor!=_level.actorList.end()){
    auto prev = *actor;
    actor++;
    /* Do not kill this bomb yet*/
    if(*actor != this){
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
  mRemove = true;
  mpPlacedBy->mBombs--;
  return;
}
