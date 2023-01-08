#include "Explosion.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "bomberbloke.h"
#include "SDL.h"
#include <network/NetClient.hpp>

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actors)

int
main()
{
  _default_bindings = {
    { SDL_SCANCODE_W, "up" },      { SDL_SCANCODE_S, "down" },
    { SDL_SCANCODE_A, "left" },    { SDL_SCANCODE_D, "right" },
    { SDL_SCANCODE_J, "powerup" }, { SDL_SCANCODE_K, "bomb" },
    { SDL_SCANCODE_P, "pause" }
  };

  _server = true;
  _local_player_list.push_back(LocalPlayer(std::string("big_beef")));
  SDL_Init(SDL_INIT_EVERYTHING);

  _draw = true;
  init_engine();

  _pScene = std::make_shared<BomberBlokeScene>(10, 10);
  _pScene->addActor(
    std::shared_ptr<actor>(std::make_shared<bloke>(1, 2, true)));
  _pScene->addActor(std::shared_ptr<actor>(std::make_shared<bloke>(3, 3)));
  _pScene->linkActorToPlayer(_pScene->mActors.back(),
                             _local_player_list.back().getId());

  server_loop();

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
