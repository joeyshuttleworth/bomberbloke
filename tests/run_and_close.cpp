#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"

const std::vector<CommandBinding> _default_bindings;

int main (){
  _draw=false;

  SDL_Init(SDL_INIT_EVERYTHING);

  init_engine();
  _local_player_list.push_back(LocalPlayer(std::string("big_beef")));
  cereal::JSONOutputArchive oArchive(std::cout);

  std::shared_ptr<bloke> b1(new bloke(5,5));
  _level.mActors.push_back(b1);
  _local_player_list.back().init(b1);
  oArchive(b1,  _level);

  _halt = true;
  client_loop();

  SDL_Delay(2000);
  SDL_Quit();

  return 0;
}


void new_game(std::string){
  return;
}









