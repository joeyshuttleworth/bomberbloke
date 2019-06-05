#include "engine.h"

void actor :: draw(){
  SDL_Rect dstrect;
  /*SDL_SetRenderTarget(_renderer, NULL);
  dstrect.w=ceil(dim[0] * _zoom);
  dstrect.h=ceil(dim[1] * _zoom);
  dstrect.x=round(position[0] * _zoom);
  dstrect.y=round((_level.dim[1]-position[1]-dim[1]) * _zoom);
  */
  SDL_RenderFillRect(_renderer, &dstrect);  
}

int actor :: move(double x, double y){
  double tmp_pos[2];
  bool in_level = true;
  bool moved = true;
  
  if(std::abs(x-position[0])+std::abs(y-position[1])<0.00001)
    moved = false;
  if(x > _level.dim[0] - dim[0]){
    tmp_pos[0] = _level.dim[0] - dim[0];
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
  if(y > _level.dim[1] - dim[1]){
    tmp_pos[1] = _level.dim[1]-dim[1];
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
    net_add_message(&msg, false);
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

actor :: actor(double x, double y){
  dim[0] = DEFAULT_ACTOR_SIZE;
  dim[1] = DEFAULT_ACTOR_SIZE;

  /*else{
    id = 0;
    }*/
  position[0] = x;
  position[1] = y;
  velocity[0] = 0;
  velocity[1] = 0;

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
  if(!sprite){
    sprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, 0, 128, 128);
    SDL_SetRenderTarget(_renderer, sprite);
    SDL_RenderClear(_renderer);
    SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0x00, 0xFF);
    SDL_RenderFillRect(_renderer, NULL);
    SDL_RenderPresent(_renderer);
    SDL_SetRenderTarget(_renderer, NULL);
  }
  dim[0] = DEFAULT_ACTOR_SIZE;
  dim[1] = DEFAULT_ACTOR_SIZE;
  remove = false;
  return;
}

actor::~actor(){
  return;
}

