#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"
#include "Explosion.hpp"

int main (){
  _draw=true;

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
  std::string username = "big_beef";
  init_engine();
  _local_player_list.push_back(LocalPlayer(username));

  std::shared_ptr<bloke> b1(new bloke(1,1));
  _level.mActors.push_back(b1);
  _local_player_list.back().init(b1);

  std::shared_ptr<Explosion> exp1(new Explosion(5,5,0.8, 0.8));
  // _particle_list.push_back(exp1);

  client_loop();

  SDL_Delay(2000);
  SDL_Quit();

  return 0;
}


void new_game(std::string){
  return;
}













