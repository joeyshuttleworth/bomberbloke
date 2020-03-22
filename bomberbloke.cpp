#include "bomberbloke.h"
#include <SDL2/SDL.h>

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
  localPlayer p(std::string("big_beef"));
  _local_player_list.push_back(localPlayer(std::string("Nickname")));
  
  bloke *bloke1 = new bloke(10, 10);
  
  bloke1->init();
  bloke1->mCollides = false;

  //  bloke1.mVelocity[0]= 0.01;

  _level.actorList.push_back(bloke1);
  _local_player_list.back().character = _level.actorList.back();
  
  client_loop();
  
  SDL_Quit();

  delete(bloke1);
  return 0;
}


void new_game(std::string){
  return;
}









