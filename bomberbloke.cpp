#include "bomberbloke.h"
#include <SDL2/SDL.h>

int    _default_bomb_time = DEFAULT_BOMB_TIMER;
double _bloke_size[2]     = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};

const std::vector<command_binding> _default_bindings =
  {{SDL_SCANCODE_W, "up"},
   {SDL_SCANCODE_S, "down"},
   {SDL_SCANCODE_A, "left"},
   {SDL_SCANCODE_D, "right"},
   {SDL_SCANCODE_J, "powerup"},
   {SDL_SCANCODE_K, "bomb"},
   {SDL_SCANCODE_P, "pause"}};

int main (int argc, char **argv){
  command_binding tmp_binding;
  SDL_Init(SDL_INIT_EVERYTHING);
  level *level1 = new level(10, 10);
  init_engine(level1);
  _local_player_list.push_back(local_p(std::string("Nickname")));
  bloke *bloke1 = new bloke(level1, 0, 0);
  bloke *bloke2 = new bloke(level1, double(10) ,0);
  _local_player_list.back().init(bloke1);
  bloke2->init(); 
  bloke1->init();
  bloke1->collides = true;
  bloke1->velocity[0] = 0.01;
  bloke2->velocity[0] =-0.01;
  SDL_FillRect(bloke1->sprite, NULL, SDL_MapRGB(bloke1->sprite->format, 0x00, 0xFF, 0xFF));
  game_loop(level1);
  SDL_Quit();
  return 0;
}
