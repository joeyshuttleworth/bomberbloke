#include "bomberbloke.h"
#include "LocalPlayer.hpp"
#include "BomberBlokeScene.hpp"
#include "Explosion.hpp"
#include "MainMenuScene.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <getopt.h>
#include <network/NetClient.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

const bool EXPLOSION_INTRO = false;

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actor)

int
main(int argc, char** argv)
{
  std::string userName = "bloke";
  std::string serverAddress = "";
  bool autoConnect = false;

  int iarg = 0;
  const char* const short_opts = "u:s:";
  const option long_opts[] = { { "username", required_argument, nullptr, 'u' },
                               { "server", required_argument, nullptr, 's' },
                               { nullptr, no_argument, nullptr, 0 } };

  while (iarg != -1) {
    iarg = getopt_long(argc, argv, short_opts, long_opts, nullptr);
    switch (iarg) {
      case 'u':
        userName = std::string(optarg);
        break;
      case 's':
        serverAddress = std::string(optarg);
        break;
    }
  }

  if (userName.length() > 0 && serverAddress.length() > 0) {
    autoConnect = true;
  }

  _default_bindings = { { SDL_SCANCODE_W, "up" },
                        { SDL_SCANCODE_S, "down" },
                        { SDL_SCANCODE_A, "left" },
                        { SDL_SCANCODE_D, "right" },
                        { SDL_SCANCODE_J, "powerup" },
                        { SDL_SCANCODE_K, "bomb" },
                        { SDL_SCANCODE_P, "pause" },
                        { SDL_SCANCODE_MINUS, "zoom *.8" },
                        { SDL_SCANCODE_EQUALS, "zoom *1.25" },
                        { SDL_SCANCODE_F, "zoom follow" },
                        { SDL_SCANCODE_0, "zoom all" } };

  _local_player_list.push_back(LocalPlayer(userName));
  _nickname = userName;

  client_init();
  _pScene = std::make_shared<MainMenuScene>(_graphics_interface.get(), 15, 15);

  if (EXPLOSION_INTRO) {
    for (unsigned int i = 0; i < 10; i++) {
      for (unsigned int j = 0; j < 10; j++)
        _pScene->mParticles.push_back(std::shared_ptr<Explosion>(
                                                                    new Explosion(_graphics_interface.get(), i, j, 1, 1, 60 + i + 2 * j, 600 - 2 * i - j, 0)));
    }

    // Play intro music
    std::shared_ptr<Sound> pIntroSound =
      soundManager.createSound("explosion_intro");
    soundManager.playSound(pIntroSound);
    pIntroSound->mGroup = SOUND_FX;
  }

  if (autoConnect) {
    std::vector<std::string> commands = { "colour FFFFFFFF" };

    if (_net_client->joinBlokeServer(serverAddress, userName, commands)) {
      _pNewScene = std::make_shared<BomberBlokeScene>(_graphics_interface.get(), 10, 10);
    }
  }

#ifndef __EMSCRIPTEN__
  client_loop();
#else
  emscripten_set_main_loop(client_entry, 0, true);
#endif

  return 0;
}

void
gameUpdate()
{
  return;
}

void
new_game(std::string)
{
  return;
}
