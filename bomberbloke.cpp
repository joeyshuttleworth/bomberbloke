#include "bomberbloke.h"
#include <SDL2/SDL.h>

int _default_bomb_timer = DEFAULT_BOMB_TIMER;
double _bloke_size[2] = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};
const unsigned int _no_default_bindings = 2;

const std::vector<command_binding> _default_bindings =
  {{SDL_SCANCODE_W, "up"},
   {SDL_SCANCODE_S, "down"},
   {SDL_SCANCODE_A, "left"},
   {SDL_SCANCODE_D, "right"},
   {SDL_SCANCODE_J, "powerup"},
   {SDL_SCANCODE_K, "bomb"},
   {SDL_SCANCODE_P, "pause"}};

void bloke :: accelerate(int direction){
  int accel[2] = {0,0};
  accelerated = true;
  switch(direction){
  case UP:
    accel[1]=1;
    break;
  case DOWN:
    accel[1]=-1;
    break;
  case RIGHT:
    accel[0]=1;
    break;
  case LEFT:
    accel[0]=-1;
    break;
  case NONE:
  default:
    break;
  }
  for(int i=0; i<2; i++){
    if(accel[i] != 0){
      velocity[i] = velocity[i] + accel[i]*max_speed*ACCELERATION_RATIO;
      if(std::abs(velocity[i]) > max_speed)
	velocity[i] = max_speed * accel[i];
    }
    else{
      double decell = max_speed*DECCELERATION_RATIO;
      if(velocity[i]<0)
	decell = -decell;
      if(std::abs(decell) > std::abs(velocity[i]))
	velocity[i]=0;
      else
	velocity[i] = velocity[i] - decell;
    }
  }
  return;
}

void bloke :: handle_command(std::string command){
  if(!accelerated){
    if(command == "up"){
      accelerate(UP);
    }
    else if(command == "right"){
      accelerate(RIGHT);
    }
    else if(command == "down"){
      accelerate(DOWN);
    }
    else if(command == "left"){
      accelerate(LEFT);
    }
    else{
      accelerate(NONE);
    }
  }
  if(command == "bomb"){
    place_bomb();
  }
  return;
}

void bloke :: update(){
  accelerated = false;
}

void bloke :: place_bomb(){
  if(bombs<max_bombs){
    bomb *new_bomb = new bomb();
    new_bomb->init(this, int(position[0]),int(position[1]));
    current_level->actor_list.push_back(new_bomb);
    bombs++;
  }
  return;
}

bomb::bomb(){
  timer = _default_bomb_timer;
  return;
}

void bomb::init(bloke *bloke, int x, int y){
  placed_by = bloke;
  position[0] = x;
  position[1] = y;
  power = bloke->power;
  return;
}

void bomb::update(){
  if(timer==0){
    explode();
    placed_by->current_level->actor_list.remove(this);
    delete(this);
  }
  timer--;
  return;
}

void bomb::explode(){
  return;
}

int main (int argc, char **argv){
  command_binding tmp_binding;
  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine();
  _local_player_list.push_back(local_p(std::string("Nickname")));
  level *level1 = new level(10, 10);
  bloke *bloke1 = new bloke(level1, 0, 0);
  bloke *bloke2 = new bloke(level1, double(10) ,0);
  _local_player_list.back().character = bloke1;
  bloke2->init(); 
  bloke1->init();
  bloke1->collides = true;
  bloke1->velocity[0] = 0.01;
  bloke2->velocity[0] =-0.01;
  _window = SDL_CreateWindow("Bomberbloke", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  _surface=SDL_GetWindowSurface(_window);
  SDL_FillRect(bloke1->sprite, NULL, SDL_MapRGB(bloke1->sprite->format, 0x00, 0xFF, 0xFF));
  SDL_FillRect(_surface, NULL, SDL_MapRGB(_surface->format, 0x00, 0x00, 0xFF));
  SDL_UpdateWindowSurface(_window);
  game_loop(level1);
  SDL_Quit();
  return 0;
}

