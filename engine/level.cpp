#include "level.h"
#include "engine.h"

level :: level (int dim_x, int dim_y){
  blocks = (uint8_t*) malloc(sizeof(uint8_t)*dim_x*dim_y);
  dim[0] = dim_x;
  dim[1] = dim_y;
  sprite = SDL_CreateRGBSurface(0, round(dim_x * _zoom), round(dim_y * _zoom), 32, 0, 0, 0, 0);
  if (!sprite){
    char msg[128];
    log_message(ERROR, SDL_GetError());
    exit(1);
  } 
  return;
}

level :: ~level(){
  dim[0] = 0;
  dim[1] = 0;
  free(blocks);
  blocks=NULL;
}

uint8_t level :: get_block(int row, int column){
  if(row < 0 || column < 0 || row > dim[0] || column > dim[1])
    return OUT_OF_BOUNDS;
  return blocks[row*dim[0]+column];
}

void level :: draw(){
  SDL_Rect rect;
  for(int i=0; i<dim[0];i++){
    for(int j=0;j<dim[1];j++){
      switch(get_block(i, j)){
      default:
	break;
      }
    }
  }
  rect.w = _zoom * dim[0];
  rect.h = _zoom * dim[1];
  rect.x = _screen_offset[0];
  rect.y = _screen_offset[1];
  SDL_FillRect(sprite, NULL, SDL_MapRGB(sprite->format, 0xFF, 0, 0xFF));
  SDL_BlitSurface(sprite, NULL, _surface, &rect);
  return;
}

bool level :: is_block_empty(actor *actor, int x, int y){
  return true;
}
