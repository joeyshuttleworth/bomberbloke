#include "bomberbloke.h"
#include "CommandEvent.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include <sstream>
#include <cereal/archives/json.hpp>

void bloke :: accelerate(){
  /*Count how many directions we are accelerating in*/
  double acceleration_magnitude =  pow(mAcceleration[1]*mAcceleration[1] + mAcceleration[0]*mAcceleration[0], 0.5);
  

  const double velocity_increase[2] = {mMaxSpeed*ACCELERATION_RATIO*mAcceleration[0]/acceleration_magnitude, mMaxSpeed*ACCELERATION_RATIO*mAcceleration[1]/acceleration_magnitude};

  for(int i=0; i<2; i++){
    if(mAcceleration[i] != 0){
      mVelocity[i] = mVelocity[i] + velocity_increase[i];
    }
    else{
      double decceleration = (mVelocity[i]>0)?-mMaxSpeed*0.5: mMaxSpeed*0.5;
      if(std::abs(decceleration) > std::abs(mVelocity[i]))
        mVelocity[i] = 0;
      else
        mVelocity[i] += decceleration;
    }
  }
  /*Finally, if we are a client, construct a command event
    to send this move command to the server*/
  std::stringstream command;
  command << "accel " << mAcceleration[0] << " " << mAcceleration[1];
  CommandEvent event(command.str());
  // Output in JSON for debugging
  cereal::JSONOutputArchive oArchive(std::cout);
  //oArchive(event);
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

  /*Handle movement commands*/
  const std::string direction_strings[4] = {"up", "right", "down", "left"};

  for(unsigned int i = 0; i < 4; i++){
    if(command.substr(1) == direction_strings[i]){
      if(key_down){
        if(mDirectionsHeld[i] == false){
          mDirectionsHeld[i] = true;
          mAcceleration[(i+1)%2] = (i<2)?1:-1;
        }
      }
      else{
        if(mDirectionsHeld[i] == true){
          mDirectionsHeld[i] = false;
          if(mDirectionsHeld[(i+2)%4])
            mAcceleration[(i + 1)%2] = (i<2)?-1:1;
          else{
            mAcceleration[(i+1)%2] = 0;
          }
        } 
      }
    }
  }
  if(command == "+bomb"){// && _server){
    place_bomb();
  }

  /*Command(s) that take parameters*/
  std::list<std::string> tokens = split_to_tokens(command);

  if(tokens.back() == "accel" && tokens.size() == 3){
    auto counter = tokens.begin();
    double x_accel = std::stod(*counter);
    counter++;
    double y_accel = std::stod(*counter);
    if(std::abs(x_accel) <= 1 && std::abs(y_accel) <= 1){
      mAcceleration[0] = x_accel;
      mAcceleration[0] = y_accel;
    }
  }

  return;
}

void bloke :: update(){
  accelerate();
  return;
}

void bloke :: place_bomb(){
  if(mBombs<mMaxBombs){
    std::shared_ptr<bomb> new_bomb(new bomb(round(mPosition[0])+0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE), round(mPosition[1]) + 0.5*(DEFAULT_BLOKE_SIZE - BOMB_SIZE)));
    new_bomb->init(this);
    _level.mActors.push_back(new_bomb);
    /*Output the serilisation of the bomb to std::cout*/
    cereal::JSONOutputArchive oArchive(std::cout);
    oArchive(*new_bomb);
    mBombs++;
  }
  return;
}

void bloke :: kick(bomb *bomb, uint8_t direction){
  return;
}

void bloke :: init(std::shared_ptr<AbstractPlayer> p = nullptr){
  mCollides = true;
  memset(&mPosition, 0, sizeof(double)*2);
  memset(&mVelocity, 0, sizeof(double)*2);
  mPosition[0] = 5;
  mPosition[1] = 3;
  if(p!=nullptr)
    mPlayerId = p->getId();
  return;
}

