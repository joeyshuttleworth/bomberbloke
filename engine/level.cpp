#include "engine.h"

level :: level (int dim_x, int dim_y){
  dim[0] = dim_x;
  dim[1] = dim_y;
  if(blocks)
    free(blocks);
  blocks = (uint8_t*)malloc(dim[0] * dim[1] * sizeof(uint8_t));
  sprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, 0, 1024, 1024);
  SDL_SetRenderTarget(_renderer, sprite);
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0xFF, 0xFF);
  SDL_RenderClear(_renderer);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_RenderPresent(_renderer);
  SDL_SetRenderTarget(_renderer, NULL);
  blocks = (uint8_t*)malloc(dim[0] * dim[1] * sizeof(uint8_t));
  return;
}

level :: level(){
  dim[0] = 10;
  dim[1] = 10;
  sprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, 0, 1024, 1024);
  SDL_SetRenderTarget(_renderer, sprite);
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0xFF, 0xFF);
  SDL_RenderClear(_renderer);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_RenderPresent(_renderer);
  SDL_SetRenderTarget(_renderer, NULL);
  blocks = (uint8_t*)malloc(dim[0] * dim[1] * sizeof(uint8_t));
  return;
}

level :: ~level(){
  free(blocks);
  blocks=NULL;
}

level :: level(const level &lvl){
  memcpy(sprite, lvl.sprite, sizeof(SDL_Surface));
  memcpy(blocks, lvl.blocks, dim[0]*dim[1]);
}

uint8_t level :: get_block(int row, int column){
  SDL_RenderClear(_renderer);
  if(row < 0 || column < 0 || row > dim[0] || column > dim[1])
    return OUT_OF_BOUNDS;
  return blocks[row*dim[0]+column];
}

void level :: draw(){
  SDL_Rect rect;
  int rgb[3];
  /*Draw backgroud blocks 
  for(int i=0; i<dim[0];i++){
    for(int j=0;j<dim[1];j++){
      switch(get_block(i, j)){
      default:
	memset(&rgb, 0, sizeof(int)*3);
	break;
      }
    }
  }
  rect.w = _zoom * dim[0];
  rect.h = _zoom * dim[1];
  rect.x = 0;
  rect.y = 0;
  */
  if(!sprite){
    sprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, 0, 1024, 1024);
  }
  SDL_RenderCopy(_renderer, sprite, NULL, NULL);
  //Next draw each actor
  for(auto i = actor_list.begin(); i!=actor_list.end(); i++){
    i->draw();
  }
  return;
}

bool level :: is_block_empty(actor *actor, int x, int y){
  return true;
}

void level :: init(){
  if(sprite)
    free(sprite);
  blocks = (uint8_t*)malloc(dim[0]*dim[1]*sizeof(uint8_t));
  if(!sprite){
    //log_message(ERROR, (char*)SDL_GetError());
  }
  return;
}
