#include "engine.hpp"
#include "MoveEvent.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>
#include <string>
#include <iostream>

int actor :: move(double x, double y){
  double tmp_pos[2];
  bool in_scene = true;

  /*  TODO: Change scene bound checking to just use the vertices so that
      it works for non-rectangular actors also  */

  /* Are we out of the left side of the scene? */
  if(x > _pScene->mDimmension[0] - mDimmension[0]){
    tmp_pos[0] = _pScene->mDimmension[0] - mDimmension[0];
    mVelocity[0] = 0;
    in_scene = false;
  }
  /* Are we too far right?*/
  else if (x <= 0){
    tmp_pos[0]=0;
    mVelocity[0] = 0;
    in_scene = false;
  }
  else{
    tmp_pos[0] = x;
  }

  /*Are we too high?*/
  if(y > _pScene->mDimmension[1] - mDimmension[1]){
    tmp_pos[1] = _pScene->mDimmension[1]-mDimmension[1];
    mVelocity[1] = 0;
    in_scene = false;
  }
  /*Are we too low?*/
  else if (y < 0){
    tmp_pos[1] = 0;
    mVelocity[1] = 0;
    in_scene = false;
  }
  else{
    tmp_pos[1] = y;
  }

  /*Move to our new coordinate*/
  mPosition[0] = tmp_pos[0];
  mPosition[1] = tmp_pos[1];

  /*Create a MoveEvent and send/save it*/

  MoveEvent e(this);
  // cereal::JSONOutputArchive oArchive(std::cout);
  //oArchive(e);


  if(in_scene){
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

actor :: actor(double x, double y, double xdim, double ydim, bool collides){

  /* TODO set mDimmension based of axis projections for non-square actors */
  mDimmension[0] = xdim;
  mDimmension[1] = ydim;

  mPosition[0] = x;
  mPosition[1] = y;
  mVelocity[0] = 0;
  mVelocity[1] = 0;

  mFrameVertices = {
    {{0., 0.}},
    {{mDimmension[0], 0.}},
    {{mDimmension[0], mDimmension[1]}},
    {{0., mDimmension[1]}}
  };

  mCollides = collides;

  return;
}

dvector actor :: getMidpoint(){
  return {mPosition[0] + mDimmension[0]/2, mPosition[1] + mDimmension[1]/2};
}

std::shared_ptr<AbstractPlayer> actor::getPlayer(){
  if(mPlayerId==0)
    return nullptr;
  else{
    /*Perform a horrible looking search over the _player_list*/
    auto iterator = std::find_if(_player_list.begin(), _player_list.end(), [&](std::shared_ptr<AbstractPlayer> p) -> bool {return p->getId() == mPlayerId;});
    if(iterator == _player_list.end()){
      /*We haven't found a player with the ID. This probably means that something has gone wrong*/
      log_message(WARNING, ("Unable to find controlling player for actor:" + std::to_string(mId)).c_str());
      return nullptr;
    }
    return *iterator;
  }
}


