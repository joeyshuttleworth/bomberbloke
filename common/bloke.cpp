#include "bomberbloke.h"

void bloke :: accelerate(){
  /*Count how many directions we are accelerating in*/
  int directions_accelerated_in = (mDirectionOfAcceleration[1] != 0)+ (mDirectionOfAcceleration[0] != 0);

  const double velocity_increase = (directions_accelerated_in==1)? mMaxSpeed*ACCELERATION_RATIO: mMaxSpeed*ACCELERATION_RATIO * 0.5;

  for(int i=0; i<2; i++){
    if(mDirectionOfAcceleration[i] != 0){
      mVelocity[i] = mVelocity[i] + mDirectionOfAcceleration[i]*velocity_increase;
    }
    else{
      double decceleration = (mVelocity[i]>0)?-mMaxSpeed*0.5: mMaxSpeed*0.5;
      if(std::abs(decceleration) > std::abs(mVelocity[i]))
        mVelocity[i] = 0;
      else
        mVelocity[i] += decceleration;
    }
  }
  return;
}

void bloke :: handle_command(std::string command){

  std::cout << command << "\n";
  /*True if the key is pressed down- false if it is up*/
  bool key_down = (command[0]=='+');
  if(command == "+kill"){
    mRemove=true;
    return;
  }
  const std::string direction_strings[4] = {"up", "right", "down", "left"};

  for(unsigned int i = 0; i < 4; i++){
    if(command.substr(1) == direction_strings[i]){
      if(key_down){
        if(mDirectionsHeld[i] == false){
          mDirectionsHeld[i] = true;
          mDirectionOfAcceleration[(i+1)%2] = (i<2)?1:-1;
        }
      }
      else{
        if(mDirectionsHeld[i] == true){
          mDirectionsHeld[i] = false;
          if(mDirectionsHeld[(i+2)%4])
            mDirectionOfAcceleration[(i + 1)%2] = (i<2)?1:-1;
          else{
            mDirectionOfAcceleration[(i+1)%2] = 0;
          }
        }
      }
    }
  }

  if(command == "bomb"){
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
