#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"

int main (){

   _default_bindings =
    {{SDL_SCANCODE_W, "up"},
     {SDL_SCANCODE_S, "down"},
     {SDL_SCANCODE_A, "left"},
     {SDL_SCANCODE_D, "right"},
     {SDL_SCANCODE_J, "powerup"},
     {SDL_SCANCODE_K, "bomb"},
     {SDL_SCANCODE_P, "pause"}
    };

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine();
  _local_player_list.push_back(LocalPlayer(std::string("big_beef")));
  cereal::JSONOutputArchive oArchive(std::cout);

  std::shared_ptr<bloke> b1(new bloke(1, 2, true));
  _level.mActors.push_back(b1);
  _local_player_list.back().setCharacter(b1);
  _level.mActors.push_back(std::shared_ptr<bloke>(new bloke(3, 3, true)));
  // oArchive(b1,  _level);

  client_loop();
  SDL_Quit();

  return 0;
}

void gameUpdate(){
  return;
}

void new_game(std::string){
  return;
}









