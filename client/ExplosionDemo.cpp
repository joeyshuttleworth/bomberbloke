#include "BomberBlokeScene.hpp"
#include "Explosion.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include "SDLGraphicsManager.hpp"
#include <SDL2/SDL.h>

int
main()
{
  _draw = true;
  _server = false;
  _default_bindings = {
    { SDL_SCANCODE_W, "up" },      { SDL_SCANCODE_S, "down" },
    { SDL_SCANCODE_A, "left" },    { SDL_SCANCODE_D, "right" },
    { SDL_SCANCODE_J, "powerup" }, { SDL_SCANCODE_K, "bomb" },
    { SDL_SCANCODE_P, "pause" }
  };

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine(false);

  _graphics_interface = std::make_unique<SDLGraphicsManager>();
  _graphics_interface->createWindow();

  _pScene = std::make_shared<BomberBlokeScene>(_graphics_interface);

  _pScene->mActors.push_back(std::shared_ptr<bloke>(new bloke(5, 5, true)));
  _local_player_list.push_back(LocalPlayer(std::string("test_bloke")));


  cereal::JSONOutputArchive oArchive(std::cout);
  for (unsigned int i = 0; i < 10; i++)
    for (unsigned int j = 0; j < 10; j++) {
      _pScene->mParticles.push_back(std::shared_ptr<Explosion>(
        new Explosion(i, j, 1, 1, false, 60 + i + 2 * j, 600 - 2 * i - j)));
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
