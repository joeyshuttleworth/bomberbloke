#include "engine.h"

void actor :: draw(){
  SDL_Rect rect;
  rect.w=ceil(dim[0] * _zoom);
  rect.h=ceil(dim[1] * _zoom);
  rect.x=round(position[0] * _zoom);
  rect.y=round((current_level->dim[1]-position[1]-dim[1]) * _zoom);
  SDL_BlitSurface(sprite, NULL, _surface, &rect);  
}

int actor :: move(double x, double y){
  double tmp_pos[2];
  bool in_level = true;
  bool moved = true;
  
  if(std::abs(x-position[0])+std::abs(y-position[1])<0.00001)
    moved = false;
  if(x > current_level->dim[0] - dim[0]){
    tmp_pos[0] = current_level->dim[0] - dim[0];
    velocity[0] = 0;
    in_level = false;
  }
  else if (x <= 0){
    tmp_pos[0]=0;
    velocity[0] = 0;
    in_level = false;
  }
  else{
    tmp_pos[0] = x;
  }
  if(y > current_level->dim[1] - dim[1]){
    tmp_pos[1] = current_level->dim[1]-dim[1];
    velocity[1] = 0;
    in_level = false;
  }
  else if (y < 0){
    tmp_pos[1] = 0;
    velocity[1] = 0;
    in_level = false;
  }
  else{
    tmp_pos[1] = y;
  }
  if(_net_on&&moved&&controller){
    net_message msg;
    msg.data = (char*)malloc(sizeof(Uint32)+4*sizeof(double));
    msg.operation = NET_MOVE;
    memcpy(msg.data, &(controller->id), sizeof(Uint32));
    memcpy(msg.data + sizeof(Uint32), position, sizeof(double)*2);
    memcpy(msg.data + sizeof(Uint32)+2*sizeof(double), velocity, 2*sizeof (double)); 
    net_add_message(&msg);
  }
  memcpy(position, tmp_pos, sizeof(double) * 2);
  if(in_level){
    return -1;
  }
  else
    return 0;
}

bool actor :: is_moving(){
  if(velocity[0] == 0 && velocity[1] == 0)
    return false;
  else
    return true;
}

actor :: actor(level *lvl, double x, double y){
  dim[0] = DEFAULT_ACTOR_SIZE;
  dim[1] = DEFAULT_ACTOR_SIZE;
  current_level=lvl;
  if(current_level->actor_list.size() > 0){
    id = current_level->actor_list.back().id + 1;
  }
  else{
    id = 0;
  }
  current_level->actor_list.push_back(*this);
  position[0] = x;
  position[1] = y;
  velocity[0] = 0;
  velocity[1] = 0;
  sprite = SDL_CreateRGBSurface(0,_zoom * dim[0],dim[1] * _zoom ,32, 0, 0, 0, 0);
  return;
}

double *actor :: get_midpoint(){
   double *rc = (double *)malloc(2*sizeof(double));
   rc[0] = position[0] + dim[0]/2;
   rc[1] = position[1] + dim[1]/2;
   return rc;
 }

void actor :: handle_command(std::string str){
  return;
}

void actor :: update(){
  return;
}

actor::actor(){
  dim[0] = DEFAULT_ACTOR_SIZE;
  dim[1] = DEFAULT_ACTOR_SIZE;
  return;
}

actor::~actor(){
  return;
}

