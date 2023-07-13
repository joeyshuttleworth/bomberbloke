#include "BomberBlokeScene.hpp"
#include "Explosion.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>

int
main()
{
  _draw = true;
  _server = true;
  _default_bindings = {
    { SDL_SCANCODE_W, "up" },      { SDL_SCANCODE_S, "down" },
    { SDL_SCANCODE_A, "left" },    { SDL_SCANCODE_D, "right" },
    { SDL_SCANCODE_J, "powerup" }, { SDL_SCANCODE_K, "bomb" },
    { SDL_SCANCODE_P, "pause" }
  };

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine(false);

  _pScene = std::make_shared<BomberBlokeScene>();

  _pScene->mActors.push_back(std::shared_ptr<bloke>(new bloke(5, 5, true)));
  cereal::JSONOutputArchive oArchive(std::cout);

  for (unsigned int i = 0; i < 10; i++)
    for (unsigned int j = 0; j < 10; j++) {
      _pScene->mParticleList.push_back(std::shared_ptr<Explosion>(
        new Explosion(i, j, 1, 1, 60 + i + 2 * j, 600 - 2 * i - j)));
    }

  client_loop();

  SDL_Delay(2000);
  SDL_Quit();

  return 0;
}

void
gameUpdate()
{
  if (_tick > 600) {
    _halt = true;
    return;
  }
  return;
}

void new_game(std::string)
{
  return;
}
