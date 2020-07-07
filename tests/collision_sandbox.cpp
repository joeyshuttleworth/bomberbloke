#include "bomberbloke.h"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"
#include <network/NetClient.hpp>
#include "Explosion.hpp"

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actors)

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

  _server = true;
  _local_player_list.push_back(LocalPlayer(std::string("big_beef")));
  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine();

  set_draw(true);

  std::shared_ptr<bloke> b1(new bloke(1, 2, true));


  _pScene->addActor(b1);
  _local_player_list.back().setCharacter(b1);

  _pScene->addActor(std::shared_ptr<actor>(new bloke(3,3)));


  server_loop();

  return 0;
}

void gameUpdate(){
  return;
}

void new_game(std::string){
  return;
}









