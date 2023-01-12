#include "BomberBlokeScene.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL3/SDL.h>

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actor)

int
main(int, char**)
{
  _draw = false;

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine();

  const std::string username = "big_beef";
  _local_player_list.push_back(LocalPlayer(username));

  _pScene = std::make_shared<BomberBlokeScene>(10, 10);

  _halt = true;
  client_loop();

  SDL_Delay(2000);
  SDL_Quit();

  return 0;
}

void
gameUpdate()
{
  return;
}

void new_game(std::string)
{
  return;
}
