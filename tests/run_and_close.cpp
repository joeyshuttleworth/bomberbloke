#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actors)

int main (){
  _draw=false;

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine();

  const std::string username = "big_beef";
  _local_player_list.push_back(LocalPlayer(username));

  _pScene = std::make_shared<BomberBlokeScene>(10,10);

  _halt = true;
  client_loop();

  SDL_Delay(2000);
  SDL_Quit();

  return 0;
}

void gameUpdate(){
  return;
}

void new_game(std::string){
  return;
}
