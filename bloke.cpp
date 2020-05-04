#include "bomberbloke.h"

void bloke :: accelerate(){


  if(!mDirectionsHeld[mDirectionOfAcceleration]){
    mDirectionOfAcceleration = -1; 
  }

  if(mDirectionOfAcceleration < 0){
    // Loop over directions
    for(int i = 0; i < 4; i++){
      if(mDirectionsHeld[i] == true){
	mDirectionOfAcceleration = i;
	break;   
      }
    }
  }
  
  double accel[2] = {0, 0};

  switch(mDirectionOfAcceleration){
  case DIR_UP:
    accel[1] = 1;
    break;
  case DIR_DOWN:
    accel[1] = -1;
    break;
  case DIR_LEFT:
    accel[0] = -1;
    break;
  case DIR_RIGHT:
    accel[0] = 1;
    break;
  }

  
  //Apply acceleration and decelleration in the two directions
  
  for(unsigned int i = 0; i < 2; i++){
    if(accel[i]){
      mVelocity[i] = mVelocity[i] + accel[i]*mMaxSpeed*ACCELERATION_RATIO;
      if(std::abs(mVelocity[i]) > mMaxSpeed){
	mVelocity[i] = accel[i]*mMaxSpeed;
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

  std::cout << command << "\n";
  bool key_up;
  
  
  if(command[0] == '+'){
    key_up = true;
  }
  else if(command[0] == '-'){
    key_up = false;
  }
  else{
    return;
  }
  
  if(command == "+kill"){
    mRemove=true;
    return;
  }
 
  std::string tmp_command = command.substr(1);
  if(tmp_command == "up"){
    mDirectionsHeld[DIR_UP] = key_up;
  }
  else if(tmp_command == "right"){
    mDirectionsHeld[DIR_RIGHT] = key_up;
  }
  else if(tmp_command == "down"){
    mDirectionsHeld[DIR_DOWN] = key_up;
  }
  else if(tmp_command == "left"){
    mDirectionsHeld[DIR_LEFT] = key_up;
  }
  if(command == "+bomb"){
    place_bomb();
  }
  return;
}

void bloke :: update(){
  accelerate();
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
