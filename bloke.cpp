#include "bomberbloke.h"

void bloke :: accelerate(int direction){
  int accel[2] = {0,0};
  mAccelerated = true;
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
    break;
  default:
    break;
  }
  for(int i=0; i<2; i++){
    if(accel[i] != 0){
      mVelocity[i] = mVelocity[i] + accel[i]*mMaxSpeed*ACCELERATION_RATIO;
      if(std::abs(mVelocity[i]) > mMaxSpeed){
	mVelocity[i] = accel[i]*mMaxSpeed;
	break;
      }
    }
    else{
      double decceleration = mMaxSpeed*0.1;
      if(mVelocity[i]<0)
	decceleration = - decceleration;
      if(std::abs(decceleration) > std::abs(mVelocity[i]))
	mVelocity[i] = 0;
      else
	mVelocity[i] -= decceleration;
    }
  }
  
  return;
}

void bloke :: handle_command(std::string command){

  
  if(command == "kill"){
    mRemove=true;
    return;
  }
  if(!mAccelerated){
    if(command == "+up"){
      direction = DIR_UP;
    }
    else if(command == "+right"){
      direction = DIR_RIGHT;
    }
    else if(command == "+down"){
      direction = DIR_DOWN;
    }
    else if(command == "+left"){
      direction = DIR_LEFT;
    }
    else if(command == "-up" && direction == DIR_UP){
      direction = DIR_NONE;
    }
    else if(command == "-down" && direction == DIR_DOWN){
      direction = DIR_NONE;
    }
    else if(command == "-up" && direction == DIR_RIGHT){
      direction = DIR_NONE;
    }
    else if(command == "-left" && direction == DIR_LEFT){
      direction = DIR_NONE;
    } 
  }
  if(command == "bomb"){
    place_bomb();
  }
  return;
}

void bloke :: update(){
  accelerate(direction);
  mAccelerated = false;
  direction = DIR_NONE;
  return;
}

void bloke :: place_bomb(){
  if(mBombs<mMaxBombs){
    bomb *new_bomb = new bomb(round(mPosition[0])+0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE), round(mPosition[1]) + 0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE));
    new_bomb->init(this);
    _level.actorList.push_back(new_bomb);
    mBombs++;
  }
  return;
}

void bloke :: kick(bomb *bomb, uint8_t direction){
  return;
}

void bloke :: init(){
  mCollides = true;
  memset(&mPosition, 0, sizeof(double)*2);
  memset(&mVelocity, 0, sizeof(double)*2);
  mPosition[0] = 5;
  mPosition[1] = 3;
  return;
}

bloke :: bloke(){
  return;
}
