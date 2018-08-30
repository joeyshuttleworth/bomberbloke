#include "bomberbloke.h"
#include <SDL2/SDL.h>

const std::vector<command_binding> _default_bindings =
  {{SDL_SCANCODE_W, "up"},
   {SDL_SCANCODE_S, "down"},
   {SDL_SCANCODE_A, "left"},
   {SDL_SCANCODE_D, "right"},
   {SDL_SCANCODE_J, "powerup"},
   {SDL_SCANCODE_K, "bomb"},
   {SDL_SCANCODE_P, "pause"}};

int main (){
  log_message(INFO, "Bomberbloke client starting...");
  init_engine();
  _local_player_list.push_back(local_p("nickname"));
  client_loop();
  SDL_Quit();
  return 0;
}

void new_game(std::string){
  return;
}
