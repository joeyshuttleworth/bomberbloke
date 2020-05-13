#include "bomberbloke.h"
#include <SDL2/SDL.h>

int main (){
  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine();
  SDL_Quit();
  return 0;
}
