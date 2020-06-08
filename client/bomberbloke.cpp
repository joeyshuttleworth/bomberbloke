#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"

const std::vector<command_binding> _default_bindings =
  {{SDL_SCANCODE_W, "up"},
   {SDL_SCANCODE_S, "down"},
   {SDL_SCANCODE_A, "left"},
   {SDL_SCANCODE_D, "right"},
   {SDL_SCANCODE_J, "powerup"},
   {SDL_SCANCODE_K, "bomb"},
   {SDL_SCANCODE_P, "pause"}
  };

int main (){
  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine();
  _local_player_list.push_back(localPlayer(std::string("big_beef")));
  cereal::JSONOutputArchive oArchive(std::cout);

  std::shared_ptr<bloke> b1(new bloke(5,5));
  _level.mActors.push_back(b1);
  _local_player_list.back().init(b1);
  oArchive(b1,  _level);

  client_loop();
  
  SDL_Quit();

  return 0;
}


void new_game(std::string){
  return;
}









