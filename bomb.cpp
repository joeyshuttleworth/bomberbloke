#include "bomberbloke.h"

unsigned int _default_bomb_timer = DEFAULT_BOMB_TIMER;

bomb::~bomb(){
  return;
}

bomb::bomb(){
  if(sprite)
    SDL_DestroyTexture(sprite);
  sprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, 0, 128, 128);
  SDL_SetRenderTarget(_renderer, sprite);
  SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0x00);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_SetRenderTarget(_renderer, NULL);
  return;
}

void bomb::init(bloke *bloke){
  collides=false;
  dim[0]=BOMB_SIZE;
  dim[1]=BOMB_SIZE;
  memset(velocity, 0, 2*sizeof(double));
  timer = _default_bomb_timer;
  placed_by = bloke;
  power = bloke->power;
  return;
}

void bomb::handle_command(std::string command){
  if(remove==false){
    if(command=="kill"){
      explode();
    }
  }
  return;
}

void bomb::update(){
  if(collides == false){
    for(int i=0;i<2;i++){
      if(std::abs(position[i]-placed_by->position[i]) > 0.5*(dim[i]+placed_by->dim[i])){
       	collides=true;
	break;
      }
    }
  }
  if(timer==0)
    explode();
  else
    timer--;
  return;
}

void bomb::explode(){
  std::list<actor>::iterator actor = _level.actor_list.begin();
  while(actor!=_level.actor_list.end()){
    auto prev = actor;
    actor++;
    if(&(*prev)!=((void*)this)){
      bool dead=false;
      for(int i = 0; i<2; i++){
	if((round(position[!i])==round(prev->position[!i])) && (std::abs(round(position[i])-round(prev->position[i])) <= power)){
	  dead=true;
	  break;
	}
      }
      if(dead)
	prev->handle_command("kill");
    }
  }
  remove = true;
  placed_by->bombs--;
  return;
}
