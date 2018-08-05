#include "bomberbloke.h"

void bloke :: accelerate(int direction){
  int accel[2] = {0,0};
  accelerated = true;
  switch(direction){
  case DIR_UP:
    accel[1]=1;
    break;
  case DIR_DOWN:
    accel[1]=-1;
    break;
  case DIR_RIGHT:
    accel[0]=1;
    break;
  case DIR_LEFT:
    accel[0]=-1;
    break;
  case DIR_NONE:
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
  if(command == "kill"){
    remove=true;
    return;
  }
  if(!accelerated){
    if(command == "up"){
      accelerate(DIR_UP);
    }
    else if(command == "right"){
      accelerate(DIR_RIGHT);
    }
    else if(command == "down"){
      accelerate(DIR_DOWN);
    }
    else if(command == "left"){
      accelerate(DIR_LEFT);
    }
    else{
      accelerate(DIR_NONE);
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
    bomb *new_bomb = new bomb(current_level, round(position[0])+0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE), round(position[1]) + 0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE));
    new_bomb->init(this);
    bombs++;
  }
  return;
}

void bloke :: kick(bomb *bomb, uint8_t direction){
  return;
}

void bloke :: init(){
  collides = true;
  sprite = SDL_CreateRGBSurface(0, _zoom * dim[0],dim[1] * _zoom ,32, 0, 0, 0, 0);
  return;
}

