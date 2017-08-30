#include "bomberbloke.h"

unsigned int _default_bomb_timer = DEFAULT_BOMB_TIMER;
bomb::~bomb(){
  return;
}

void bomb::init(bloke *bloke){
  collides=false;
  dim[0]=BOMB_SIZE;
  dim[1]=BOMB_SIZE;
  memset(velocity, 0, 2*sizeof(double));
  timer = _default_bomb_timer;
  placed_by = bloke;
  current_level = bloke->current_level;
  power = bloke->power;
  free(sprite);
  sprite = SDL_CreateRGBSurface(0, _zoom * dim[0], dim[1] * _zoom, 32, 0, 0, 0, 0);
  SDL_FillRect(sprite, NULL, SDL_MapRGB(sprite->format, 0xAF, 0xAF, 0x00));
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
  std::list<actor *>::iterator actor = placed_by->current_level->actor_list.begin();
  while(actor!=placed_by->current_level->actor_list.end()){
    auto prev = actor;
    actor++;
    if((*prev)!=this){
      bool dead=false;
      for(int i = 0; i<2; i++){
	if((round(position[!i])==round(placed_by->position[!i])) && (std::abs(round(position[i])-round(placed_by->position[i])) <= power)){
	  dead=true;
	  break;
	}
      }
      if(dead)
	(*prev)->handle_command("kill");
    }
  }
  remove = true;
  placed_by->bombs--;
  return;
}
