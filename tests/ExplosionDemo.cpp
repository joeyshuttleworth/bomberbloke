#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"
#include "Explosion.hpp"

int main (){
  _draw=true;
  _server=true;
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

  for(unsigned int i = 0; i < 10; i++)
    for(unsigned int j = 0; j < 10; j++){
      _particle_list.push_back(std::shared_ptr<Explosion>(new Explosion(i, j, 1, 1, 60 + i + 2*j, 600 - 2*i - j)));
  }

  client_loop();

  SDL_Delay(2000);
  SDL_Quit();

  return 0;
}

void gameUpdate(){
  if(_tick > 2000){
    return;
  }
  return;
}

void new_game(std::string){
  return;
}













