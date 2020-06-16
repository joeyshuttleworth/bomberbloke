#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"

int main (){
_draw=false;

SDL_Init(SDL_INIT_EVERYTHING);
init_engine();

 const std::string username = "big_beef";
_local_player_list.push_back(LocalPlayer(username));


std::shared_ptr<bloke> b1(new bloke(5,5));
_level.mActors.push_back(b1);
_local_player_list.back().setCharacter(b1);

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
