#include "engine.h"
#include "actor.h"
#include "level.h"
void actor :: draw(){
  SDL_Rect rect;
  rect.w=round(dim[0] * _zoom);
  rect.h=round(dim[1] * _zoom);
  rect.x=_screen_offset[0] + round((position[0]) * _zoom);
  rect.y=_screen_offset[1] + round((current_level->dim[1]-position[1]-dim[1]) * _zoom);
  SDL_BlitSurface(sprite, NULL, _surface, &rect);  
}

int actor :: move(double x, double y){
  double tmp_pos[2];
  bool in_level = true;
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
  if(current_level->is_block_empty(this, round(tmp_pos[0]), round(tmp_pos[1]))){
    memcpy(position, tmp_pos, sizeof(double) * 2);
  }
  else if(in_level){
    return -1;
  }
  else
    return -2;
  return 0;
}

bool actor :: is_moving(){
  if(velocity[0] == 0 && velocity[1] == 0)
    return false;
  else
    return true;
}

actor :: actor(level *level, double x, double y){
  dim[0] = DEFAULT_ACTOR_SIZE;
  dim[1] = DEFAULT_ACTOR_SIZE;
  sprite = SDL_CreateRGBSurface(0,_zoom * dim[0],dim[1] * _zoom ,32, 0, 0, 0, 0);
  current_level=level;
  id = current_level->actor_list.size();
  current_level->actor_list.push_back(this);
  if(!current_level->is_block_empty(this, round(x), round(y))){
    log_message(ERROR, "Invalid initial position");
  }
  else{
    move(x, y);
  }
  velocity[0] = 0;
  velocity[1] = 0;
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
