#include "engine.h"
#include "MoveEvent.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>
#include <string>
#include <iostream>

void actor :: draw(){
  /*dstrect is a structure detailing the rectangle we will draw our sprite to */
  SDL_Rect dstrect;
  SDL_SetRenderTarget(_renderer, NULL);
  dstrect.w=ceil(mDimmension[0] * _zoom);
  dstrect.h=ceil(mDimmension[1] * _zoom);
  dstrect.x=round(mPosition[0] * _zoom);
  dstrect.y=round((_level.mDimmension[1]-mPosition[1]-mDimmension[1]) * _zoom);
  SDL_RenderFillRect(_renderer, &dstrect);
  return;
}

int actor :: move(double x, double y){
  double tmp_pos[2];
  bool in_level = true;
  bool moved = true;
  
  /* Are we out of the left side of the level? */
  if(x > _level.mDimmension[0] - mDimmension[0]){
    tmp_pos[0] = _level.mDimmension[0] - mDimmension[0];
    mVelocity[0] = 0;
    in_level = false;
  }
  /* Are we too far right?*/
  else if (x <= 0){
    tmp_pos[0]=0;
    mVelocity[0] = 0;
    in_level = false;
  }
  else{
    tmp_pos[0] = x;
  }
  
  /*Are we too high?*/
  if(y > _level.mDimmension[1] - mDimmension[1]){
    tmp_pos[1] = _level.mDimmension[1]-mDimmension[1];
    mVelocity[1] = 0;
    in_level = false;
  }
  /*Are we too low?*/
  else if (y < 0){
    tmp_pos[1] = 0;
    mVelocity[1] = 0;
    in_level = false;
  }
  else{
    tmp_pos[1] = y;
  }

  /*Move to our new coordinate*/
  mPosition[0] = tmp_pos[0];
  mPosition[1] = tmp_pos[1];

  /*Create a MoveEvent and send/save it*/

  MoveEvent e(this);
  cereal::JSONOutputArchive oArchive(std::cout);
  //oArchive(e);


  if(in_level){
    return -1;
  }
  else
    return 0;
}

bool actor :: is_moving(){
  if(mVelocity[0] == 0 && mVelocity[1] == 0)
    return false;
  else
    return true;
}

actor :: actor(double x, double y){
  mDimmension[0] = DEFAULT_ACTOR_SIZE;
  mDimmension[1] = DEFAULT_ACTOR_SIZE;

  mPosition[0] = x;
  mPosition[1] = y;
  mVelocity[0] = 0;
  mVelocity[1] = 0;

  return;
}

double actor :: get_midpoint(int index){
  if(index == 0){
   return mPosition[0] + mDimmension[0]/2;
  }
  else if(index==1){
    return mPosition[1] + mDimmension[1]/2;
  }
  log_message(ERROR, "error actor::get_midpoint was given a dodgy index!\n");
   return 0;
 }

void actor :: handle_command(std::string str){
  return;
}

void actor :: update(){
  return;
}

actor::actor(){ 
  if(!mpSprite){
    mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 128, 128);
    SDL_SetRenderTarget(_renderer, mpSprite);
    SDL_RenderClear(_renderer);
    SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0x00, 0xFF);
    SDL_RenderFillRect(_renderer, NULL);
    SDL_RenderPresent(_renderer);
    SDL_SetRenderTarget(_renderer, NULL);
  }
  mDimmension[0] = DEFAULT_ACTOR_SIZE;
  mDimmension[1] = DEFAULT_ACTOR_SIZE;
  mRemove = false;
  return;
}

player* actor::getPlayer(){
  if(mpControllingPlayer)
    return mpControllingPlayer;
  else{
    if(mPlayerId==0)
      return NULL;
    else{
      /*Perform a horrible looking search over the _player_list*/
      auto iterator = std::find_if(_player_list.begin(), _player_list.end(), [&](player p) -> bool {p.id == mPlayerId;});
    
      if(iterator == _player_list.end()){
        /*We haven't found a player with the ID. This probably means that something has gone wrong*/
        log_message(WARNING, ("Unable to find controlling player for actor:" + std::to_string(mId)).c_str());
        return NULL;
      }
      else{
        /*We have found the player. Store this point in mpControllingPlayer for later use.*/
        mpControllingPlayer = &(*iterator); // This gives a pointer to the player object. I don't know of any less ugly way.
        return mpControllingPlayer;
      }
    }
  }
}
void actor::setController(player* p){
  mPlayerId = p->id;
  mpControllingPlayer = p;
  return;
} 
